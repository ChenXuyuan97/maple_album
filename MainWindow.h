#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "photo/photoitem.h"
#include "photoview.h"
#include "photoscene.h"
#ifdef Q_OS_WINDOWS
#include <windows.h>
#include <QtWinExtras/QtWin>
#include <QDomComment>
#endif
#include <QAbstractNativeEventFilter>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow, public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    /**
     * @brief CreateItem 生成一个Item
     * @param path 图片完整路径
     */
    void CreateItem(const QString& path);

    /**
     * @brief CreateItem 生成一个Item
     * @param pixmap 已有图片
     */
    void CreateItem(const QPixmap& pixmap);

protected:
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;
    void Init();
    void Capture();

private:
    Ui::MainWindow *ui;
    PhotoView* m_pView;
    PhotoScene* m_pScene;
    QLabel* m_pLabelPrompt;
};
#endif // MAINWINDOW_H
