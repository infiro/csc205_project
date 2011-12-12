#ifndef IMAGE_H
#define IMAGE_H

#include <QMainWindow>
#include <QVector>
#include <QtGui>
#include "math.h"
#include "histogram.h"


struct RGBColor
{
   GLfloat r;
   GLfloat g;
   GLfloat b;

   RGBColor(GLfloat inR, GLfloat inG, GLfloat inB)
   {
       r = inR;
       g = inG;
       b = inB;
   }

   RGBColor() { r=0.0f ; g=0.0f ; b=0.0f;}

};

///////////////////////////////////////////////////////////////////////////
// Region
class CRegion
{
public:
    CRegion():m_RubberBand(QRubberBand::Rectangle){}
    CRegion(const QPoint &origin, const QPoint &end, QWidget * rubberbandWidget);
    ~CRegion(){}

    int  Z_Level() const {return m_ZLevel;}
    void Z_Level( int ZLevel){m_ZLevel = ZLevel;}

    QRubberBand* rubberBand() {return &m_RubberBand;}
    void show(bool show);
    bool containsPoint(QPoint point);
    bool containsPixel(float x, float y);
    void moveTo(int dx, int dy);

    QPoint convertToMouseP(float x, float y);

    const QPoint&   getCenter()   const {return m_center;}
    const QPoint&   getTopLeft()  const {return m_topLeft;}
    const QPoint&   getBotRight() const {return m_botRight;}
    const QPoint&   getBotLeft()  const {return m_botLeft;}
    const QPoint&   getTopRight() const {return m_topRight;}
    const QPoint&   getOrigin()   const {return m_origin;}
    const QPoint&   getEndPoint() const {return m_endPoint;}

    void center( QPoint center) {m_center =QPoint(center.x(), center.y());}
    void topleft(const QPoint& topLeft) {m_topLeft = topLeft;}
    void topright(const QPoint& topRight) {m_topRight = topRight;}
    void botleft(const QPoint& botLeft) {m_botLeft = botLeft;}
    void botright(const QPoint& botRight) {m_botRight = botRight;}
    void origin(const QPoint& origin) {m_origin = origin;}
    void endpoint(const QPoint& endPoint) {m_endPoint = endPoint;}

    void setTopLeft (const QPoint& point);
    void setBotRight(const QPoint& point);

private:
    QRubberBand m_RubberBand;
    QWidget* m_ImageLabel;

    QPoint m_center;
    QPoint m_origin;
    QPoint m_endPoint;

    int m_ZLevel;
    QPoint m_topLeft;
    QPoint m_topRight;
    QPoint m_botLeft;
    QPoint m_botRight;

};


//////////////////////////////////////////////////////////////////////
// Have all information about this image
class MainWindow;
class CImage
{
public:
    CImage();
    CImage(QString filename);
    ~CImage(){};
    void updateHistogram();
    bool load(QString filename);
    bool load(const QImage& inputImage, QString filename);
    bool save(QString filename);

    bool isNull() const {return m_OriginalImage.isNull();}
    QPixmap getOriginalPixmap()const {return QPixmap::fromImage(m_OriginalImage);}
    QPixmap getGrayPixmap()    const {return QPixmap::fromImage(m_GrayImage);}
    QString getName() const {return m_fileName;}

    QVector<float> getGrayHistogram() const {return m_rgHistogram;}
    QVector<float> getCumulativeHistogram() const {return m_rgCDFHistogram;}
    CHistogramWidget* getHistogramWidget(){return &m_Histogram;}

    void overrideOriginalImage();
    void setOriginalImage(const QImage& image);
    const QImage& getOriginalImage() const {return m_OriginalImage;}

    // Histogram manipulation
    void resetHistogram();
    void equalizeHitogram();
    void matchHistogram( QVector<float> refCDF);

    void applyFilter(const QVector<float> &rgFilter, int width, int height, bool redChannel, bool greenChannel, bool blueChannel);
    void applyFilter(MainWindow* window);
    void tiltShift(MainWindow* window, int target_z, QPoint selectedPosition);
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
