#include <QtGui>
#include <QtOpenGL>
#include <QDebug>

#include "image.h"


//////////////////////////////////////////
// Constructor
// CImage
CImage::CImage()
{
    m_Histogram.setImage(this);
    m_HistoMax = 1;
    m_HistoMin = 0;
}

CImage::CImage(QString fileName)
{
    m_Histogram.setImage(this);
    m_fileName = fileName;

    // Load file
    if(!m_OriginalImage.load(fileName))
        throw "can not load file name";

    // Initialize image
    CalculateGrayScale();
    CalculateHistogram();
}

//////////////////////////////////////////////////
// reset to original image testing
void CImage::resetHistogram()
{
    m_HistoMax = 1;
    m_HistoMin = 0;
    m_CDFMin   = 0;
    // Initialize image
    CalculateGrayScale();
    CalculateHistogram();
}

//////////////////////////////////////////////////
// load image
bool CImage::load(QString fileName)
{
    // Load file
    if(!m_OriginalImage.load(fileName))
        return false;

    // Initialize image
    CalculateGrayScale();
    CalculateHistogram();

    m_fileName = fileName;
    m_Histogram.updateGL();

    return true;
}

void CImage::updateHistogram()
{
    m_Histogram.updateGL();
}

/////////////////////////////////////////
// Create GrayImage
void CImage::CalculateGrayScale()
{
    if(m_OriginalImage.isNull())
        return;

    m_GrayImage = QImage(m_OriginalImage);

    QRgb col;
    float gray;
    float width  = m_GrayImage.width();
    float height = m_GrayImage.height();
    for (float i = 0; i < width; ++i)
    {
        for (float j = 0; j < height; ++j)
        {
            col = m_GrayImage.pixel(i, j);
            gray = qGray(col);
            m_GrayImage.setPixel(i, j, qRgb(gray, gray, gray));
        }
    }
}

////////////////////////////////////
// Create histogram data
void CImage::CalculateHistogram()
{
    m_rgHistogram.clear();
    m_rgCDFHistogram.clear();
    m_HistoMin = 0;
    m_HistoMax = 1;
    m_CDFMin = 0;

    // initialize Histogram vector
    for(int i=0; i<256; i++)
    {
        m_rgHistogram.push_back(0);
        m_rgCDFHistogram.push_back(0);
    }

    // Calculate histogram
    QRgb col;
    float gray;
    float width  = m_GrayImage.width();
    float height = m_GrayImage.height();
    for (float i = 0; i < width; ++i)
    {
        for (float j = 0; j < height; ++j)
        {
            col = m_GrayImage.pixel(i, j);
            gray = qGray(col);
            m_rgHistogram[gray]++;

            // Find histogram max value
            if(m_HistoMax < m_rgHistogram[gray])
                m_HistoMax = m_rgHistogram[gray];
        }
    }

    // Calculate Cumulative histogram
    int sum = 0;
    for(int i =0; i< m_rgCDFHistogram.size(); i++)
    {
        sum += m_rgHistogram[i];
        m_rgCDFHistogram[i] = sum;

        // Find CDFMin !=0
        if(sum!= 0 && m_CDFMin == 0)
            m_CDFMin = sum;
    }

}

///////////////////////////////////////////////////////
// Get Histogram equalization for a grayscale value
// h(v) = ((CDF(v) - CDFMin)*(L-1))/(M*N - CDFMin)
// Where L = 256
// M*N = total number of pixels
// CDFMin = minimum valude of Cumulative distribution function
float CImage::getEqualizedGrayScale(float grayIndex)
{
   // int equalizedGray = m_rgHistogram[grayIndex];

    int CDFv = m_rgCDFHistogram[grayIndex];
    int width  = m_GrayImage.width();
    int height = m_GrayImage.height();

    float result = (CDFv - m_CDFMin)*255.0f;
    result = (result*1.0f)/(width*height - m_CDFMin)*1.0f;

    return result;
}

///////////////////////////////////////////////////////
// Recalculate grayscale and write it back to output image
// Will change ImageGrayScale and Histogram as a whole
// Need to reset to get back to grayScale image
void CImage::equalizeHitogram()
{
    // Recalculate gray scale
    QRgb col;
    float gray;
    float width  = m_GrayImage.width();
    float height = m_GrayImage.height();
    for (float i = 0; i < width; ++i)
    {
        for (float j = 0; j < height; ++j)
        {
            col = m_GrayImage.pixel(i, j);
            gray = qGray(col);

            float equalizedGray = getEqualizedGrayScale(gray);
            m_GrayImage.setPixel(i, j, qRgb(equalizedGray, equalizedGray, equalizedGray));
        }
    }

    // Recalculate Histograms
    CalculateHistogram();
}

///////////////////////////////////////////////////////
// Recalculate grayscale and write it back to output image
// match histogram
// find grayScale g of old grayscale f sothat p(f) < pd(g)
void CImage::matchHistogram( QVector<float> refCDF)
{
    // Recalculate gray scale
    QRgb col;
    float gray;
    float width  = m_GrayImage.width();
    float height = m_GrayImage.height();
    for (float i = 0; i < width; ++i)
    {
        for (float j = 0; j < height; ++j)
        {
            col = m_GrayImage.pixel(i, j);
            gray = qGray(col);

            float matchedGray = getMatchedGrayScale(gray,refCDF);
            m_GrayImage.setPixel(i, j, qRgb(matchedGray, matchedGray, matchedGray));
        }
    }

    // Recalculate Histograms
    CalculateHistogram();
}

float CImage::getMatchedGrayScale(float grayIndex, const QVector<float> &refCDF)
{
    // find OldCDF
    int OldCDF = m_rgCDFHistogram[grayIndex];

    // find newCDF sothat OldCDF<newCDF
    for(int i=0; i< refCDF.size(); i++)
    {
        if(OldCDF < refCDF[i])
            return i;
    }

    return grayIndex;
}

//////////////////////////////////////////
// Override original image with grayImage
void CImage::overrideOriginalImage()
{
    if(m_GrayImage.isNull())
        return;

    m_OriginalImage = QImage(m_GrayImage);
}

//////////////////////////////////////////
// Override original image
void CImage::setOriginalImage(const QImage& image)
{
    m_OriginalImage = QImage(image);
}

//////////////////////////////////////////
QImage CImage::getOriginalImage()
{
    return m_OriginalImage;
}

/////////////////////////////////////////
// Blur image
void CImage::applyFilter(const QVector<float> &rgFilter, int FilterWidth, int FilterHeight, bool redChannel, bool greenChannel, bool blueChannel)
{
    if(m_OriginalImage.isNull())
        return;

    m_GrayImage = QImage(m_OriginalImage);

    // get color valur for each pixel
    float width  = m_GrayImage.width();
    float height = m_GrayImage.height();
    for (float i = 0; i < width; ++i)
    {
        for (float j = 0; j < height; ++j)
        {
            // Original color
            QRgb OriginalCol = m_GrayImage.pixel(i, j);
            QRgb col = getColour(i,j,rgFilter,FilterWidth,FilterHeight);

            // Only apply the approviate channels
            int red   = (redChannel)  ?qRed(col):qRed(OriginalCol);
            int green = (greenChannel)?qGreen(col):qGreen(OriginalCol);
            int blue  = (blueChannel) ?qBlue(col):qBlue(OriginalCol);

            m_GrayImage.setPixel(i, j, qRgb(red,green,blue));
        }
    }

    // Recalculate Histograms
    CalculateHistogram();
}

/////////////////////////////////////////////////////////////////////
// Get colour out from the filter
QRgb CImage::getColour(int x, int y, const QVector<float> &rgFilter, int filterWidth, int filterHeight)
{
    // Get Colour from neighbour hood
    float red   = 0.0f;
    float green = 0.0f;
    float blue  = 0.0f;
    int wOffset = filterWidth/2;
    int hOffset = filterHeight/2;

    int imageWidth  = m_GrayImage.width();
    int imageHeight = m_GrayImage.height();

    // Go through the matrix filter
    for (int i = 0; i < filterWidth; i++)
    {
      for (int j = 0; j < filterHeight; j++)
      {
        int xloc = x+i-wOffset;
        int yloc = y+j-hOffset;

        int filterLoc = i + j*filterWidth;

        // get colour for this pixel
        if(xloc <0 || xloc >= imageWidth || yloc<0 || yloc>=imageHeight)
        {
            // out of bound, do nothing
        }
        else
        {
            QRgb col = m_GrayImage.pixel(xloc, yloc);
            red   += rgFilter.at(filterLoc)*qRed  (col)*1.0f;
            green += rgFilter.at(filterLoc)*qGreen(col)*1.0f;
            blue  += rgFilter.at(filterLoc)*qBlue (col)*1.0f;
        }
      }
    }

    // Makesure the color is not out of bound
    if(red   < 0  ) red   =0;
    if(red   > 255) red   =255;
    if(green < 0  ) green =0;
    if(green > 255) green =255;
    if(blue  < 0  ) blue  =0;
    if(blue  > 255) blue  =255;

    // Calculate with filter matrix
    QRgb color = qRgb(red,green,blue);
    return color;
}
