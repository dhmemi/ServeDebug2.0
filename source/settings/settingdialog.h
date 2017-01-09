#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include "source/globaldefine/upDefine.h"
#include <QDialog>
#include <QVariantMap>

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(QVariantMap param, QWidget *parent = 0);
    ~SettingDialog();
    QVariantMap getSetting();
signals:
    void settingChanged();
private slots:
    void applySlot();
    void okSlot();
    void cancelSlot();
    void csvBrowseSlot();
    void binBrowseSlot();
    void odrBrowseSlot();

private:
    void initPrevious();
    void groupSignal();
    void groupWarning(QString str);

    QVariantMap currentSetting;


    Ui::SettingDialog *ui;
};

#endif // SETTINGDIALOG_H
