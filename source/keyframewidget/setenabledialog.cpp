#include "setenabledialog.h"
#include <QCheckBox>
#include <QPushButton>
#include <QLayout>

#include <QDebug>

SetEnableDialog::SetEnableDialog(ServoInfoMap *servoInfos, bool *ok, QWidget *parent)
    : QDialog(parent), zhServoInfoMap(servoInfos), isOk(ok)
{
    this->setWindowTitle("勾选要使能的舵机：");

    initSence();

    connect(selectAllBox, &QCheckBox::clicked, this, &SetEnableDialog::onSelectAllBoxClicked);
    connect(okButton, &QPushButton::clicked, this, &SetEnableDialog::onOkButtonClicked);
    connect(cancelButton, &QPushButton::clicked, this, &SetEnableDialog::onCancelButtonClicked);
}

SetEnableDialog::~SetEnableDialog()
{
}

void SetEnableDialog::onSelectAllBoxClicked()
{
    for(int i=0; i<servoInfoBoxes.size(); i++){
        servoInfoBoxes.at(i)->setChecked(selectAllBox->isChecked());
    }
}

void SetEnableDialog::onOkButtonClicked()
{
    int j = 0;
    for(int i: zhServoInfoMap->keys()){
        (*zhServoInfoMap)[i].isEnabled = servoInfoBoxes.at(j)->isChecked();
        j++;
    }
    *isOk = true;
    close();
}

void SetEnableDialog::onCancelButtonClicked()
{
    *isOk = false;
    close();
}

void SetEnableDialog::initSence()
{
    QCheckBox *boxPtr;
    for(int i: zhServoInfoMap->keys()){
        boxPtr = new QCheckBox(this);
        boxPtr->setText(QString::number(i)+"-"+(*zhServoInfoMap)[i].name);
        boxPtr->setChecked((*zhServoInfoMap)[i].isEnabled);
        servoInfoBoxes.push_back(boxPtr);
    }

    selectAllBox = new QCheckBox("Select All", this);
    selectAllBox->setChecked(false);

    okButton = new QPushButton("Ok", this);
    cancelButton = new QPushButton("Cancel", this);

    QGridLayout *mainLayout = new QGridLayout(this);
    for(int i = 0; i < servoInfoBoxes.size(); i++){
        mainLayout->addWidget(servoInfoBoxes.at(i), i/5, i%5);
    }
    qDebug() << "dialog: servo numer" << servoInfoBoxes.size();
    if(servoInfoBoxes.size()>=5){
        mainLayout->addWidget(selectAllBox, servoInfoBoxes.size()/5+2, 1);
        mainLayout->addWidget(okButton, servoInfoBoxes.size()/5+2, 3);
        mainLayout->addWidget(cancelButton, servoInfoBoxes.size()/5+2, 4);
    }else if(servoInfoBoxes.size()>2){
        mainLayout->addWidget(selectAllBox, 2, servoInfoBoxes.size()-3);
        mainLayout->addWidget(okButton, 2, servoInfoBoxes.size()-2);
        mainLayout->addWidget(cancelButton, 2, servoInfoBoxes.size()-1);
    }else{
        selectAllBox->hide();
        mainLayout->addWidget(okButton, 2, 0);
        mainLayout->addWidget(cancelButton, 2, 1);
    }

    setLayout(mainLayout);
}
