#include "ScreenCaptureEngine.h"
#include <QStyleOption>
#include <QPainter>
#include <QScreen>
#include <QApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDesktopWidget>
#ifdef QT_DEBUG
#include <QDebug>
#endif

const int CaptureView::nCaptureBorder = 1;
const QColor CaptureView::borderColor = QColor(93, 190, 138);

CaptureView::CaptureView(QWidget *parent) : QWidget(parent), m_bLPressed(false), m_bLFinished(false)
{
    setObjectName(QStringLiteral("capture_bg_view"));
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowState(Qt::WindowActive | Qt::WindowFullScreen);
    QScreen* screen = QApplication::primaryScreen();
    m_screenRect = screen->geometry();
    setFixedSize(m_screenRect.width(), m_screenRect.height());
    setStyleSheet(QStringLiteral("QWidget#capture_bg_view{border:none;}"));

    // 初始化其它控件
    m_screenImage = screen->grabWindow(QApplication::desktop()->winId(),
                                       0,
                                       0,
                                       screen->geometry().width(),
                                       screen->geometry().height());
    m_pLabelSize = new QLabel(this);
    m_pBtnComplete = new QPushButton(QStringLiteral("完成"), this);
    m_pBtnComplete->setCursor(QCursor(Qt::PointingHandCursor));
    m_pLabelSize->setStyleSheet(QStringLiteral("QLabel{color:white;font-family:Microsoft Yahei;font-size:15px;background-color:black;}"));
    m_pBtnComplete->setStyleSheet(QStringLiteral("QPushButton{color:white;font-family:Microsoft Yahei;border:none;font-size:18px;background-color:black;}"));
    m_pBtnComplete->setFixedSize(60, 22);
    m_pLabelSize->hide();
    m_pBtnComplete->hide();
    setMouseTracking(true);
    connect(m_pBtnComplete, &QPushButton::clicked, this, [&]{
        emit CaptureFinished(true, m_captureImage);
        deleteLater();
    });
}

CaptureView::~CaptureView()
{

}

void CaptureView::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    p.drawPixmap(0, 0, m_screenImage);
    p.fillRect(rect(), QColor(128, 128, 128, 128));
    //
    QPointF diff = m_ptCurrent - m_ptPressed;
    QPen pen(borderColor);
    pen.setWidth(nCaptureBorder);
    p.setPen(pen);
    if(m_bLPressed)
    {
        if(!m_bLFinished)
        {
            if(diff.x() != 0 || diff.y() != 0)
            {
                m_captureRect = QRect(m_ptPressed, m_ptCurrent);

                m_pLabelSize->setText(QString::number(std::abs(diff.x())) +
                                      QStringLiteral(" x ") +
                                      QString::number(std::abs(diff.y())));
                m_pLabelSize->adjustSize();
                PlaceLabelSize(diff);
                if(m_pLabelSize->isHidden())
                    m_pLabelSize->show();

                CaptureRegionTransform(diff);
                p.drawRect(m_captureRect);
                m_captureImage = m_screenImage.copy(QRect(m_captureRect.x() + nCaptureBorder,
                                                          m_captureRect.y() + nCaptureBorder,
                                                          m_captureRect.width() - nCaptureBorder,
                                                          m_captureRect.height() - nCaptureBorder));
                p.drawPixmap(QRect(m_captureRect.x() + nCaptureBorder,
                                   m_captureRect.y() + nCaptureBorder,
                                   m_captureRect.width() - nCaptureBorder,
                                   m_captureRect.height() - nCaptureBorder), m_captureImage);
            }
        }
        else
        {
            PlaceLabelSize(diff);
            p.drawRect(m_captureRect);
            m_captureImage = m_screenImage.copy(QRect(m_captureRect.x() + nCaptureBorder,
                                                      m_captureRect.y() + nCaptureBorder,
                                                      m_captureRect.width() - nCaptureBorder,
                                                      m_captureRect.height() - nCaptureBorder));
            p.drawPixmap(QRect(m_captureRect.x() + nCaptureBorder,
                               m_captureRect.y() + nCaptureBorder,
                               m_captureRect.width() - nCaptureBorder,
                               m_captureRect.height() - nCaptureBorder), m_captureImage);
        }
    }
    else
    {
        if(m_bLFinished)
        {
            if(m_pLabelSize->isHidden())
                m_pLabelSize->show();

            p.drawRect(m_captureRect);
            p.drawPixmap(QRect(m_captureRect.x() + nCaptureBorder,
                               m_captureRect.y() + nCaptureBorder,
                               m_captureRect.width() - nCaptureBorder,
                               m_captureRect.height() - nCaptureBorder), m_captureImage);
        }
    }
    //
    QWidget::paintEvent(event);
}

void CaptureView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        if(!m_bLPressed)
        {
            m_bLPressed = true;
        }
        if(m_bLFinished)
        {
            if(cursor() == Qt::SizeAllCursor)
                m_ptPressed = event->pos() - m_captureRect.topLeft();
        }
        else
            m_ptPressed = event->pos();
    }
    else
    {
        QWidget::mousePressEvent(event);
    }
}

void CaptureView::mouseMoveEvent(QMouseEvent *event)
{
    if(m_bLPressed)
    {
        m_ptCurrent = event->pos();
        if(m_bLFinished)
        {
            if(cursor() == Qt::SizeAllCursor)
            {
                m_captureRect.moveTo(event->globalPos() - m_ptPressed);
                PlaceCompleteButton();
            }
        }
        update();
    }
    else
    {
        if(m_bLFinished)
        {
            if(m_captureRect.contains(event->pos()))
            {
                if(cursor() != Qt::SizeAllCursor)
                    setCursor(QCursor(Qt::SizeAllCursor));
            }
            else
            {
                if(cursor() != Qt::ArrowCursor)
                    setCursor(QCursor(Qt::ArrowCursor));
            }
        }
    }
    QWidget::mouseMoveEvent(event);
}

void CaptureView::mouseReleaseEvent(QMouseEvent *event)
{
    if(m_bLPressed)
    {
        if(!m_bLFinished)
        {
            m_bLFinished = true;
            if(m_pBtnComplete->isHidden())
                m_pBtnComplete->show();
            PlaceCompleteButton();
        }
        m_bLPressed = false;
        update();
    }
    QWidget::mouseReleaseEvent(event);
}

void CaptureView::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
    {
        emit CaptureFinished(false, QPixmap());
        deleteLater();
    }
    else
    {
        QWidget::keyPressEvent(event);
    }
}

void CaptureView::PlaceLabelSize(const QPointF& ptDiff)
{
    int height = m_pLabelSize->height();
    if(m_bLFinished)
    {
        if(m_captureRect.top() >= height)
        {
            m_pLabelSize->move(m_captureRect.left(),
                               m_captureRect.top() - height);
        }
        else
        {
            m_pLabelSize->move(m_captureRect.left() + nCaptureBorder,
                               m_captureRect.top() + nCaptureBorder);
        }
    }
    else
    {
        if(ptDiff.x() > 0 && ptDiff.y() > 0)
        {
            // TODO: x大于0，y大于0，则往右下角移动
            // 此时label要显示在m_ptPressed
            if(m_ptPressed.y() < height)
            {
                m_pLabelSize->move(m_ptPressed.x(), m_ptPressed.y());
            }
            else
            {
                m_pLabelSize->move(m_ptPressed.x(), m_ptPressed.y() - height);
            }
        }
        else if(ptDiff.x() > 0 && ptDiff.y() < 0)
        {
            // TODO: x大于0，y小于0，则往右上角移动
            // 此时label要显示在m_ptPressed + (0, ptDiff.y())
            if(m_ptPressed.y() + ptDiff.y() < height)
            {
                m_pLabelSize->move(m_ptPressed.x() + nCaptureBorder, m_ptPressed.y() + ptDiff.y() + nCaptureBorder);
            }
            else
            {
                m_pLabelSize->move(m_ptPressed.x(), m_ptPressed.y() + ptDiff.y() - height);
            }
        }
        else if(ptDiff.x() < 0 && ptDiff.y() > 0)
        {
            // TODO: x小于0，y大于0，则往左下角移动
            // 此时label要显示在m_ptPressed + (ptDiff.x(), 0)
            if(m_ptPressed.y() < height)
            {
                m_pLabelSize->move(m_ptPressed.x() + ptDiff.x(), m_ptPressed.y());
            }
            else
            {
                m_pLabelSize->move(m_ptPressed.x() + ptDiff.x(), m_ptPressed.y() - height);
            }
        }
        else if(ptDiff.x() < 0 && ptDiff.y() < 0)
        {
            // TODO: x小于0，y小于0，则往左上角移动
            // 此时label要显示在m_ptPressed + (ptDiff.x(), ptDiff.y())
            if(m_ptPressed.y() + ptDiff.y() < height)
            {
                m_pLabelSize->move(m_ptPressed.x() + ptDiff.x() + nCaptureBorder, m_ptPressed.y() + ptDiff.y() + nCaptureBorder);
            }
            else
            {
                m_pLabelSize->move(m_ptPressed.x() + ptDiff.x(), m_ptPressed.y() + ptDiff.y() - height);
            }
        }
        else
        {
            // TODO: 暂时不做处理
        }
    }
}

void CaptureView::PlaceCompleteButton()
{
    if(height() - m_captureRect.top() - m_captureRect.height() < m_pBtnComplete->height())
    {
        m_pBtnComplete->move(m_captureRect.x() + m_captureRect.width() - m_pBtnComplete->width(),
                             m_captureRect.y() - m_pBtnComplete->height());

    }
    else
    {
        m_pBtnComplete->move(m_captureRect.x() + m_captureRect.width() - m_pBtnComplete->width(),
                             m_captureRect.y() + m_captureRect.height() + nCaptureBorder);
    }
}

void CaptureView::CaptureRegionTransform(const QPointF &ptDiff)
{
    if(ptDiff.x() > 0 && ptDiff.y() > 0)
    {
        // TODO: x大于0，y大于0，则往右下角移动
        m_captureRect = QRect(m_ptPressed, m_ptCurrent);
    }
    else if(ptDiff.x() > 0 && ptDiff.y() < 0)
    {
        // TODO: x大于0，y小于0，则往右上角移动
        m_captureRect = QRect(m_ptPressed.x(),
                              m_ptPressed.y() + ptDiff.y(),
                              ptDiff.x(),
                              std::abs(ptDiff.y()));
    }
    else if(ptDiff.x() < 0 && ptDiff.y() > 0)
    {
        // TODO: x小于0，y大于0，则往左下角移动
        m_captureRect = QRect(m_ptPressed.x() + ptDiff.x(),
                              m_ptPressed.y(),
                              std::abs(ptDiff.x()),
                              ptDiff.y());

    }
    else if(ptDiff.x() < 0 && ptDiff.y() < 0)
    {
        // TODO: x小于0，y小于0，则往左上角移动
        m_captureRect = QRect(m_ptPressed.x() + ptDiff.x(),
                              m_ptPressed.y() + ptDiff.y(),
                              std::abs(ptDiff.x()),
                              std::abs(ptDiff.y()));
    }
    else
    {
        // TODO: 暂时不做处理
    }
}


//////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

ScreenCaptureEngine::ScreenCaptureEngine(QObject *parent) : QObject(parent)
{

}

ScreenCaptureEngine::~ScreenCaptureEngine()
{

}

void ScreenCaptureEngine::Start(bool bHide)
{
    Q_UNUSED(bHide)
    CaptureView* view = new CaptureView(nullptr);
    connect(view, &CaptureView::CaptureFinished, this, [&](bool b, QPixmap pixmap){
        emit CaptureFinished(b, pixmap);
        deleteLater();
    });
    view->move(0, 0);
    view->show();
    view->raise();
}
