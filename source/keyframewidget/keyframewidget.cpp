#include "keyframewidget.h"
#include "source/globaldefine/upDefine.h"
#include "setenabledialog.h"
#include "ui_keyframewidget.h"
#include "spinboxdelegate.h"

#include <QMessageBox>
#include <QPushButton>
#include <QSlider>
#include <QLayout>
#include <QSpinBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QStringList>
#include <QFile>
#include <QSplitter>
#include <QDebug>
#include <QHeaderView>
#include <QScrollBar>
#include <QAction>
#include <QMenu>
#include <QCloseEvent>

KeyFrameWidget::KeyFrameWidget(QVariantMap param, QWidget *parent)
    : QDockWidget(parent), ui(new Ui::KeyFrameWidget), paramMap(param)
{
    ui->setupUi(this);
    orientation = paramMap["tabDrc"].toInt();

    initSence();
    setWindowTitle("关键帧编辑器");
}

KeyFrameWidget::~KeyFrameWidget()
{
    delete ui;
}

void KeyFrameWidget::setParam(QVariantMap param)
{
    paramMap = param;
}

void KeyFrameWidget::openFileSlot(QString abfileName)
{
    //若不解连接，再次打开文件时会引起错误，原因暂未知
    disconnect(sliderWidget,&QTableWidget::itemChanged,this,&KeyFrameWidget::onSliderWidgetchanged);

    QFile openFile(abfileName);
    if(!openFile.open(QIODevice::ReadOnly)){
        groupWarning(QObject::tr("Fail to open file!"));
        return;
    }
    QString fileData(openFile.readAll());
    openFile.close();
    if(fileData.isEmpty()){
        groupWarning(QObject::tr("file is empty !"));
        return;
    }
    QStringList fileLines(fileData.split("\r\n", QString::SkipEmptyParts));
    if(!initModelHeader(fileLines.at(0), fileLines.size()-1)){
        return;
    }
    QString templine;
    QStringList lineData;
    for(int i=1; i<fileLines.size(); i++){
        templine.clear();
        templine = fileLines.at(i);
        lineData.clear();
        lineData = templine.split(",", QString::SkipEmptyParts);
        for(int j=0; j<lineData.size(); j++){
            QString dataItem = lineData.at(j);
            QTableWidgetItem *tempItem = new QTableWidgetItem;
            tempItem->setData(Qt::DisplayRole, dataItem.toInt());
            if(Qt::Vertical == orientation){
                keyFrameWidget->setItem(i-1, j, tempItem);
            }else if(Qt::Horizontal == orientation){
                keyFrameWidget->setItem(j, i-1, tempItem);
            }
        }
    }
    connect(sliderWidget,&QTableWidget::itemChanged,this,&KeyFrameWidget::onSliderWidgetchanged);
    dataChanged();
}

bool KeyFrameWidget::initModelHeader(QString header, int rowCount)
{
    QStringList headerLabels(header.split(",", QString::SkipEmptyParts));
    if(paramMap["debugModel"].toInt() == MODEL_CONNECTED){
        if(sliderVector.size() != 0){
            if(headerLabels.size() != sliderVector.size()+1){
                groupWarning("File cannot match with current information !");
                return false;
            }else{
                if(Qt::Vertical == orientation){
                    keyFrameWidget->setRowCount(rowCount);
                    for(int i=1; i<rowCount; i++){
                        keyFrameWidget->setItemDelegateForColumn(i, new SpinBoxDelegate(this));
                    }
                }else if(Qt::Horizontal == orientation){
                    keyFrameWidget->setColumnCount(rowCount);
                    for(int i=1; i<rowCount; i++){
                        keyFrameWidget->setItemDelegateForRow(i, new SpinBoxDelegate(this));
                    }
                }
                return true;
            }
        }
    }
    sliderWidget->clear();
    keyFrameWidget->clear();
    for(int i=0; i<sliderVector.size(); i++){
        delete sliderVector.at(i);
        delete spinBoxVector.at(i);
    }
    sliderVector.clear();
    spinBoxVector.clear();

    if(Qt::Vertical == orientation){
        sliderWidget->setColumnCount(headerLabels.size());
        sliderWidget->setRowCount(2);
        sliderWidget->setHorizontalHeaderLabels(headerLabels);
        sliderWidget->setRowHeight(1, 230);
        sliderWidget->setItem(0, 0, new QTableWidgetItem);

        //returnWidget = new QWidget(this);
        sliderReturnUp = new QPushButton("ReU", this);
        //sliderReturnDown = new QPushButton("ReD", returnWidget);
        //QVBoxLayout *returnLayout = new QVBoxLayout(returnWidget);
        //returnLayout->addWidget(sliderReturnUp);
        //returnLayout->addWidget(sliderReturnDown);
        //returnWidget->setLayout(returnLayout);
        sliderWidget->setCellWidget(1,0,sliderReturnUp);

        QSpinBox* spinBox;
        QSlider* slider;
        for(int i = 1; i<sliderWidget->columnCount(); i++){
            spinBox = new QSpinBox(this);
            spinBox->setRange(0, 1023);
            spinBox->setValue(512);
            sliderWidget->setCellWidget(0,i,spinBox);
            spinBoxVector.push_back(spinBox);
            slider = new QSlider(Qt::Vertical,this);
            slider->setRange(0, 1023);
            slider->setValue(512);
            sliderWidget->setCellWidget(1,i,slider);
            sliderVector.push_back(slider);
            connect(sliderReturnUp,&QPushButton::clicked,this,&KeyFrameWidget::onSliderReturnClicked);
            connect(spinBox, SIGNAL(valueChanged(int)), slider, SLOT(setValue(int)));
            connect(slider, &QSlider::valueChanged, spinBox, &QSpinBox::setValue);
            connect(spinBox, &QSpinBox::editingFinished, this, &KeyFrameWidget::onSliderDataChanged);
            connect(slider, &QSlider::valueChanged, this, &KeyFrameWidget::onSliderDataChanged);
            connect(slider, &QSlider::sliderReleased, this, &KeyFrameWidget::onSliderReleased);
        }
        keyFrameWidget->setColumnCount(headerLabels.size());
        keyFrameWidget->setRowCount(rowCount);
        keyFrameWidget->setHorizontalHeaderLabels(headerLabels);

    }else if(Qt::Horizontal == orientation){
        sliderWidget->setRowCount(headerLabels.size());
        sliderWidget->setColumnCount(2);
        sliderWidget->setVerticalHeaderLabels(headerLabels);
        sliderWidget->setColumnWidth(1, 230);
        sliderWidget->setItem(0, 0, new QTableWidgetItem);

        //returnWidget = new QWidget(this);
        sliderReturnUp = new QPushButton("ReU", this);
        //sliderReturnDown = new QPushButton("ReD", returnWidget);R
        //QVBoxLayout *returnLayout = new QVBoxLayout(returnWidget);
        //returnLayout->addWidget(sliderReturnUp);
        //returnLayout->addWidget(sliderReturnDown);
        //returnWidget->setLayout(returnLayout);
        sliderWidget->setCellWidget(0,1,sliderReturnUp);

        QSpinBox* spinBox;
        QSlider* slider;
        for(int i = 1; i<sliderWidget->rowCount(); i++){
            spinBox = new QSpinBox(this);
            spinBox->setRange(0, 1023);
            spinBox->setValue(512);
            sliderWidget->setCellWidget(i,0,spinBox);
            spinBoxVector.push_back(spinBox);
            slider = new QSlider(Qt::Horizontal,this);
            slider->setRange(0, 1023);
            slider->setValue(512);
            sliderWidget->setCellWidget(i,1,slider);
            sliderVector.push_back(slider);
            connect(sliderReturnUp,&QPushButton::clicked,this,&KeyFrameWidget::onSliderReturnClicked);
            connect(spinBox, SIGNAL(valueChanged(int)), slider, SLOT(setValue(int)));
            connect(slider, &QSlider::valueChanged, spinBox, &QSpinBox::setValue);
            connect(spinBox, &QSpinBox::editingFinished, this, &KeyFrameWidget::onSliderDataChanged);
            connect(slider, &QSlider::valueChanged, this, &KeyFrameWidget::onSliderDataChanged);
        }
        keyFrameWidget->setRowCount(headerLabels.size());
        keyFrameWidget->setColumnCount(rowCount);
        keyFrameWidget->setVerticalHeaderLabels(headerLabels);
    }
    return true;
}

void KeyFrameWidget::enableSomeServoDialog()
{
    bool ok = false;
    SetEnableDialog zhSetEnableDialog(&zhServoInfoMap, &ok, this);
    zhSetEnableDialog.exec();
    if(ok){
        QByteArray enableData;
        for(int i: zhServoInfoMap.keys()){
            enableData.append(quint8(i));
            enableData.append(zhServoInfoMap[i].isEnabled);
        }
        emit servoEnableChangedSignal(enableData);
    }
}

void KeyFrameWidget::groupWarning(QString str)
{
    QMessageBox::warning(this,QObject::tr("Wrong"),str);
}

void KeyFrameWidget::onCurrentCellChanged(int curRow, int curCol, int preRow, int preCol)
{
    for(int i=0; i<sliderVector.size(); i++){ //解信号槽，避免触发slider.valueChanged()改变后面的数据
        disconnect(sliderVector.at(i), &QSlider::valueChanged, this, &KeyFrameWidget::onSliderDataChanged);
    }

    if(keyFrameWidget->item(curRow,curCol) != nullptr){
        if(Qt::Vertical == orientation){
            sliderWidget->item(0,0)->setData(Qt::DisplayRole,
                                             keyFrameWidget->item(curRow, 0)->data(Qt::DisplayRole).toInt());
            for(int i=1; i<keyFrameWidget->columnCount(); i++){
                sliderVector.at(i-1)->setValue(
                            keyFrameWidget->item(curRow,i)->data(Qt::DisplayRole).toInt());
                spinBoxVector.at(i-1)->setValue(
                            keyFrameWidget->item(curRow,i)->data(Qt::DisplayRole).toInt());
            }
        }else if(Qt::Horizontal == orientation){
            sliderWidget->item(0,0)->setData(Qt::DisplayRole,
                                             keyFrameWidget->item(0, curCol)->data(Qt::DisplayRole).toInt());
            for(int i=1; i<keyFrameWidget->rowCount(); i++){
                sliderVector.at(i-1)->setValue(
                            keyFrameWidget->item(i,curCol)->data(Qt::DisplayRole).toInt());
                spinBoxVector.at(i-1)->setValue(
                            keyFrameWidget->item(i,curCol)->data(Qt::DisplayRole).toInt());
            }
        }
    }

    for(int i=0; i<sliderVector.size(); i++){//重新连接信号槽
        connect(sliderVector.at(i), &QSlider::valueChanged, this, &KeyFrameWidget::onSliderDataChanged);
    }

}

void KeyFrameWidget::onkeyFrameDataChanged(QTableWidgetItem *item)
{
    if(keyFrameWidget->currentItem() == nullptr){
        return;
    }
    if(Qt::Vertical == orientation){
        if(item->column()>0 &&
            sliderVector.at(item->column()-1)->value() != item->data(Qt::DisplayRole).toInt()){
            sliderVector.at(item->column()-1)->setValue(item->data(Qt::DisplayRole).toInt());
            spinBoxVector.at(item->column()-1)->setValue(item->data(Qt::DisplayRole).toInt());
        }else if(item->column() == 0){
            keyFrameWidget->sortByColumn(0, Qt::AscendingOrder);
            sliderWidget->item(0, 0)->setData(
                        Qt::DisplayRole, keyFrameWidget->currentItem()->data(Qt::DisplayRole));
        }
    }
    dataChanged();
}

void KeyFrameWidget::onSliderDataChanged()
{
    if(keyFrameWidget->currentItem() != nullptr){
        if(Qt::Vertical == orientation){
            for(int i=0; i<sliderVector.size(); i++){
                keyFrameWidget->item(keyFrameWidget->currentRow(),i+1)->
                        setData(Qt::DisplayRole,sliderVector.at(i)->value());
            }
        }else if(Qt::Horizontal == orientation){
            for(int i=0; i<sliderVector.size(); i++){
                keyFrameWidget->item(i+1,keyFrameWidget->currentColumn())->
                        setData(Qt::DisplayRole,sliderVector.at(i)->value());
            }
        }
        dataChanged();
    }
}

void KeyFrameWidget::onSliderReleased()
{
    if(paramMap["debugModel"].toInt() == MODEL_CURRENT){
        gotoFrameActionSlot();
    }
}

void KeyFrameWidget::onSliderReturnClicked()
{
    for(int i=0; i<sliderVector.size(); i++){
        spinBoxVector.at(i)->setValue(512);
        sliderVector.at(i)->setValue(512);
    }
}

void KeyFrameWidget::onInsertButtonClicked()
{
    if(sliderVector.isEmpty()){
        return;
    }
    int i = 0;
    bool tag = false;
    if(Qt::Vertical == orientation){
        for(i=0; i<keyFrameWidget->rowCount() &&
            keyFrameWidget->item(i,0)->data(Qt::DisplayRole).toInt()
            <= sliderWidget->item(0,0)->data(Qt::DisplayRole).toInt(); i++){
            if(keyFrameWidget->item(i, 0)->data(Qt::DisplayRole).toInt()
                    == sliderWidget->item(0, 0)->data(Qt::DisplayRole).toInt()){
                for(int j=1; j<keyFrameWidget->columnCount(); j++){
                    keyFrameWidget->item(i,j)->setData(Qt::DisplayRole,sliderVector.at(j-1)->value());
                }
                tag = true;
                break;
            }
        }
        if(!tag){
            keyFrameWidget->insertRow(i);
            QTableWidgetItem *tempItem = new QTableWidgetItem;
            tempItem->setData(Qt::DisplayRole,sliderWidget->item(0,0)->data(Qt::DisplayRole).toInt());
            keyFrameWidget->setCurrentCell(-1, 0); // 使 keyFrameWidget.currentCell == nullptr,否则下一步会促发信号槽引起错误
            keyFrameWidget->setItem(i,0,tempItem);
            for(int j=1; j<keyFrameWidget->columnCount(); j++){
                tempItem = new QTableWidgetItem;
                tempItem->setData(Qt::DisplayRole,sliderVector.at(j-1)->value());
                keyFrameWidget->setItem(i,j, tempItem);
            }
            keyFrameWidget->setCurrentCell(i, 0);
        }
    }else if(Qt::Horizontal == orientation){
        for(i=0; i<keyFrameWidget->columnCount() &&
            keyFrameWidget->item(0,i)->data(Qt::DisplayRole).toInt()
            <= sliderWidget->item(0,0)->data(Qt::DisplayRole).toInt(); i++){
            if(keyFrameWidget->item(0,i)->data(Qt::DisplayRole).toInt()
                    == sliderWidget->item(0, 0)->data(Qt::DisplayRole).toInt()){
                for(int j=1; j<keyFrameWidget->rowCount(); j++){
                    keyFrameWidget->item(j,i)->setData(Qt::DisplayRole,sliderVector.at(j-1)->value());
                }
                tag = true;
                break;
            }
        }
        if(!tag){
            keyFrameWidget->insertColumn(i);
            QTableWidgetItem *tempItem = new QTableWidgetItem;
            tempItem->setData(Qt::DisplayRole,sliderWidget->item(0,0)->data(Qt::DisplayRole).toInt());
            keyFrameWidget->setCurrentCell(0, -1); // 使 keyFrameWidget.currentCell == nullptr,否则下一步会促发信号槽引起错误
            keyFrameWidget->setItem(0,i,tempItem);
            for(int j=1; j<keyFrameWidget->rowCount(); j++){
                tempItem = new QTableWidgetItem;
                tempItem->setData(Qt::DisplayRole,sliderVector.at(j-1)->value());
                keyFrameWidget->setItem(j,i, tempItem);
            }
            keyFrameWidget->setCurrentCell(0, i);
        }
    }
    dataChanged();
}

void KeyFrameWidget::onSliderWidgetchanged()
{
    if(Qt::Vertical == orientation){
        int i;
        for(i=0; i<keyFrameWidget->rowCount(); i++){
            if(sliderWidget->item(0,0)->data(Qt::DisplayRole).toInt()
                    ==keyFrameWidget->item(i,0)->data(Qt::DisplayRole).toInt()){
                keyFrameWidget->setCurrentItem(keyFrameWidget->item(i,0));
                emit keyFrameWidget->currentCellChanged(i,0,0,0);
                break;
            }
        }
        if(i == keyFrameWidget->rowCount()){
            keyFrameWidget->setCurrentItem(NULL);
        }
    }else if(Qt::Horizontal == orientation){
        int i;
        for(i=0; i<keyFrameWidget->columnCount(); i++){
            if(sliderWidget->item(0,0)->data(Qt::DisplayRole).toInt()
                    ==keyFrameWidget->item(0,i)->data(Qt::DisplayRole).toInt()){
                keyFrameWidget->setCurrentItem(keyFrameWidget->item(0,i));
                emit keyFrameWidget->currentCellChanged(0,i,0,0);
                break;
            }
        }
        if(i == keyFrameWidget->columnCount()){
            keyFrameWidget->setCurrentItem(NULL);
        }
    }
    dataChanged();
}

void KeyFrameWidget::onCustomContextRequest_1(QPoint pos)
{
    actionMenu->clear();
    actionMenu->addAction(removeAction);
    actionMenu->addAction(gotoAction);
    actionMenu->exec(QCursor::pos());
}

void KeyFrameWidget::onCustomContextRequest_2(QPoint pos)
{
    if(paramMap["debugModel"].toInt() == MODEL_FILE){
        return;
    }
    actionMenu->clear();
    actionMenu->addAction(getdownPos);
    actionMenu->exec(QCursor::pos());
}

void KeyFrameWidget::removeFrameSlot()
{
    if(keyFrameWidget->currentItem() == nullptr){
        return;
    }else{
        if(Qt::Vertical == orientation){
            keyFrameWidget->removeRow(keyFrameWidget->currentIndex().row());
        }else if(Qt::Horizontal == orientation){
            keyFrameWidget->removeColumn(keyFrameWidget->currentIndex().column());
        }
    }
    dataChanged();
}

void KeyFrameWidget::gotoFrameActionSlot()
{
    if(servoType == Digital_UBT12HB){
        QVector<ServoInfo> servoInfos;
        int j = 0;
        for(int i : zhServoInfoMap.keys()){
            servoInfos.push_back(zhServoInfoMap[i]);
            servoInfos[j].currentPos = sliderVector.at(j)->value();
            j++;
        }
        emit gotoFrameSignal(servoInfos);
    }
}

void KeyFrameWidget::ghpInsertSlot(int time)
{
    sliderWidget->item(0,0)->setData(Qt::DisplayRole, time);
    onInsertButtonClicked();
}

void KeyFrameWidget::ghpModifySlot(int id, int time, int newData)
{
    if(Qt::Vertical == orientation){
        int i;
        for(i=0; i<keyFrameWidget->rowCount(); i++){
            if(keyFrameWidget->item(i,0)->data(Qt::DisplayRole).toInt() == time){
                break;
            }
        }
        int j;
        for(j=0; j<keyFrameWidget->columnCount(); j++){
            QString head = keyFrameWidget->horizontalHeaderItem(j)->text().split("-", QString::SkipEmptyParts).at(0);
            if(head.toInt() == id){
                break;
            }
        }
        if(keyFrameWidget->item(i,j) != NULL){
            keyFrameWidget->item(i,j)->setData(Qt::DisplayRole, newData);
        }
    }else if(Qt::Horizontal == orientation){
        int i;
        for(i=0; i<keyFrameWidget->columnCount(); i++){
            if(keyFrameWidget->item(0,i)->data(Qt::DisplayRole).toInt() == time){
                break;
            }
        }
        int j;
        for(j=0; j<keyFrameWidget->rowCount(); j++){
            QString head = keyFrameWidget->verticalHeaderItem(j)->text().split("-", QString::SkipEmptyParts).at(0);
            if(head.toInt() == id){
                break;
            }
        }
        if(keyFrameWidget->item(j,i) != NULL){
            keyFrameWidget->item(j,i)->setData(Qt::DisplayRole, newData);
        }
    }
    emit dataChanged();
}

void KeyFrameWidget::ghpMoveSlot(int oldtime, int newtime)
{
    qDebug()<<"ghp moved" << oldtime << newtime;
    if(Qt::Vertical == orientation){
        int i;
        for(i=0; i<keyFrameWidget->rowCount(); i++){
            if(keyFrameWidget->item(i,0)->data(Qt::DisplayRole).toInt() == oldtime){
                break;
            }
        }
        if(i<keyFrameWidget->rowCount()){
            keyFrameWidget->item(i,0)->setData(Qt::DisplayRole,newtime);
            keyFrameWidget->sortByColumn(0);
        }
    }else if(Qt::Horizontal == orientation){
        int i;
        for(i=0; i<keyFrameWidget->columnCount(); i++){
            if(keyFrameWidget->item(0,i)->data(Qt::DisplayRole).toInt() == oldtime){
                break;
            }
        }
        if(i<keyFrameWidget->columnCount()){
            keyFrameWidget->item(0,i)->setData(Qt::DisplayRole,newtime);
            //sort TODO
        }
    }
    emit dataChanged();
}

void KeyFrameWidget::ghpRemoveSlot(int time)
{
    if(Qt::Vertical == orientation){
        int i;
        for(i=0; i<keyFrameWidget->rowCount(); i++){
            if(keyFrameWidget->item(i,0)->data(Qt::DisplayRole).toInt() == time){
                break;
            }
        }
        if(i<keyFrameWidget->rowCount()){
            keyFrameWidget->removeRow(i);
        }
    }else if(Qt::Horizontal == orientation){
        int i;
        for(i=0; i<keyFrameWidget->columnCount(); i++){
            if(keyFrameWidget->item(0,i)->data(Qt::DisplayRole).toInt() == time){
                break;
            }
        }
        if(i<keyFrameWidget->columnCount()){
            keyFrameWidget->removeColumn(i);
        }
    }
    emit dataChanged();
}

void KeyFrameWidget::ghpSelected(int index)
{
    if(Qt::Vertical == orientation){
        if(index<keyFrameWidget->rowCount() && index >= 0){
            keyFrameWidget->selectRow(index);
        }else{
            //groupWarning("cannot find data on keyFrameTable");
        }
    }else if(Qt::Horizontal == orientation){
        if(index<keyFrameWidget->columnCount() && index >= 0){
            keyFrameWidget->selectColumn(index);
        }else{
            //groupWarning("cannot find data on keyFrameTable");
        }
    }
}

void KeyFrameWidget::ghpRefreshSlot()
{
    dataChanged();
}

void KeyFrameWidget::getDownPosActionSlot()
{
    emit getDownPosSignal();
}

void KeyFrameWidget::setSliderConnectionSlot(uchar type, ServoInfoMap info)
{
    servoType = type;
    zhServoInfoMap = info;
    QString header;
    header.append("time,");
    for(int i: info.keys()){
        header.append(QString::number(i)+"-"+info[i].name+",");
    }
    for(int i=0; i<sliderVector.size(); i++){
        delete sliderVector.at(i);
        delete spinBoxVector.at(i);
    }
    sliderVector.clear();
    spinBoxVector.clear();
    initModelHeader(header, 0);
    setSliderPosSlot(info);
    dataChanged();
}

void KeyFrameWidget::setSliderPosSlot(ServoInfoMap info)
{
    if(info.size()!= sliderVector.size()){
        groupWarning(QObject::tr("servo number changed !"));
        return;
    }
    QList<int> ids(info.keys());
    for(int i=0; i<ids.size(); i++){
        sliderVector.at(i)->setValue(info[ids.at(i)].currentPos);
        spinBoxVector.at(i)->setValue(sliderVector.at(i)->value());
    }
}

void KeyFrameWidget::dataChanged()
{
    ACInfoMap keyinfo;
    ActionChannelInfo channelTemp;
    if(Qt::Vertical == orientation){
        for(int i=1; i<keyFrameWidget->columnCount(); i++){
            channelTemp.clear();
            QString header = keyFrameWidget->horizontalHeaderItem(i)->text();
            QList<QString> headerIN(header.split("-", QString::SkipEmptyParts));
            channelTemp.id = headerIN.at(0).toInt();
            if(headerIN.size()<2){
                channelTemp.name = "Servo";
            }else{
                channelTemp.name = headerIN.at(1);
            }
            for(int j=0; j<keyFrameWidget->rowCount(); j++){
                channelTemp.keyPoint.push_back(
                            Point(keyFrameWidget->item(j,0)->data(Qt::DisplayRole).toInt(),
                            keyFrameWidget->item(j,i)->data(Qt::DisplayRole).toInt()));
            }
            keyinfo.insert(channelTemp.id, channelTemp);
        }
    }else if(Qt::Horizontal == orientation){
        for(int i=1; i<keyFrameWidget->rowCount(); i++){
            channelTemp.clear();
            QString header = keyFrameWidget->verticalHeaderItem(i)->text();
            QList<QString> headerIN(header.split("-", QString::SkipEmptyParts));
            channelTemp.id = headerIN.at(0).toInt();
            if(headerIN.size()<2){
                channelTemp.name = "Servo";
            }else{
                channelTemp.name = headerIN.at(1);
            }
            for(int j=0; j<keyFrameWidget->columnCount(); j++){
                channelTemp.keyPoint.push_back(
                            Point(keyFrameWidget->item(0,j)->data(Qt::DisplayRole).toInt(),
                            keyFrameWidget->item(i,j)->data(Qt::DisplayRole).toInt()));
            }
            keyinfo.insert(channelTemp.id, channelTemp);
        }
    }
    emit dataChangedSignal(keyinfo);
}

void KeyFrameWidget::initSence()
{
    sliderWidget = new QTableWidget(this);
    sliderWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    sliderWidget->horizontalHeader()->setDefaultSectionSize(40);
    sliderWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    keyFrameWidget = new QTableWidget(this);
    keyFrameWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    keyFrameWidget->horizontalHeader()->setDefaultSectionSize(40);
    keyFrameWidget->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    keyFrameWidget->verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    insertButton = new QPushButton(this);
    insertButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //insertButton->setIcon(QIcon(":/right"));
    insertButton->setFocusPolicy(Qt::NoFocus);
    insertButton->setStyleSheet("background: #E8E8E8; border: none; padding: 0px;");

    removeAction = new QAction("Delete", this);
    gotoAction = new QAction("Servo goto", this);
    getdownPos = new QAction("Get down", this);
    actionMenu = new QMenu(this);

    if(Qt::Vertical == orientation){
        sliderWidget->setHorizontalScrollBar(keyFrameWidget->horizontalScrollBar());
        sliderWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
        keyFrameWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
        keyFrameWidget->horizontalHeader()->hide();
        insertButton->setIcon(QIcon(tr(":/MDI-Image/downward.png")));
        insertButton->setFixedHeight(17);
        connect(keyFrameWidget->verticalHeader(),&QHeaderView::customContextMenuRequested,
                this,&KeyFrameWidget::onCustomContextRequest_1);
        mainSplitter = new QSplitter(Qt::Vertical,this);
    }else if(Qt::Horizontal == orientation){
        sliderWidget->setVerticalScrollBar(keyFrameWidget->verticalScrollBar());
        sliderWidget->verticalHeader()->setDefaultSectionSize(25);
        sliderWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
        keyFrameWidget->verticalHeader()->setDefaultSectionSize(25);
        keyFrameWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
        keyFrameWidget->verticalHeader()->hide();
        insertButton->setIcon(QIcon(tr(":/MDI-Image/rightward.png")));
        insertButton->setFixedWidth(17);
        connect(keyFrameWidget->horizontalHeader(),&QHeaderView::customContextMenuRequested,
                this,&KeyFrameWidget::onCustomContextRequest_1);
        mainSplitter = new QSplitter(Qt::Horizontal,this);
    }
    mainSplitter->addWidget(sliderWidget);
    mainSplitter->addWidget(insertButton);
    mainSplitter->addWidget(keyFrameWidget);
    setWidget(mainSplitter);

    connect(keyFrameWidget,&QTableWidget::currentCellChanged,this,&KeyFrameWidget::onCurrentCellChanged);
    connect(keyFrameWidget,&QTableWidget::itemChanged,this,&KeyFrameWidget::onkeyFrameDataChanged);
    connect(sliderWidget,&QTableWidget::customContextMenuRequested,this,&KeyFrameWidget::onCustomContextRequest_2);
    connect(sliderWidget,&QTableWidget::itemChanged,this,&KeyFrameWidget::onSliderWidgetchanged);
    connect(getdownPos,&QAction::triggered,this,&KeyFrameWidget::getDownPosActionSlot);
    connect(insertButton,&QPushButton::clicked,this,&KeyFrameWidget::onInsertButtonClicked);
    connect(removeAction, &QAction::triggered, this, &KeyFrameWidget::removeFrameSlot);
    connect(gotoAction, &QAction::triggered, this, &KeyFrameWidget::gotoFrameActionSlot);

}

void KeyFrameWidget::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}
