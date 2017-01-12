#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QVariantMap>
class QToolBar;
class QToolButton;
class QLabel;
class NetWorkDialog;
class SerialDialog;
class CommandAnalysis;
class LogDebug;
class ServerDebug;
class KeyFrameWidget;
class ArmManage;
class GraphScene;
class SettingDialog;
class about;

class QString;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void initToolBar(); //Each toolBar has its parents,we do not need to delete it.
    void groupSignals();

    //init children class
    void initNetWorkDialog();
    void initSerialDialog();
    void initCommandAnalysis();
    void initLogDebug();
    void initServerDebug();
    void initKeyFrameWidget();
    void initArmManage();
    void initSetting();
    void initAbout();
    void initButtonsState();


    void closeEvent(QCloseEvent *event);

private slots:
    void serialButtonSlot();
    void networkButtonSlot();
    void servoDebugButtonSlot();
    void downserveButtonSlot();
    void logrecordButtonSlot();
    void settingButtonSlot();
    void aboutButtonSlot();
    void setStateSlot(QString str);

private:
    void createCommandAnalysis();
    void destroyCommandAnalysisSlot();

    void getSetting();
    void readSettingFile();
    void writeSettingFile();
    void saveNetSettingSlot(QVariantMap param);
    void saveComSettingSlot(QVariantMap param);

    void groupzhSceneSignals(GraphScene* p);
    void setServoTypeState(unsigned char type);

signals:
    void startConnect(QString str);

    //connect out
public:
    //children class
    NetWorkDialog* zhNetWork;
    SerialDialog* zhSerial;
    CommandAnalysis* zhCommandAnalysis;
    ServerDebug* zhServerDebug;
    KeyFrameWidget* zhKeyFrameWidget;
    ArmManage* zhArmManage;
    SettingDialog* zhSetting;
    about* zhabout;

private:
    Ui::MainWindow *ui;

    //MainWindow Scene Tools
    QToolBar* mainToolBar;
    QToolButton* serialButton;
    QToolButton* networkButton;
    QToolButton* servoDebugButton;
    QToolButton* logrecordButton;
    QToolButton* downserveButton;
    QToolButton* settingButton;
    QToolButton* aboutButton;

    //show State
    QLabel* stateLabel;

    QVariantMap paramMap;
    //children class
    LogDebug* zhLogDebug;
};

#endif // MAINWINDOW_H
