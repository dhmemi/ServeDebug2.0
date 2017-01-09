#ifndef ARMMANAGE_H
#define ARMMANAGE_H

#include <QDockWidget>
#include <vector>
#include <QCloseEvent>
#include <QVariantMap>

using namespace std;

class QString;
class QStringList;
class QModelIndex;
class QTabWidget;
class QDockWidget;
class QTableWidget;
class QTreeWidget;
class QSplitter;
class QTreeWidgetItem;
class QPushButton;

namespace Ui {
class ArmManage;
}

class ArmManage : public QDockWidget
{
    Q_OBJECT
public:
    explicit ArmManage(QVariantMap param, QWidget *parent = NULL);
    ~ArmManage();
    void setParam(QVariantMap param);

public slots:
    //from CommandAnalysis
    void downReplyActionSlot(QStringList actions);
    void downReplySequenceSlot(QStringList sequence);
    void downUploadSequenceSlot(QString sequence, QStringList actions);

signals:
    //to KeyFrameManager?
    void upSaveActionSignal(QString abcsvName ,QString abbinName);
    void upOpenActionSiganl(QString abfileName);
    //to ?
    void downloadActionSignal(QString abbinName);
    //to CommandAnalysis
    void downloadSequenceSignal(QString fileName, QStringList actionNames);
    void downRefreshActionSignal();
    void downRefreshSequenceSignal();
    void downUploadSequenceSiganl(QString sequence);
    void downRenameActionSignal(QString oldName, QString newName);
    void downExecuteActionSignal(QString action);
    void downDeleteActionSignal(QString action);
    void downExecuteSequenceSignal(QString sequence);
    void downRenameSequenceSignal(QString oldName, QString newName);
    void downDeleteSequenceSignal(QString sequence);
    void closed();

private slots:
    void upRefreshActionSlot();
    void upSaveActionSlot();
    void upRenameActionSlot();
    void upCoverActionSlot();
    void upDeleteActionSlot();
    void downloadActionSlot();
    void upaddActionToSequenceSlot();
    void upRefreshSequenceSlot();
    void upCreateSequenceSlot();
    void upSaveSequenceSlot();
    void upRenameSequenceSlot();
    void upDeleteSequenceSlot();
    void upMoveUpActionSlot();
    void upMoveDownActionSlot();
    void downloadSequenceSlot();

    void downRefreshActionSlot();
    void downRenameActionSlot();
    void downExecuteActionSlot();
    void downDeleteActionSlot();
    void downaddActionToSequenceSlot();
    void downRefreshSequenceSlot();
    void downCreateSequenceSlot();
    void downSaveSequenceSlot();
    void downExecuteSequenceSlot();
    void downRenameSequenceSlot();
    void downDeleteSequenceSlot();
    void downMoveUpActionSlot();
    void downMoveDownActionSlot();

    void upOnActionDoubleClicked(int row, int col);
    void upOnSequenceDoubleClicked(QTreeWidgetItem* item, int col);
    void downOnSequenceDoubleClicked(QTreeWidgetItem* item, int col);

private:
    void initSence();
    void groupSignals();
    void controTools();
    void groupWarning(QString str);
    void closeEvent(QCloseEvent *event);

    QVariantMap paramMap;
    QString upActiveAction; //current editing action file

    //filemanager-ui
    QTabWidget* mainTabWidget;
        QSplitter* upSplitter;
            QWidget* upActionWidget;
                QTableWidget* upActionTable;
                QPushButton* upRefreshActionButton;
                QPushButton* upSaveActionButton;
                QPushButton* upRenameActionButton;
                QPushButton* upCoverActionButton;
                QPushButton* upDeleteActionButton;
                QPushButton* downloadActionButton;
            QPushButton* upaddActionToSequenceButton;
            QWidget* upSequenceWidget;
                QTreeWidget* upSequenceTree;
                QPushButton* upRefreshSequenceButton;
                QPushButton* upCreateSequenceButton;
                QPushButton* upSaveSequenceButton;
                QPushButton* upRenameSequenceButton;
                QPushButton* upDeleteSequenceButton;
                QPushButton* upMoveUpActionButton;
                QPushButton* upMoveDownActionButton;
                QPushButton* downloadSequenceButton;
        QSplitter* downSplitter;
            QWidget* downActionWidget;
                QTableWidget* downActionTable;
                QPushButton* downRefreshActionButton;
                QPushButton* downRenameActionButton;
                QPushButton* downExecuteActionButton;
                QPushButton* downDeleteActionButton;
            QPushButton* downaddActionToSequenceButton;
            QWidget* downSequenceWidget;
                QTreeWidget* downSequenceTree;
                QPushButton* downRefreshSequenceButton;
                QPushButton* downCreateSequenceButton;
                QPushButton* downSaveSequenceButton;
                QPushButton* downExecuteSequenceButton;
                QPushButton* downRenameSequenceButton;
                QPushButton* downDeleteSequenceButton;
                QPushButton* downMoveUpActionButton;
                QPushButton* downMoveDownActionButton;

    Ui::ArmManage *ui;
};

#endif // ARMMANAGE_H
