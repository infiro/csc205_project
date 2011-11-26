#include <QtGui>
#include <QtOpenGL>

#include <math.h>
#include "histogram.h"
#include "image.h"

///////////////////////////////////////////////////////////////////////
CHistogramWidget::CHistogramWidget(QWidget *parent)
    : QGLWidget(parent)
{
}

CHistogramWidget::~CHistogramWidget()
{
}
void CHistogramWidget::setImage(CImage *image)
{
    m_pImage = image;
}

void CHistogramWidget::initializeGL()
{
    //Background color will be white
    glClearColor(1.0, 1.0, 1.0, 1.0);

    //Enable features we want to use from OpenGL
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void CHistogramWidget::paintGL()
{
    //Clear target buffer and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glColor3f(1.0f,0.0f,1.0f);

    // Set up dimension
    int width = this->width();
    int height = this->height();

    float h_space = width/256.0f;
    float v_space   = height*1.0f/(m_pImage->m_HistoMax*1.0f);

    QVector<float> grayValue = m_pImage->m_rgHistogram;

    // Draw histogram
    glBegin(GL_LINES);
    for(int i=0; i< grayValue.size(); i++)
    {
        glVertex3f(i*h_space  ,1.0f, 0.0f);
        glVertex3f(i*h_space  ,grayValue[i]*v_space, 0.0f);
    }
    glEnd();

    glFlush();
}

void CHistogramWidget::resizeGL(int width, int height)
{
    //Correction for aspect ratio
    glViewport(0, 0, width, height);

    //Set projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (float)width, 0.0, (float)height);

    //Return to ModelView Matrix
    glMatrixMode(GL_MODELVIEW);
}

//////////////////////////////////////////////////////////////////
// Mouse press
void CHistogramWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
}

////////////////////////////////////////////////////////////////
// mouse move
void CHistogramWidget::mouseMoveEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
}

////////////////////////////////////////////////////////////////
// mouse release
void CHistogramWidget::mouseReleaseEvent(QMouseEvent *event)
{
     m_lastPos = event->pos();

}
