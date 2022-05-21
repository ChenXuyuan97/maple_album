#include "FloatingActionButton.h"
#include <QPainter>
#include <QStyleOption>
#include <QMouseEvent>
#include <QPainterPath>
#include <cmath>
#ifdef QT_DEBUG
#include <QDebug>
#include <QDateTime>
#endif
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>

#ifndef PI
#define PI acos(-1)
#endif

FloatingActionButton::FloatingActionButton(QWidget *parent) : QWidget(parent)
{
    m_bFold = true;
    setFixedSize(72, 72);
    setObjectName(QStringLiteral("floating_action_button"));
    // 设置主按钮样式
    setStyleSheet(QStringLiteral("QWidget#floating_action_button{background:url(:/controls/res/floating_action_button/unfold.png) no-repeat;background-position:center;background-color:white;border-radius:36px;}"
                                 "QWidget:hover#floating_action_button{background-color:rgb(242,246,252);}"));
    m_vecButtons.reserve(5);
    setCursor(QCursor(Qt::PointingHandCursor));
    QGraphicsDropShadowEffect *effect2 = new QGraphicsDropShadowEffect(this);
    effect2->setOffset(0, 0);
    effect2->setColor(Qt::gray);
    effect2->setBlurRadius(20);
    setGraphicsEffect(effect2);
}

FloatingActionButton::~FloatingActionButton()
{

}

void FloatingActionButton::Push(QWidget *btn, const QString& imagePath)
{
    if(btn == nullptr)
    {
        throw std::runtime_error("FloatingActionButton: The added button is null!");
        return;
    }
    btn->setFixedSize(60, 60);
    btn->setCursor(QCursor(Qt::PointingHandCursor));
    QGraphicsDropShadowEffect *effect2 = new QGraphicsDropShadowEffect(btn);
    effect2->setOffset(0, 0);
    effect2->setColor(Qt::gray);
    effect2->setBlurRadius(10);
    btn->setGraphicsEffect(effect2);
    if(imagePath.isEmpty())
    {
        btn->setStyleSheet(QStringLiteral("QPushButton{font-family:Microsoft Yahei;background-color:white;border-radius:30px;}"
                                          "QPushButton:hover{background-color:rgb(242,246,252);}"));
    }
    else
    {
        QString imageQss = QStringLiteral("image:url(") + imagePath + QStringLiteral(");");
        btn->setStyleSheet("QPushButton{font-family:Microsoft Yahei;background-color:white;border-radius:30px;"+ imageQss + "}"
                                                                                                                            "QPushButton:hover{background-color:rgb(242,246,252);}");
    }

    // 移动到主按钮的中心点
    btn->move(geometry().x() + (width() - btn->width() ) / 2,
              geometry().y() + (height() - btn->height()) / 2);
    btn->setVisible(true); // Push后要显示出来，若等到动画时候再显示，第一次show会非常慢
    btn->lower();
    m_vecButtons.emplace_back(btn);
}

void FloatingActionButton::Unfold()
{
    float radius = 128.0f; // 圆的半径
    QPointF circle(geometry().x() + width() / 2, geometry().y() + height() / 2); // 圆心
    size_t sz = m_vecButtons.size();
    float angle = 180.0f; // 散射出去的角度范围
    if(sz > 1)
    {
        angle /= (sz - 1);
    }
    if(sz > 0)
    {
        for(size_t i = 0; i < sz; i++)
        {
            QWidget* tmpBtn = m_vecButtons[i];
            if(m_bFold)
            {
                float left = (radius + tmpBtn->width() / 2) * std::sin( (i * angle) * PI / 180 );
                float top = (radius + tmpBtn->width() / 2) * std::cos( (i * angle) * PI / 180 );
                int width = tmpBtn->width();
                int height = tmpBtn->height();
                QRect rf(circle.x() + left - tmpBtn->width() / 2, circle.y() + top - tmpBtn->height() / 2, width, height);
                tmpBtn->raise();
                UnfoldAnimation(tmpBtn, rf);
            }
            else
            {
                FoldAnimation(tmpBtn);
            }
        }
        m_bFold = !m_bFold;
    }
}

void FloatingActionButton::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        Unfold();
    }
}

void FloatingActionButton::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(event);
}

void FloatingActionButton::moveEvent(QMoveEvent *event)
{
    Q_UNUSED(event)
    for(std::size_t i = 0; i < m_vecButtons.size(); i++)
    {
        // 移动到主按钮的中心点
        QWidget* btn = m_vecButtons[i];
        btn->move(geometry().x() + (width() - btn->width() ) / 2,
                  geometry().y() + (height() - btn->height()) / 2);
        btn->lower();
    }
}

void FloatingActionButton::UnfoldAnimation(QWidget *btn, QRectF dest)
{
    QPropertyAnimation *animation = new QPropertyAnimation(btn, "geometry");

    animation->setDuration(m_nDuration);

    animation->setStartValue(btn->geometry());

    animation->setEndValue(dest);

    connect(animation, &QPropertyAnimation::finished, this, [&](){
        setDisabled(false);
    });

    setDisabled(true);
    animation->start(QAbstractAnimation::DeleteWhenStopped);

}

void FloatingActionButton::FoldAnimation(QWidget *btn)
{
    QPropertyAnimation *animation = new QPropertyAnimation(btn, "geometry");

    animation->setDuration(m_nDuration);

    animation->setStartValue(btn->geometry());

    animation->setEndValue(QRectF(geometry().x(), geometry().y(), btn->width(), btn->height()));

    connect(animation, &QPropertyAnimation::finished, this, [&, btn](){
        setDisabled(false);
        btn->lower();
    });

    setDisabled(true);
    animation->start(QAbstractAnimation::DeleteWhenStopped);

}
