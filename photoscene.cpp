#include "photoscene.h"
#include <QGraphicsItem>

PhotoScene::PhotoScene(QWidget *parent) : QGraphicsScene(parent)
{

}

PhotoScene::~PhotoScene()
{

}

void PhotoScene::AdjustSceneSize()
{
    QList<QGraphicsItem*> list = items();
    int posX = 0;
    int posY = 0;
    for(int i = 0; i < list.size(); i++)
    {
        QGraphicsItem* item = list[i];
        int itemPosX = item->sceneBoundingRect().x() + item->sceneBoundingRect().width() ;
        int itemPosY = item->sceneBoundingRect().y() + item->sceneBoundingRect().height();
        if(itemPosX > posX)
            posX = itemPosX;
        if(itemPosY > posY)
            posY = itemPosY;
    }
    if(posX < m_minSize.width())
        posX = m_minSize.width();
    if(posY < m_minSize.height())
        posY = m_minSize.height();
    setSceneRect(0, 0, posX, posY);
}

void PhotoScene::SetMinimumSize(const QSize &size)
{
    m_minSize = size;
}

QGraphicsItem *PhotoScene::GetSelected()
{
    QList<QGraphicsItem*> list = selectedItems();
    if(list.size() <= 0)
        return nullptr;
    else
        return list.front();
}

void PhotoScene::Clear()
{
    auto list = items();
    for(int i = 0; i < list.size(); i++)
    {
        QGraphicsItem* pItem = list[i];
        removeItem(pItem);
        delete pItem;
        pItem = nullptr;
    }
    AdjustSceneSize();
}

void PhotoScene::Multiplechoice(QGraphicsItem *pItem)
{
    auto list = selectedItems();
    for(int i = 0; i < list.size(); i++)
    {
        if(list[i] != pItem)
            list[i]->setSelected(false);
    }
}

void PhotoScene::SelectAll(bool b)
{
    auto list = items();
    for(int i = 0; i < list.size(); i++)
    {
        list[i]->setSelected(b);
    }
}
