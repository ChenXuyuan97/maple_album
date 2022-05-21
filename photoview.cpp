#include "photoview.h"
#include <QMouseEvent>
#include "photoscene.h"
#include <QFile>
#include <photo/photoitem.h>
#include <QKeyEvent>

PhotoView::PhotoView(QWidget *parent) : QGraphicsView(parent)
{
    setAcceptDrops(true);
    QFile file(":/resource/style/photo_view.qss");
    if(file.open(QIODevice::ReadOnly))
    {
        QString qss = file.readAll();
        file.close();
        setStyleSheet(qss);
    }
}

PhotoView::~PhotoView()
{

}

//void PhotoView::drawBackground(QPainter *painter, const QRectF &rect)
//{
//    painter->fillRect(rect, QBrush(Qt::black));
//}

//void PhotoView::paintEvent(QPaintEvent *event)
//{
//    QPainter p(this);
//    p.fillRect(rect(), QBrush(Qt::black));
////    QGraphicsView::paintEvent(event);
//}

void PhotoView::wheelEvent(QWheelEvent *event)
{
    PhotoScene* s = dynamic_cast<PhotoScene*>(const_cast<QGraphicsScene*>(scene()));
    if(s)
    {
        PhotoItem* item = dynamic_cast<PhotoItem*>(s->GetSelected());
        if(item)
        {
            int n = event->angleDelta().y();
            item->ScaleItem(n > 0 ? 1 : -1);
            return;
        }
    }
    QGraphicsView::wheelEvent(event);
}

void PhotoView::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete)
    {
        auto list = scene()->selectedItems();
        if(list.size() > 0)
        {
            QGraphicsItem* pItem = list[0];
            scene()->removeItem(pItem);
            delete pItem;
            pItem = nullptr;
        }
    }
    QGraphicsView::keyPressEvent(event);
}
