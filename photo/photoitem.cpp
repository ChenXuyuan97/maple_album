#include "photoitem.h"
#include <QPainter>
#include <QPropertyAnimation>
#include <QGraphicsView>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneWheelEvent>
#include <math.h>
#ifdef QT_DEBUG
#include <QDebug>
#endif

PhotoItem::PhotoItem(QGraphicsItem *parent ) : QGraphicsItem(parent)
{
    m_nWidth = 0;
    m_nHeight = 0;
    m_bScaled = false;
    m_bRPressed = false;
    m_angle = 0.0;
    setFlags(QGraphicsItem::ItemIsMovable |
             QGraphicsItem::ItemIsFocusable |
             QGraphicsItem::ItemIsSelectable |
             QGraphicsItem::ItemSendsGeometryChanges);
}

PhotoItem::~PhotoItem()
{

}

void PhotoItem::Create(QString path)
{
    m_srcImg.load(path);
    int width = m_srcImg.width();
    int height = m_srcImg.height();
    m_ratio = (width * 1.0f) / height;
    if(width > P_MIN_WIDTH)
        m_scaledImg = m_srcImg.scaled(P_MIN_WIDTH, P_MIN_WIDTH / m_ratio);
    else
        m_scaledImg = m_srcImg.scaled(width, height);
    m_nWidth = m_scaledImg.width() + P_PADDING;
    m_nHeight = m_scaledImg.height() + P_PADDING;
    update();
}

void PhotoItem::Create(const QPixmap &pixmap)
{
    m_srcImg = pixmap.toImage();
    int width = m_srcImg.width();
    int height = m_srcImg.height();
    m_ratio = (width * 1.0f) / height;
    if(width > P_MIN_WIDTH)
        m_scaledImg = m_srcImg.scaled(P_MIN_WIDTH, P_MIN_WIDTH / m_ratio);
    else
        m_scaledImg = m_srcImg.scaled(width, height);
    m_nWidth = m_scaledImg.width() + P_PADDING;
    m_nHeight = m_scaledImg.height() + P_PADDING;
    update();
}

const QImage &PhotoItem::GetSrcImage() const
{
    return m_srcImg;
}

const QImage &PhotoItem::GetScaledImage() const
{
    return m_scaledImg;
}

double PhotoItem::GetRatio() const
{
    return m_ratio;
}

void PhotoItem::Scale(int srcWidth, double scale)
{
    int newWidth = srcWidth + ZOOM_WIDTH * scale;
    m_scaledImg = m_srcImg.scaled(newWidth, newWidth / m_ratio, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    m_nWidth = newWidth + P_PADDING;
    m_nHeight = (newWidth / m_ratio) + P_PADDING;
}

void PhotoItem::Update()
{
    QGraphicsView* view = scene()->views()[0];
    view->viewport()->update();
}

void PhotoItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter->fillRect(0, 0, m_nWidth, m_nHeight, Qt::white);
    QStyle::State state = option->state;
    if( ((state & (~QStyle::State_Selected)) == (state ^ QStyle::State_Selected)) ||
            ((state & (~QStyle::State_HasFocus)) == (state ^ QStyle::State_HasFocus)) )
    {
        QPen pen(Qt::red);
        pen.setStyle(Qt::PenStyle::DashLine);
        pen.setWidth(1);
        painter->setPen(pen);
    }
    else
    {
        QPen pen(Qt::white);
        pen.setStyle(Qt::PenStyle::SolidLine);
        painter->setPen(pen);
    }

    painter->drawRect(0, 0, m_nWidth, m_nHeight);
    painter->drawImage(P_PADDING / 2, P_PADDING / 2, m_scaledImg);
}

QRectF PhotoItem::boundingRect() const
{
    return QRectF(0, 0, m_nWidth, m_nHeight);
}

void PhotoItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit selected(this);
    if(event->button() == Qt::RightButton)
    {
        m_trans = transform();
        if(!m_bRPressed)
            m_bRPressed = true;
    }
    else
    {
        if(m_bRPressed)
            m_bRPressed = false;
    }
}

void PhotoItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
#ifdef QT_DEBUG
    qDebug() << "item_pos is " << scenePos();
#endif
    if(event->button() == Qt::NoButton)
    {
        if(m_bRPressed) // 右键按下
        {
            QVector2D vec1(event->scenePos().x(), event->scenePos().y());
            vec1.normalize();
            QVector2D vec2(event->pos().x(), event->pos().y());
            vec2.normalize();
            qreal ret = QVector2D::dotProduct(vec1, vec2);
            if(ret > 1.0)
                ret = 1.0;
            if(ret < -1.0)
                ret = -1.0;

            qreal ret2 = QVector3D::crossProduct(QVector3D(vec1, 1.0), QVector3D(vec2, 1.0)).z();
#ifdef QT_DEBUG
            qDebug() << "cross_product result is " << ret2;
#endif
            double angle = std::acos(ret);
            if(isnan(angle))
                angle = 0.0;
            angle = angle * 1.0f / (PI / 180);
            if(ret2 < 0)
                angle = -angle;

            m_angle += angle;
            m_trans.translate(boundingRect().center().x(),
                              boundingRect().center().y());
            m_trans.rotate(angle);
            m_trans.translate(-boundingRect().center().x(),
                              -boundingRect().center().y());
            setTransform(m_trans);

#ifdef QT_DEBUG
            qDebug() << "angle:  " << angle;
#endif
        }
        else
        {
            QGraphicsItem::mouseMoveEvent(event);
        }
    }
    else
    {
        QGraphicsItem::mouseMoveEvent(event);
    }
}

void PhotoItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(m_bRPressed)
    {
        m_bRPressed = false;
    }
    m_angle = 0;
    emit moveFinished();
    QGraphicsItem::mouseReleaseEvent(event);
}

QVariant PhotoItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange  && scene()) // 控件发生移动
    {
        QPointF newPos = value.toPointF(); //即将要移动的位置
        QRectF rect(0, 0, scene()->width(), scene()->height()); // 你要限制的区域
        if (!rect.contains(newPos)) // 是否在区域内
        {
            newPos.setX(qMin(rect.right(), qMax(newPos.x(), rect.left())));
            newPos.setY(qMin(rect.bottom(), qMax(newPos.y(), rect.top())));
            return newPos;
        }
    }
    return QGraphicsItem::itemChange(change, value);

}

QVector2D PhotoItem::GetCenterVector()
{
    QPointF pt(scenePos().x(), scenePos().y());
    QPointF ptCenter(pt.x() + m_nWidth / 2.0f,
                     pt.y() + m_nHeight / 2.0f);
    QVector2D vec2d(ptCenter);
    vec2d.normalize();
    return vec2d;
}

QVector2D PhotoItem::GenCurrentVector(QPointF pt)
{
    QPointF ptCurrent(pt.x(), pt.y());
    QVector2D vec2d(ptCurrent);
    vec2d.normalize();
    return vec2d;
}

void PhotoItem::ZoomAnimation(double nScale)
{
    ScaleAnimation* a1 = new ScaleAnimation(this);
    connect(a1, &ScaleAnimation::animationFinished, this, [&](){ m_bScaled = false; });
    a1->SetItem(this);
    int currentWidth = m_scaledImg.width();
    int currentHeight = m_scaledImg.height();
    double newWidth = currentWidth + nScale * ZOOM_WIDTH;
    double newHeight = newWidth / m_ratio;
    double xDiff = newWidth - currentWidth;
    double yDiff = newHeight - currentHeight;

    a1->SetStartValue(QRectF(scenePos().x(),
                             scenePos().y(),
                             currentWidth,
                             currentHeight));
    if(nScale > 0)
    {
        xDiff = -xDiff;
        yDiff = +yDiff;
    }
    else
    {
        xDiff = +xDiff;
        yDiff = -yDiff;
    }
    a1->SetEndValue(QRectF(scenePos().x() + xDiff / 2,
                           scenePos().y() + yDiff / 2,
                           newWidth,
                           newHeight));
    a1->Start(500);
    if(!m_bScaled)
        m_bScaled = true;
}

void PhotoItem::ScaleItem(int nScale)
{
    if(!m_bScaled)
        ZoomAnimation(nScale);
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


ScaleAnimation::ScaleAnimation(QObject *parent) : QObject(parent)
{
    m_pItem = nullptr;
    m_xSlice = 0.0;
    m_ySlice = 0.0;
    m_widthSlice = 0.0;
    m_sliceBack = 0.0;
    m_nCounter = 0;
    connect(&m_endTimer, &QTimer::timeout, this, [&](){
        if(m_timer.isActive())
            m_timer.stop();
        emit animationFinished();
        deleteLater();
    });
}

ScaleAnimation::~ScaleAnimation()
{

}

void ScaleAnimation::SetItem(PhotoItem *item)
{
    m_pItem = item;
    if(m_pItem)
    {
        int tpWidth = m_pItem->GetScaledImage().width();
        connect(&m_timer, &QTimer::timeout, this, [&, tpWidth](){
            if(m_nCounter % 2 == 0)
            {
                m_pItem->Scale(tpWidth, m_widthSlice);
                m_pItem->moveBy(m_xSlice, m_ySlice); // 旋转后因为坐标系变化，setpos情况很复杂，改用moveby最简单
                m_pItem->Update();
            }
            m_widthSlice += m_sliceBack;
            m_nCounter++;
        });
    }
}

void ScaleAnimation::SetStartValue(QRectF&& s)
{
    m_start = s;
}

void ScaleAnimation::SetEndValue(QRectF&& e)
{
    m_end = e;
}

void ScaleAnimation::Start(int milliseconds)
{
    m_diff = QRectF(m_end.x() - m_start.x(),
                    m_end.y() - m_end.y(),
                    m_end.width() - m_start.width(),
                    m_end.height() - m_start.height());
    double timeSlice = (milliseconds * 1.0f) / nMinInterval;
    m_xSlice = m_diff.x() / timeSlice;
    m_ySlice = m_diff.y() / timeSlice;
    m_widthSlice = (m_diff.width() / timeSlice) / timeSlice;
    m_sliceBack = m_widthSlice;
    m_endTimer.start(milliseconds);
    m_timer.start(nMinInterval);
}
