#include <QtGui>
#include <QtOpenGL>
#include <QDebug>

#include "image.h"

#define OFFSET_X 10
#define OFFSET_Y 30


/////////////////////////////////////////
// CRegion
CRegion::CRegion(const QPoint &origin, const QPoint &end, const RGBColor &colour, QWidget * rubberbandWidget)
    :m_RubberBand(QRubberBand::Rectangle,rubberbandWidget)
{
    m_RGBColor  = colour;
    m_origin    = QPoint(origin.x() - OFFSET_X, origin.y() - OFFSET_Y);
    m_endPoint  = QPoint(end.x()    - OFFSET_X, end.y()    - OFFSET_Y);

    // Origin == TopLeft to End == BotRight
    if(m_origin.x() <= m_endPoint.x() && m_origin.y() <= m_endPoint.y())
    {
        m_topLeft   = m_origin;
        m_botRight  = m_endPoint;
        m_topRight  = QPoint(m_botRight.x(), m_topLeft.y());
        m_botLeft   = QPoint(m_topLeft.x() , m_botRight.y());
    }
    else
    // Origin == botRight, End == TopLeft
    if(m_origin.x() > m_endPoint.x() && m_origin.y() > m_endPoint.y())
    {
        m_topLeft   = m_endPoint;
        m_botRight  = m_origin;
        m_botLeft   = QPoint(m_topLeft.x()  , m_botRight.y());
        m_topRight  = QPoint(m_botRight.x() , m_topLeft.y());
    }
    else
    // Origin == TopRight, End == BotLeft
    if(m_origin.x() > m_endPoint.x() && m_origin.y() <= m_endPoint.y())
    {
        m_botLeft   = m_endPoint;
        m_topRight  = m_origin;
        m_topLeft   = QPoint(m_botLeft.x()  , m_topRight.y());
        m_botRight  = QPoint(m_topRight.x() , m_botLeft.y());
    }
    else
    // Origin == BotLeft, End == TopRight
    if(m_origin.x() <= m_endPoint.x() && m_origin.y() > m_endPoint.y())
    {
        m_botLeft   = m_origin;
        m_topRight  = m_endPoint;
        m_topLeft   = QPoint(m_botLeft.x()  , m_topRight.y());
        m_botRight  = QPoint(m_topRight.x() , m_botLeft.y());
    }

    m_center    = QPoint((m_topLeft.x() + m_topRight.x())/2.0f,(m_topLeft.y() + m_botLeft.y())/2.0f);

    // offSet rubberBand location
    m_RubberBand.setGeometry(QRect(m_topLeft, m_botRight));
 }

void CRegion::filter(const QVector<float> &filter, int width, int height)
{
    m_filterWidth = width;
    m_filterHeight = height;

    m_rgFilters.clear();

    for(int i=0; i< filter.size(); i++)
    {
        m_rgFilters.push_back(filter.at(i));
    }
}

void CRegion::show(bool show)
{
    (show)? m_RubberBand.show():m_RubberBand.hide();
}

bool CRegion::containsPoint(QPoint point)
{
    // Offset point
    QPoint offsetPoint(point.x() - OFFSET_X, point.y() - OFFSET_Y);

    // Check point
    if( offsetPoint.x() < m_topRight.x() &&
        offsetPoint.x() > m_topLeft.x() &&
        offsetPoint.y() < m_botLeft.y() &&
        offsetPoint.y() > m_topLeft.y())
        return true;

    return false;
}

void CRegion::moveTo(int dx, int dy)
{
    // set centre
    m_center.setX(m_center.x() + dx);
    m_center.setY(m_center.y() + dy);

    // bounding rectangle
    m_topLeft.setX(m_topLeft.x() + dx);
    m_topLeft.setY(m_topLeft.y() + dy);
    m_topRight.setX(m_topRight.x() + dx);
    m_topRight.setY(m_topRight.y() + dy);
    m_botLeft.setX(m_botLeft.x() + dx);
    m_botLeft.setY(m_botLeft.y() + dy);
    m_botRight.setX(m_botRight.x() + dx);
    m_botRight.setY(m_botRight.y() + dy);

    m_origin.setX(m_origin.x() + dx);
    m_origin.setY(m_origin.y() + dy);
    m_endPoint.setX(m_endPoint.x() + dx);
    m_endPoint.setY(m_endPoint.y() + dy);

    // Set rubberband
    m_RubberBand.setGeometry(QRect(m_topLeft, m_botRight));
}
