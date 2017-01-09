#ifndef SETENABLEDIALOG_H
#define SETENABLEDIALOG_H

#include "source/globaldefine/mainStructure.h"
#include <QDialog>

class QCheckBox;
class QPushButton;

class SetEnableDialog : public QDialog
{
public:
    SetEnableDialog(ServoInfoMap *servoInfos, bool *ok, QWidget *parent = 0);
    ~SetEnableDialog();

private slots:
    void onSelectAllBoxClicked();
    void onCancelButtonClicked();
    void onOkButtonClicked();
private:
    void initSence();

    ServoInfoMap *zhServoInfoMap;
    bool *isOk;

    QVector<QCheckBox*> servoInfoBoxes;
    QCheckBox *selectAllBox;
    QPushButton *cancelButton;
    QPushButton *okButton;
};

#endif // SETENABLEDIALOG_H
