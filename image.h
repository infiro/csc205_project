#ifndef IMAGE_H
#define IMAGE_H

#include <QMainWindow>
#include <QVector>
#include "histogram.h"

//////////////////////////////////////////////////////////////////////
// Have all information about this image
class CImage
{
public:
    CImage();
    CImage(QString filename);
    ~CImage(){};
    void updateHistogram();
    bool load(QString filename);
    bool isNull() const {return m_OriginalImage.isNull();}
    QPixmap getOriginalPixmap()const {return QPixmap::fromImage(m_OriginalImage);}
    QPixmap getGrayPixmap()    const {return QPixmap::fromImage(m_GrayImage);}
    QString getName() const {return m_fileName;}

    QVector<float> getGrayHistogram() const {return m_rgHistogram;}
    QVector<float> getCumulativeHistogram() const {return m_rgCDFHistogram;}
    CHistogramWidget* getHistogramWidget(){return &m_Histogram;}

    void overrideOriginalImage();
    void setOriginalImage(const QImage& image);
    QImage getOriginalImage();

    // Histogram manipulation
    void resetHistogram();
    void equalizeHitogram();
    void matchHistogram( QVector<float> refCDF);

    void applyFilter(const QVector<float> &rgFilter, int width, int height, bool redChannel, bool greenChannel, bool blueChannel);
    QRgb getColour(int i, int j, const QVector<float> &rgFilter, int width, int height);

    //Image width & height
    int imageWidth() const {return m_OriginalImage.width();}
    int imageHeight() const {return m_OriginalImage.height();}

private:
    void CalculateGrayScale();
    void CalculateHistogram();

    float getEqualizedGrayScale(float grayIndex);
    float getMatchedGrayScale(float grayIndex, const QVector<float> &refCDF);

private:
    QString m_fileName;

    QImage m_OriginalImage;
    QImage m_GrayImage;

    int m_HistoMax;
    int m_HistoMin;

    int m_CDFMin;

    // Histogram
    QVector<float> m_rgHistogram;
    QVector<float> m_rgCDFHistogram;

    CHistogramWidget m_Histogram;

    friend class CHistogramWidget;
};
#endif // IMAGE_H
