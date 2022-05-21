#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QFile>
#include "controls/FloatingActionButton.h"
#include <QFileDialog>
#include <QMessageBox>
#include <screen_capture/ScreenCaptureEngine.h>
#ifdef QT_DEBUG
#include <QDebug>
#endif

#ifndef CAPTURE_HOT_KEY_ID
#define CAPTURE_HOT_KEY_ID 0x100
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(1366, 768);
    Init();
}

MainWindow::~MainWindow()
{
    UnregisterHotKey(HWND(winId()), CAPTURE_HOT_KEY_ID);
    delete ui;
}

void MainWindow::CreateItem(const QString &path)
{
    PhotoItem* item = new PhotoItem;
    item->Create(path);
    connect(item, &PhotoItem::moveFinished, m_pScene, &PhotoScene::AdjustSceneSize);
    connect(item, &PhotoItem::selected, m_pScene, &PhotoScene::Multiplechoice);
    m_pScene->addItem(item);
    m_pScene->AdjustSceneSize();
}

void MainWindow::CreateItem(const QPixmap &pixmap)
{
    PhotoItem* item = new PhotoItem;
    item->Create(pixmap);
    connect(item, &PhotoItem::moveFinished, m_pScene, &PhotoScene::AdjustSceneSize);
    connect(item, &PhotoItem::selected, m_pScene, &PhotoScene::Multiplechoice);
    m_pScene->addItem(item);
    m_pScene->AdjustSceneSize();
}

bool MainWindow::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(eventType)
    Q_UNUSED(result)
    MSG* msg = static_cast<MSG*>(message);
    if (msg->message == WM_HOTKEY)
    {
        switch (msg->wParam) {
        case CAPTURE_HOT_KEY_ID:
            Capture();
            break;
        default:
            break;
        }
        return true;
    }
    return false;
}

void MainWindow::Init()
{
    m_pLabelPrompt = new QLabel(this);
    m_pLabelPrompt->setFixedSize(width(), 20);
    m_pLabelPrompt->setObjectName(QStringLiteral("label_prompt"));
    m_pLabelPrompt->setAlignment(Qt::AlignLeft);
    m_pLabelPrompt->setText(QStringLiteral("点击图片选中，按右键可以自由旋转，按退格/删除可以删除选中图片!"));
    m_pLabelPrompt->move(0, height() - m_pLabelPrompt->height());
    m_pLabelPrompt->setStyleSheet(QStringLiteral("QLabel#label_prompt{font-size: 16px;font-family:Microsoft Yahei;background-color:white;color: black;}"));

    setWindowTitle(QStringLiteral("MapleAlbum"));
    ui->centralwidget->setObjectName(QStringLiteral("main_widget"));
    QFile file(":/resource/style/main_window.qss");
    if(file.open(QIODevice::ReadOnly))
    {
        QString qss = file.readAll();
        file.close();
        ui->centralwidget->setStyleSheet(qss);
    }

#ifdef Q_OS_WINDOWS
    // 快捷键
    if (RegisterHotKey(HWND(winId()), CAPTURE_HOT_KEY_ID, MOD_ALT, 'Q'))
    {
#ifdef QT_DEBUG
        qDebug() << "ALT + Q 注册成功";
#endif
    }
    else
    {
        QMessageBox::warning(this,
                             QStringLiteral("警告"),
                             QStringLiteral("ALT + Q快捷键已被注册"),
                             QMessageBox::Yes,
                             QMessageBox::Yes);
    }
#endif
    // 加载图像场景
    m_pView = new PhotoView(this);
    m_pScene = new PhotoScene(this);

    m_pView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    m_pView->setCacheMode(QGraphicsView::CacheBackground);
    m_pView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    m_pView->setOptimizationFlag(QGraphicsView::DontSavePainterState);

    m_pView->setScene(m_pScene);
    m_pView->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_pView->setFixedSize(1366, 768 - m_pLabelPrompt->height());
    m_pScene->setSceneRect(QRect(0, 0, m_pView->width(), m_pView->height()));
    m_pScene->SetMinimumSize(size());
    m_pView->move(0, 0);
    m_pView->show();

    // 加载操作项
    FloatingActionButton* fab = new FloatingActionButton(this);

    // 导出按钮
    QPushButton* pBtnExport = new QPushButton(this);
    pBtnExport->setToolTip(QStringLiteral("导出"));
    fab->Push(pBtnExport, ":/resource/skin/export.png");
    connect(pBtnExport, &QPushButton::clicked, this, [&]{
        m_pScene->SelectAll(false);
        m_pScene->AdjustSceneSize();
        QPainter painter;
        QImage image(m_pScene->sceneRect().width(),
                     m_pScene->sceneRect().height(),
                     QImage::Format_ARGB32);
        image.fill(qRgb(0x00, 0x00, 0x00));
        painter.begin(&image);
        m_pScene->render(&painter, m_pScene->sceneRect(), m_pScene->sceneRect());
        painter.end();
        QString path = QFileDialog::getSaveFileName(this,
                                                    QStringLiteral("保存"),
                                                    QDir::homePath(),
                                                    QStringLiteral("Image Files (*.png)"));
        if(!path.isEmpty())
        {
            image.save(path);
        }
    });

    // 屏幕截图
    QPushButton* pBtnScreenShot = new QPushButton(this);
    pBtnScreenShot->setToolTip(QStringLiteral("导入屏幕截图"));
    fab->Push(pBtnScreenShot, ":/resource/skin/screen_capture.png");
    connect(pBtnScreenShot, &QPushButton::clicked, this, [&]{
#ifdef Q_OS_WINDOWS
        if (OpenClipboard(NULL) && IsClipboardFormatAvailable(CF_DIB))
        {
            HBITMAP hBitmap = (HBITMAP)GetClipboardData(CF_BITMAP);
            if (hBitmap != NULL && hBitmap != INVALID_HANDLE_VALUE)
            {
                QPixmap pixmap = QtWin::fromHBITMAP(hBitmap);
                CreateItem(pixmap);
            }
        }
        CloseClipboard();
#else
        QMessageBox::warning(this,
                             QStringLiteral("警告"),
                             QStringLiteral("该功能只能Windows系统使用!"),
                             QMessageBox::Yes,
                             QMessageBox::Yes)
        #endif
    });


    // QQ聊天框截图导入按钮
    QPushButton* pBtnQQ = new QPushButton(this);
    pBtnQQ->setToolTip(QStringLiteral("导入QQ聊天截图"));
    fab->Push(pBtnQQ, ":/resource/skin/qq.png");
    connect(pBtnQQ, &QPushButton::clicked, this, [&]{
#ifdef Q_OS_WINDOWS
        UINT format = ::RegisterClipboardFormat(L"QQ_Unicode_RichEdit_Format");
        if (OpenClipboard(NULL))
        {
            HANDLE data = GetClipboardData(format);
            if (data != NULL)
            {
                int len = GlobalSize(data);
                auto mdata = GlobalLock(data);
                if (mdata != nullptr && len > 0)
                {
                    QString str((char*)data);

                    QDomDocument doc;
                    bool ret = doc.setContent(str);
                    if(!ret)
                        return;
                    QDomElement docElem = doc.documentElement();
                    QDomNode firstNode = docElem.firstChild();
                    while(!firstNode.isNull())
                    {
                        QDomElement element = firstNode.toElement();
                        if(!element.isNull() && element.tagName().compare(QStringLiteral("EditElement")) == 0)
                        {
                            QDomNamedNodeMap attributes = firstNode.attributes();
                            if(!attributes.isEmpty())
                            {
                                bool bImage = false;
                                bool bFilePath = false;
                                QString strPath;
                                for(int i = 0; i < attributes.count(); i++)
                                {
                                    QDomAttr attr = attributes.item(i).toAttr();
#ifdef QT_DEBUG
                                    qDebug() << "attr_name is " << attr.name();
#endif
                                    if(attr.name().compare(QStringLiteral("filepath")) == 0)
                                    {
                                        bFilePath = true;
                                        strPath = QString::fromWCharArray(attr.value().toStdWString().c_str());
                                        continue;
                                    }
                                    if(attr.name().compare(QStringLiteral("type")) == 0)
                                    {
                                        if(attr.value().toInt() == 1 || attr.value().toInt() == 3)
                                        {
                                            bImage = true;
                                        }
                                    }
                                }
                                if(bImage && bFilePath && QFile(strPath).exists())
                                {
                                    CreateItem(strPath);
                                }
                            }
                        }
                        firstNode = firstNode.nextSibling();
                    }
                }
            }
            GlobalUnlock(data);
            CloseClipboard();
        }
#else
        QMessageBox::warning(this,
                             QStringLiteral("警告"),
                             QStringLiteral("该功能只能Windows系统使用!"),
                             QMessageBox::Yes,
                             QMessageBox::Yes)
        #endif
    });

    // 清除
    QPushButton* pBtnClear = new QPushButton(this);
    pBtnClear->setToolTip(QStringLiteral("清除"));
    fab->Push(pBtnClear, ":/resource/skin/clear.png");
    connect(pBtnClear, &QPushButton::clicked, this, [&]{
        if(QMessageBox::warning(this,
                                QStringLiteral("警告"),
                                QStringLiteral("确定清除所有照片吗？"),
                                QMessageBox::Yes | QMessageBox::No,
                                QMessageBox::No) == QMessageBox::Yes)
        {
            m_pScene->Clear();
        }
    });

    // 自己的截图按钮
    QPushButton* pBtnCapture = new QPushButton(this);
    pBtnCapture->setToolTip(QStringLiteral("截图(ALT + Q)"));
    fab->Push(pBtnCapture, ":/resource/skin/capture.png");
    connect(pBtnCapture, &QPushButton::clicked, this, &MainWindow::Capture);

    // 导入图片按钮
    QPushButton* pBtnImport = new QPushButton(this);
    pBtnImport->setToolTip(QStringLiteral("导入本地图片"));
    connect(pBtnImport, &QPushButton::clicked, this, [&]{
        QString fileName =  QFileDialog::getOpenFileName(this,
                                                         QStringLiteral("图片"),
                                                         QDir::homePath(),
                                                         QStringLiteral("Image Files (*.png *.jpg *.bmp)"));
        if(!fileName.isEmpty())
        {
            CreateItem(fileName);
        }
    });
    fab->Push(pBtnImport, ":/resource/skin/picture_import.png");

    fab->move(30, height() / 2);
    fab->raise();
    m_pLabelPrompt->raise();

    ui->centralwidget->setAcceptDrops(true);
    setAcceptDrops(true);
}

void MainWindow::Capture()
{
    hide();
    ScreenCaptureEngine* sce = new ScreenCaptureEngine;
    connect(sce, &ScreenCaptureEngine::CaptureFinished, this, [&](bool b, QPixmap pixmap){
        if(isHidden())
            show();
        if(b)
        {
            CreateItem(pixmap);
        }
    });
    sce->Start();
}
