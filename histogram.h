#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QGLWidget>

class CImage;
//This is our OpenGL Component we built it on top of QGLWidget
class CHistogramWidget : public QGLWidget
{
    Q_OBJECT

public:
    CHistogramWidget(QWidget *parent = 0);
    ~CHistogramWidget();

    void setImage(CImage* image);
protected:
    //Initialize the OpenGL Graphics Engine
    void initializeGL();

    //All our painting stuff are here
    void paintGL();

    //When user resizes main window, the scrollArea will be resized and it will call this function from
    //its attached GLWidget
    void resizeGL(int width, int height);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    QPoint      m_lastPos;
    CImage      *m_pImage;
};
#endif // HISTOGRAM_H
