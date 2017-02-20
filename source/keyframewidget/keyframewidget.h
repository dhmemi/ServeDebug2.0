#ifndef KEYFRAMEWIDGET_H
#define KEYFRAMEWIDGET_H

#include "source/globaldefine/mainStructure.h"
#include "source/servodebug/SplineSample/myspline.h"

#include <QDockWidget>
#include <QVariantMap>
class QTableWidget;
class QTableWidgetItem;
class QPushButton;
class QSlider;
class QSpinBox;
class QSplitter;
class QAction;
class QMenu;

namespace Ui {
class KeyFrameWidget;
}

class KeyFrameWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit KeyFrameWidget(QVariantMap param, QWidget *parent = 0);
    ~KeyFrameWidget();

    void setParam(QVariantMap param);

signals:
    void getDownPosSignal();
    void gotoFrameSignal(QVector<ServoInfo> infos);
    void dataChangedSignal(ACInfoMap info);
    void servoEnableChangedSignal(QByteArray enableData);

public slots:
    void openFileSlot(QString abfileName);
    void setSliderPosSlot(ServoInfoMap info);
    void setSliderConnectionSlot(uchar type, ServoInfoMap info);

    void enableSomeServoDialog();

    void ghpInsertSlot(int time);
    void ghpMoveSlot(int oldtime, int newtime);
    void ghpModifySlot(int id, int time, int newData);
    void ghpRemoveSlot(int time);
    void ghpSelected(int index);
    void ghpRefreshSlot();

private slots:
    void onCurrentCellChanged(int curRow, int curCol, int preRow, int preCol);
    void onSliderDataChanged();
    void onSliderReleased();
    void onkeyFrameDataChanged(QTableWidgetItem* item);
    void onSliderReturnClicked();
    void onInsertButtonClicked();
    void onSliderWidgetchanged();
    void onCustomContextRequest_1(QPoint pos);
    void onCustomContextRequest_2(QPoint pos);
    void removeFrameSlot();
    void gotoFrameActionSlot();
    void getDownPosActionSlot();
private:
    void initSence();
    //void groupSignals();
    void dataChanged();
    bool initModelHeader(QString header, int rowCount);
    void groupWarning(QString str);
    void closeEvent(QCloseEvent *event);

    QVariantMap paramMap;
    ServoInfoMap zhServoInfoMap;
    uchar servoType;
    int orientation;

    QTableWidget* sliderWidget;
    QTableWidget* keyFrameWidget;
    QPushButton* sliderReturnUp;
    QPushButton* sliderReturnDown;
    QWidget* returnWidget;
    QPushButton* insertButton;
    QAction* removeAction;
    QAction* gotoAction;
    QAction* getdownPos;
    QMenu* actionMenu;
    QVector<QSpinBox*> spinBoxVector;
    QVector<QSlider*> sliderVector;
    QSplitter* mainSplitter;

    Ui::KeyFrameWidget *ui;
};

#endif // KEYFRAMEWIDGET_H
