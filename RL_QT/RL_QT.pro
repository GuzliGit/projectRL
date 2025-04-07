QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cellfactory.cpp \
    cellitem.cpp \
    createproj_dialog.cpp \
    environmenteditor.cpp \
    floorcell.cpp \
    main.cpp \
    mainwindow.cpp \
    miniflowlayout.cpp \
    rl_scene.cpp \
    wallcell.cpp \
    widgetwithflowlayout.cpp

HEADERS += \
    cellfactory.h \
    cellitem.h \
    createproj_dialog.h \
    environmenteditor.h \
    floorcell.h \
    mainwindow.h \
    miniflowlayout.h \
    rl_scene.h \
    wallcell.h \
    widgetwithflowlayout.h

FORMS += \
    createproj_dialog.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
