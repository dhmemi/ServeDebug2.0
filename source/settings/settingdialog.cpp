#include "settingdialog.h"
#include "ui_settingdialog.h"

#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QJsonDocument>

SettingDialog::SettingDialog(QVariantMap param, QWidget *parent) :
    QDialog(parent), currentSetting(param), ui(new Ui::SettingDialog)
{
    ui->setupUi(this);

    initPrevious();
    groupSignal();
}

QVariantMap SettingDialog::getSetting()
{
    return currentSetting;
}

void SettingDialog::initPrevious()
{
    ui->csvPathEdit->setText(currentSetting["csvPath"].toString());
    ui->binPathEdit->setText(currentSetting["binPath"].toString());
    ui->odrPathEdit->setText(currentSetting["odrPath"].toString());
    ui->netWaitBox->setRange(0,500);
    ui->netWaitBox->setValue(currentSetting["netWait"].toInt());
    ui->modelBox->setCurrentIndex(currentSetting["debugModel"].toInt());
    ui->tabDrcBox->setCurrentIndex(currentSetting["tabDrc"].toInt()-1);
    ui->msgLenBox->setRange(0,2048);
    ui->msgLenBox->setSingleStep(50);
    ui->msgLenBox->setValue(currentSetting["msgLenth"].toInt());
    ui->msgLenLabel->setChecked(currentSetting["isMsgLimited"].toBool());
    ui->msgLenBox->setEnabled(ui->msgLenLabel->isChecked());
    ui->interpolateTimeBox->setRange(0, 500);
    ui->interpolateTimeBox->setValue(currentSetting["interpolateTime"].toInt());
    ui->interpolateTimeLabel->setChecked(currentSetting["isInterpolateTimeSet"].toBool());
    ui->interpolateTimeBox->setEnabled(ui->interpolateTimeLabel->isChecked());
    ui->logFormatBox->setCurrentText(QString::number(currentSetting["logFormat"].toInt()));
}

void SettingDialog::groupSignal()
{
    connect(ui->csvPathButton, &QPushButton::clicked, this, &SettingDialog::csvBrowseSlot);
    connect(ui->binPathButton, &QPushButton::clicked, this, &SettingDialog::binBrowseSlot);
    connect(ui->odrPathButton, &QPushButton::clicked, this, &SettingDialog::odrBrowseSlot);
    connect(ui->okButton, &QPushButton::clicked, this, &SettingDialog::okSlot);
    connect(ui->cancelButton, &QPushButton::clicked, this, &SettingDialog::cancelSlot);
    connect(ui->applyButton, &QPushButton::clicked, this, &SettingDialog::applySlot);
    connect(ui->msgLenLabel, &QCheckBox::stateChanged, ui->msgLenBox, &QSpinBox::setEnabled);
    connect(ui->interpolateTimeLabel, &QCheckBox::stateChanged, ui->interpolateTimeBox, &QSpinBox::setEnabled);
}

void SettingDialog::groupWarning(QString str)
{
    QMessageBox::warning(this,QObject::tr("Wrong"),str);
}

void SettingDialog::csvBrowseSlot()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
        QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(!path.isEmpty()){
        ui->csvPathEdit->setText(path);
    }

}

void SettingDialog::binBrowseSlot()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
        QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(!path.isEmpty()){
        ui->binPathEdit->setText(path);
    }
}

void SettingDialog::odrBrowseSlot()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
        QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(!path.isEmpty()){
        ui->odrPathEdit->setText(path);
    }
}

void SettingDialog::okSlot()
{
    applySlot();
    close();
}

void SettingDialog::cancelSlot()
{
    close();
}

void SettingDialog::applySlot()
{
    QDir temp;
    if(!QDir(ui->csvPathEdit->text()).exists()){
        if(!temp.mkpath(ui->csvPathEdit->text())){
            groupWarning(tr("无法创建文件夹!"));
            return;
        }
    }
    if(!QDir(ui->binPathEdit->text()).exists()){
        if(!temp.mkpath(ui->binPathEdit->text())){
            groupWarning(tr("无法创建文件夹!"));
            return;
        }
    }
    if(!QDir(ui->odrPathEdit->text()).exists()){
        if(!temp.mkpath(ui->odrPathEdit->text())){
            groupWarning(tr("无法创建文件夹!"));
            return;
        }
    }

    currentSetting.insert("csvPath", ui->csvPathEdit->text());
    currentSetting.insert("binPath", ui->binPathEdit->text());
    currentSetting.insert("odrPath", ui->odrPathEdit->text());

    currentSetting.insert("netWait", ui->netWaitBox->value());
    currentSetting.insert("isMsgLimited", ui->msgLenLabel->isChecked());
    currentSetting.insert("msgLenth", ui->msgLenBox->value());

    currentSetting.insert("debugModel", ui->modelBox->currentIndex());
    currentSetting.insert("isInterpolateTimeSet", ui->interpolateTimeLabel->isChecked());
    currentSetting.insert("interpolateTime",ui->interpolateTimeBox->value());
    currentSetting.insert("tabDrc", ui->tabDrcBox->currentIndex()+1);
    currentSetting.insert("logFormat", ui->logFormatBox->currentText().toInt());

    emit settingChanged();
}

SettingDialog::~SettingDialog()
{
    delete ui;
}
