#ifndef SCREENCAPTUREENGINE_H
#define SCREENCAPTUREENGINE_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QPushButton>

class CaptureView : public QWidget
{
    Q_OBJECT
public:
    CaptureView(QWidget* parent = nullptr);
    ~CaptureView();
    static const int nCaptureBorder;
    static const QColor borderColor;
protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    /**
     * @brief PlaceLabelSize 将m_pLabelSize放置到合适的位置
     * @param ptDiff 当前鼠标坐标减去按下时刻的坐标
     */
    void PlaceLabelSize(const QPointF& ptDiff);

    /**
     * @brief PlaceCompleteButton
     */
    void PlaceCompleteButton();

    /**
     * @brief CaptureRegionTransform 截图区域rect的转换
     * @param ptDiff
     */
    void CaptureRegionTransform(const QPointF& ptDiff);

private:
    QLabel* m_pLabelSize; // 显示当前截图区域的大小
    QPushButton* m_pBtnComplete; // 完成截图
    bool m_bLPressed; // 左键是否按下
    bool m_bLFinished; // 左键按下并松开，结束区域的决定
    QRect m_screenRect; // 屏幕区域
    QRect m_captureRect; // 截图的区域
    QPoint m_ptPressed; // 按下时候的坐标
    QPoint m_ptCurrent; // 当前移动的坐标
    QPixmap m_screenImage;
    QPixmap m_captureImage;
signals:
    /**
     * @brief CaptureFinished 窗口退出时候发出，代表截屏结束
     * @param b true: 截屏结束, false: 截屏中途退出
     * @param pixmap 若b参数是true，则返回截图的图像信息。否则，返回空图像
     */
    void CaptureFinished(bool b, QPixmap pixmap);
};

class ScreenCaptureEngine : public QObject
{
    Q_OBJECT
public:
    explicit ScreenCaptureEngine(QObject *parent = nullptr);
    ~ScreenCaptureEngine();
    Q_DISABLE_COPY_MOVE(ScreenCaptureEngine)

    /**
     * @brief Start 开始截图
     * @param bHide true: 隐藏当前窗口截图，false：不隐藏当前窗口
     */
    void Start(bool bHide = true);

signals:
    /**
     * @brief CaptureFinished 窗口退出时候发出，代表截屏结束
     * @param b true: 截屏结束, false: 截屏中途退出
     * @param pixmap 若b参数是true，则返回截图的图像信息。否则，返回空图像
     */
    void CaptureFinished(bool b, QPixmap pixmap);
};

#endif // SCREENCAPTUREENGINE_H
