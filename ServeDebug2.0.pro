#-------------------------------------------------
#
# Project created by QtCreator 2013-08-02T22:43:25
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ServeDebug2.0
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    source/communicate/network/networkdialog.cpp \
    source/communicate/serialcom/serialdialog.cpp \
    source/communicate/serialcom/win_qextserialport.cpp \
    source/communicate/serialcom/qextserialport.cpp \
    source/communicate/serialcom/qextserialbase.cpp \
    source/communicate/receivedata/receivedata.cpp \
    source/insanalysis/commandanalysis.cpp \
    source/servodebug/serverdebug.cpp \
    source/downserve/armmanage.cpp \
    source/servodebug/Graph/keypointitem.cpp \
    source/servodebug/Graph/graphscene.cpp \
    source/servodebug/Graph/graphitem.cpp \
    source/servodebug/Graph/bubbleitem.cpp \
    source/servodebug/Graph/boundcursoritem.cpp \
    source/servodebug/SplineSample/spline.cpp \
    source/servodebug/SplineSample/myspline.cpp \
    source/logdebug/logdebug.cpp \
    source/settings/settingdialog.cpp \
    source/keyframewidget/keyframewidget.cpp \
    source/keyframewidget/spinboxdelegate.cpp \
    source/keyframewidget/setenabledialog.cpp \
    source/about/about.cpp

HEADERS  += mainwindow.h \
    source/communicate/network/networkdialog.h \
    source/communicate/serialcom/serialdialog.h \
    source/communicate/serialcom/win_qextserialport.h \
    source/communicate/serialcom/qextserialport.h \
    source/communicate/serialcom/qextserialbase.h \
    source/communicate/receivedata/receivedata.h \
    source/insanalysis/communicateDefine.h \
    source/insanalysis/commandanalysis.h \
    source/globaldefine/mainStructure.h \
    source/globaldefine/upDefine.h \
    source/servodebug/serverdebug.h \
    source/downserve/armmanage.h \
    source/servodebug/Graph/keypointitem.h \
    source/servodebug/Graph/graphscene.h \
    source/servodebug/Graph/graphitem.h \
    source/servodebug/Graph/bubbleitem.h \
    source/servodebug/Graph/boundcursoritem.h \
    source/servodebug/SplineSample/spline.h \
    source/servodebug/SplineSample/myspline.h \
    source/logdebug/logdebug.h \
    source/settings/settingdialog.h \
    source/keyframewidget/keyframewidget.h \
    source/keyframewidget/spinboxdelegate.h \
    source/keyframewidget/setenabledialog.h \
    source/about/about.h

FORMS    += mainwindow.ui \
    source/communicate/network/networkdialog.ui \
    source/communicate/serialcom/serialdialog.ui \
    source/servodebug/serverdebug.ui \
    source/downserve/armmanage.ui \
    source/servodebug/analogdata.ui \
    source/logdebug/logdebug.ui \
    source/settings/settingdialog.ui \
    source/keyframewidget/keyframewidget.ui \
    source/about/about.ui

RESOURCES += \
    MainWindowImage.qrc \
    documentImage.qrc

CONFIG += static
