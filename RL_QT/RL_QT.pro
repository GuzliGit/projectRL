QT       += core gui
QT += charts
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    agent/agentfactory.cpp \
    agent/agentobj.cpp \
    algorithms/q_learn_exp.c \
    createproj_dialog.cpp \
    custom_tools/miniflowlayout.cpp \
    custom_tools/widgetwithflowlayout.cpp \
    environment/cellfactory.cpp \
    environment/cellitem.cpp \
    environment/environmenteditor.cpp \
    environment/floorcell.cpp \
    environment/gatecell.cpp \
    environment/riskycell.cpp \
    environment/rl_scene.cpp \
    environment/wallcell.cpp \
    main.cpp \
    mainwindow.cpp \
    trainers/qlearningtrainer.cpp

HEADERS += \
    agent/agentfactory.h \
    agent/agentobj.h \
    algorithms/q_learn_exp.h \
    createproj_dialog.h \
    custom_tools/miniflowlayout.h \
    custom_tools/widgetwithflowlayout.h \
    environment/cellfactory.h \
    environment/cellitem.h \
    environment/environmenteditor.h \
    environment/floorcell.h \
    environment/gatecell.h \
    environment/riskycell.h \
    environment/rl_scene.h \
    environment/wallcell.h \
    mainwindow.h \
    trainers/qlearningtrainer.h

FORMS += \
    createproj_dialog.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

DISTFILES += \
    memory_files/docks.mem
