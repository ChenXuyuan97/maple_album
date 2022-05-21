#ifndef PHOTOSCENE_H
#define PHOTOSCENE_H

#include <QWidget>
#include <QGraphicsScene>

class PhotoScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit PhotoScene(QWidget *parent = nullptr);
    ~PhotoScene();

    /**
     * @brief AdjustSceneSize 调整当前的scene大小
     */
    void AdjustSceneSize();

    /**
     * @brief SetMinimumSize 设置scene的最小尺寸
     * @param size
     */
    void SetMinimumSize(const QSize& size);

    /**
     * @brief GetSelected 获取选中的第一个控件
     * @return
     */
    QGraphicsItem* GetSelected();

    /**
     * @brief Clear 清除所有的item
     */
    void Clear();

    /**
     * @brief Multiplechoice 触发单选
     * @param pItem
     */
    void Multiplechoice(QGraphicsItem* pItem);

    /**
     * @brief SelectAll 全部选中/取消选中
     * @param b true:全选，false:全取消
     */
    void SelectAll(bool b);

private:
    QSize m_minSize;
};

#endif // PHOTOSCENE_H
