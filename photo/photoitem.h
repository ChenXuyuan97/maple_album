#ifndef PHOTOITEM_H
#define PHOTOITEM_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <mutex>
#include <QVector2D>
#include <QVector3D>
#include <QPixmap>

#ifndef P_MIN_WIDTH
#define P_MIN_WIDTH 300
#endif

#ifndef P_PADDING
#define P_PADDING 20
#endif

#ifndef ZOOM_WIDTH
#define ZOOM_WIDTH 50
#endif

#ifndef PI
#define PI acos(-1)
#endif

class ScaleAnimation;

class PhotoItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
    Q_PROPERTY(qreal scale READ scale WRITE setScale)
    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation)
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
public:
    explicit PhotoItem(QGraphicsItem *parent  = nullptr);
    ~PhotoItem() override;
    void Create(QString path);
    void Create(const QPixmap& pixmap);
    const QImage& GetSrcImage() const;
    const QImage& GetScaledImage() const;
    double GetRatio() const;
    void Scale(int srcWidth, double scale);
    void Update();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QRectF boundingRect() const override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    QVector2D GetCenterVector();
    QVector2D GenCurrentVector(QPointF point);
    void ZoomAnimation(double nScale);

private:
    int m_nWidth;
    int m_nHeight;
    double m_ratio;
    QImage m_srcImg;
    QImage m_scaledImg;
    bool m_bScaled;
    bool m_bRPressed; // 右键是否按下
    double m_angle;
    QTransform m_trans;

signals:
    /**
     * @brief moveFinished 一个item移动结束
     */
    void moveFinished();

    /**
     * @brief selected 当前被选中
     */
    void selected(PhotoItem*);

public slots:
    void ScaleItem(int nScale);
};

class ScaleAnimation : public QObject
{
    Q_OBJECT
public:
    explicit ScaleAnimation(QObject *parent = nullptr);
    ~ScaleAnimation();
    void SetItem(PhotoItem* item);
    void SetStartValue(QRectF&& s);
    void SetEndValue(QRectF&& e);
    void Start(int milliseconds);

protected:
    const int nMinInterval = 10;

private:
    QTimer m_timer;
    QTimer m_endTimer;
    PhotoItem* m_pItem;
    QRectF m_start;
    QRectF m_end;
    QRectF m_diff;
    double m_xSlice;
    double m_ySlice;
    double m_widthSlice;
    double m_sliceBack;
    int m_nCounter;

signals:
    void animationFinished();
};

#endif // PHOTOITEM_H
