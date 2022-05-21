#ifndef PHOTOVIEW_H
#define PHOTOVIEW_H

#include <QObject>
#include <QGraphicsView>

class PhotoView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit PhotoView(QWidget *parent = nullptr);
    ~PhotoView() override;

protected:
    //void drawBackground(QPainter *painter, const QRectF &rect) override;
    //void paintEvent(QPaintEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // PHOTOVIEW_H
