#ifndef FLOATINGACTIONBUTTON_H
#define FLOATINGACTIONBUTTON_H

#include <QWidget>
#include <QPushButton>


class FloatingActionButton : public QWidget
{
    Q_OBJECT
public:
    explicit FloatingActionButton(QWidget *parent = nullptr);
    ~FloatingActionButton() override;

    /**
     * @brief Push 添加要展开的按钮(注：子按钮大小必须小于主按钮，否则无法盖住隐藏)
     * @param btn
     * @param imagePath 图像的图片路径
     */
    void Push(QWidget* btn, const QString& imagePath = "");

    /**
     * @brief Unfold 全部折叠
     */
    void Unfold();

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent* event) override;

    /**
     * @brief moveEvent 主按钮移动后，子按钮也要跟着移动
     * @param event
     */
    void moveEvent(QMoveEvent* event) override;
    void UnfoldAnimation(QWidget* btn, QRectF dest);
    void FoldAnimation(QWidget* btn);

private:
    std::vector<QWidget*> m_vecButtons;
    bool m_bFold; // 是否折叠
    const int m_nDuration = 300; // 动画时间
};

#endif // FLOATINGACTIONBUTTON_H
