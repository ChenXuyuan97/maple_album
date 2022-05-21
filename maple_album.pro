QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

win32: QT += winextras xml

win32: LIBS += -lUser32

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    controls/FloatingActionButton.cpp \
    main.cpp \
    MainWindow.cpp \
    photo/photoitem.cpp \
    photoscene.cpp \
    photoview.cpp \
    screen_capture/ScreenCaptureEngine.cpp

HEADERS += \
    MainWindow.h \
    controls/FloatingActionButton.h \
    photo/photoitem.h \
    photoscene.h \
    photoview.h \
    screen_capture/ScreenCaptureEngine.h

FORMS += \
    MainWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    album.qrc
