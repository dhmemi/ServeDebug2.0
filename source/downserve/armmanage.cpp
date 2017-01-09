#include "armmanage.h"
#include "source/globaldefine/upDefine.h"
#include "ui_armmanage.h"

#include <QDockWidget>
#include <QTabWidget>
#include <QTableWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QPushButton>
#include <QLayout>
#include <QString>
#include <QStringList>
#include <QMessageBox>
#include <QDir>
#include <QtDebug>
#include <QFileInfo>
#include <QSplitter>
#include <QModelIndex>
#include <QFileDialog>
#include <QInputDialog>
#include <QIcon>
#pragma execution_character_set("utf-8")

ArmManage::ArmManage(QVariantMap param, QWidget *parent) :
    QDockWidget(parent), paramMap(param), ui(new Ui::ArmManage)
{
    ui->setupUi(this);
    initSence();
    groupSignals();

    upRefreshActionSlot();
    upRefreshSequenceSlot();
    downRefreshActionSlot();
    downRefreshSequenceSlot();

    setWindowTitle(QObject::tr("文件管理"));
}

ArmManage::~ArmManage()
{
    delete ui;
}

void ArmManage::setParam(QVariantMap param)
{
    paramMap = param;
}

void ArmManage::upRefreshActionSlot()
{
    QStringList temp = QDir(paramMap["csvPath"].toString()).entryList(QStringList(QString("*.csv")));
    upActionTable->clearContents();
    if(0<temp.size()){
        upActionTable->setRowCount(temp.size());
        for(int i=0; i<temp.size(); i++){
            upActionTable->setItem(i,0,new QTableWidgetItem(temp.at(i)));
        }
    }
}

void ArmManage::upSaveActionSlot()
{
    QString fileName(QInputDialog::getText(this, tr("Save file"), tr("file name:")));
    if(fileName.size()<=0){
        groupWarning(QObject::tr("file name is empty!"));
        return;
    }else if(fileName.contains(' ')){
        groupWarning(QObject::tr("文件名不能包含空格!"));
        return;
    }
    if(fileName.endsWith(".csv")){
        fileName.remove(fileName.size()-4, 4);
    }
    QString abcsvName = paramMap["csvPath"].toString()+"/"+fileName+".csv";
    QString abbinName = paramMap["binPath"].toString()+"/"+fileName+".bin";
    emit upSaveActionSignal(abcsvName, abbinName);
    upRefreshActionSlot();
}

void ArmManage::upRenameActionSlot()
{
    if(!upActionTable->currentItem()){
        return;
    }
    QString oldName = upActionTable->item(upActionTable->currentRow(), 0)->text();
    if(oldName == upActiveAction){
        groupWarning(QObject::tr("file is editing!"));
        return;
    }else{
        oldName.remove(oldName.size()-4, 4);
    }
    bool ok;
    QString newName(QInputDialog::getText(this, tr("Change file name"),
                                            tr("New file name"), QLineEdit::Normal,
                                            oldName, &ok));
    if(ok && newName.size()>0){
        if(newName.endsWith(".csv")){
            newName.remove(newName.size()-4, 4);
        }
        bool csvRenamed = QFile::rename(paramMap["csvPath"].toString()+"/"+oldName+".csv",
                                        paramMap["csvPath"].toString()+"/"+newName+".csv");
        bool binRenamed = QFile::rename(paramMap["binPath"].toString()+"/"+oldName+".bin",
                                        paramMap["binPath"].toString()+"/"+newName+".bin");
        if(csvRenamed && binRenamed){
            upRefreshActionSlot();
        }else{
            groupWarning(QObject::tr("Rename failed!"));
        }
    }else{
        groupWarning(QObject::tr("无效文件名!"));
    }
}

void ArmManage::upCoverActionSlot()
{
    if(!upActionTable->currentItem()){
        return;
    }
    QString fileName = upActionTable->item(upActionTable->currentRow(), 0)->text();
    QMessageBox msgBox(this);
    msgBox.setText(tr("Are you sure to cover \"") + fileName + tr("\" ?"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    fileName.remove(fileName.size()-4, 4);
    QString abcsvName = paramMap["csvPath"].toString()+"/"+fileName+".csv";
    QString abbinName = paramMap["binPath"].toString()+"/"+fileName+".bin";
    switch(msgBox.exec())
    {
    case QMessageBox::Ok:
        emit upSaveActionSignal(abcsvName, abbinName);
        upRefreshActionSlot();
        break;
    case QMessageBox::Cancel:
        break;
    }
}

void ArmManage::upDeleteActionSlot()
{
    if(!upActionTable->currentItem()){
        return;
    }
    QString fileName = upActionTable->item(upActionTable->currentRow(), 0)->text();
    if(fileName == upActiveAction){
        groupWarning(QObject::tr("file is editing!"));
        return;
    }
    QMessageBox msgBox(this);
    msgBox.setText(tr("Are you sure to Delete \"") + fileName + tr("\" ?"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    fileName.remove(fileName.size()-4, 4);
    QDir csvdir(paramMap["csvPath"].toString());
    QDir bindir(paramMap["binPath"].toString());
    switch(msgBox.exec())
    {
    case QMessageBox::Ok:
        if(csvdir.remove(fileName+".csv") && bindir.remove(fileName+".bin")){
            upActionTable->removeRow(upActionTable->currentRow());
        }else{
            groupWarning("cannot delete file \""+fileName+".csv\" !");
        }
        break;
    case QMessageBox::Cancel:
        break;
    }
}

void ArmManage::downloadActionSlot()
{
    if(!upActionTable->currentItem()){
        return;
    }
    QString fileName = upActionTable->item(upActionTable->currentRow(), 0)->text();
    fileName.remove(fileName.size()-4, 4);
    QString abbinName = paramMap["binPath"].toString()+"/"+fileName+".bin";
    emit downloadActionSignal(abbinName);
}

void ArmManage::upaddActionToSequenceSlot()
{
    if(!(upActionTable->currentItem()&&upSequenceTree->currentItem())){
        return;
    }
    QString fileName = upActionTable->item(upActionTable->currentRow(), 0)->text();
    if(fileName == upActiveAction){
        groupWarning(QObject::tr("file is editing!"));
        return;
    }
    if(upSequenceTree->topLevelItemCount()<=0){
        return;
    }
    if(NULL == upSequenceTree->currentItem()){
        return;
    }
    QTreeWidgetItem* cur = upSequenceTree->currentItem();
    if(NULL != cur->parent()){
        cur = cur->parent();
    }
    fileName.remove(fileName.size()-4, 4);
    cur->addChild(new QTreeWidgetItem(cur, QStringList(QString(fileName+".bin"))));
    //upSaveSequenceSlot();
}

void ArmManage::upRefreshSequenceSlot()
{
    QStringList temp = QDir(paramMap["odrPath"].toString()).entryList(QStringList(QString("*.odr")));
    upSequenceTree->clear();
    if(0<temp.size()){
        QList<QTreeWidgetItem*> items;
        for(int i=0; i<temp.size();i++){
            new QTreeWidgetItem(upSequenceTree, QStringList(temp.at(i)));
        }
    }
}

void ArmManage::upCreateSequenceSlot()
{
    bool ok;
    QString newName(QInputDialog::getText(this, tr("Create sequence"),
                                            tr("Sequence name"), QLineEdit::Normal,
                                            QString(), &ok) + ".odr");
    if(ok && newName.size()>4){
        QFile file(paramMap["odrPath"].toString()+"/"+newName);
        if(file.exists()){
            groupWarning("\""+newName+"\"have existed !");
        }else{
            file.open(QFile::WriteOnly);
            file.write("");
            file.close();
        }
        new QTreeWidgetItem(upSequenceTree, QStringList(newName));
        upSaveSequenceSlot();
    }
}

void ArmManage::upSaveSequenceSlot()
{
    if(!upSequenceTree->currentItem()){
        return;
    }
    QTreeWidgetItem* cur = upSequenceTree->currentItem();
    if(NULL != cur->parent()){
        cur = cur->parent();
    }
    QString fileName = cur->text(0);
    QString fileContent;
    for(int i=0; i<cur->childCount(); i++){
        fileContent.append(cur->child(i)->text(0));
        fileContent.append("||");
    }
    QFile file(paramMap["odrPath"].toString()+"/"+fileName);
    if(file.open(QIODevice::WriteOnly)){
        file.write(fileContent.toLatin1().data());
        file.close();
    }else{
        groupWarning("cannot op file \""+fileName+"\" !");
    }
}

void ArmManage::upRenameSequenceSlot()
{
    if(!upSequenceTree->currentItem()){
        return;
    }
    QTreeWidgetItem* cur = upSequenceTree->currentItem();
    if(NULL != cur->parent()){
        cur = cur->parent();
    }
    QString oldName = cur->text(0);
    bool ok;
    QString newName(QInputDialog::getText(this, tr("Change file name"),
                                            tr("New file name"), QLineEdit::Normal,
                                            oldName, &ok) + ".odr");
    if(ok && newName.size() > 4){
        bool csvRenamed = QFile::rename(paramMap["odrPath"].toString()+"/"+oldName,
                                        paramMap["odrPath"].toString()+"/"+newName);
        if(csvRenamed){
            cur->setText(0, newName);
        }else{
            groupWarning(QObject::tr("Rename failed!"));
        }
    }else{
        groupWarning(QObject::tr("无效文件名!"));
    }
}

void ArmManage::upDeleteSequenceSlot()
{
    if(!upSequenceTree->currentItem()){
        return;
    }
    QTreeWidgetItem* cur = upSequenceTree->currentItem();
    QString fileName = cur->text(0);
    if(NULL == cur->parent()){
        QMessageBox msgBox(this);
        msgBox.setText(tr("Sure to Delete \"") + fileName + tr("\" ?"));
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        QDir odrdir(paramMap["odrPath"].toString());
        switch(msgBox.exec())
        {
        case QMessageBox::Ok:
            if(odrdir.remove(fileName)){
                QTreeWidgetItem* zeroChild;
                while(cur->childCount()>0){
                    zeroChild = cur->child(0);
                    cur->removeChild(zeroChild);
                    delete zeroChild;
                }
                for(int i=0; i<upSequenceTree->topLevelItemCount(); i++){
                    if(cur == upSequenceTree->topLevelItem(i)){
                        upSequenceTree->takeTopLevelItem(i);
                        delete cur;
                        break;
                    }
                }
            }else{
                groupWarning("cannot delete file \""+fileName+"\" !");
            }
            break;
        case QMessageBox::Cancel:
            break;
        }
    }else{
        cur->parent()->removeChild(cur);
        delete cur;
    }
}

void ArmManage::upMoveUpActionSlot()
{
    if(!upSequenceTree->currentItem()){
        return;
    }
    QTreeWidgetItem* cur = upSequenceTree->currentItem();
    QTreeWidgetItem* parent = cur->parent();
    if(NULL == parent){
        return;
    }else if(parent->indexOfChild(cur)<=0 ||
             parent->indexOfChild(cur)>parent->childCount()-1){
        return;
    }else{
        QString temp = parent->child(parent->indexOfChild(cur)-1)->text(0);
        parent->child(parent->indexOfChild(cur)-1)->setText(0, cur->text(0));
        cur->setText(0, temp);
    }
    upSequenceTree->setCurrentItem(parent->child(parent->indexOfChild(cur)-1));
    //upSaveSequenceSlot();
}

void ArmManage::upMoveDownActionSlot()
{
    if(!upSequenceTree->currentItem()){
        return;
    }
    QTreeWidgetItem* cur = upSequenceTree->currentItem();
    QTreeWidgetItem* parent = cur->parent();
    if(NULL == parent){
        return;
    }else if(parent->indexOfChild(cur)<0 ||
             parent->indexOfChild(cur)>=parent->childCount()-1){
        return;
    }else{
        QString temp = parent->child(parent->indexOfChild(cur)+1)->text(0);
        parent->child(parent->indexOfChild(cur)+1)->setText(0, cur->text(0));
        cur->setText(0, temp);
    }
    upSequenceTree->setCurrentItem(parent->child(parent->indexOfChild(cur)+1));
    //upSaveSequenceSlot();
}

void ArmManage::downloadSequenceSlot()
{
    if(!upSequenceTree->currentItem()){
        return;
    }
    QTreeWidgetItem* cur = upSequenceTree->currentItem();
    if(NULL != cur->parent()){
        cur = cur->parent();
    }
    if(cur->childCount() != 0){
        upSaveSequenceSlot();
    }
    QFile sequenceFile(paramMap["odrPath"].toString()+"/"+cur->text(0));
    if(!sequenceFile.open(QIODevice::ReadOnly)){
        groupWarning(QObject::tr("Cannot open file!"));
    }else{
        QString fileContent = sequenceFile.readAll();
        if(fileContent.isEmpty()){
            groupWarning(QObject::tr("file is empty!"));
        }else{
            QStringList actions(fileContent.split("||", QString::SkipEmptyParts));
            for(int i=0; i<actions.size(); i++){
                emit downloadActionSignal(paramMap["binPath"].toString()+"/"+actions.at(i));
            }
            emit downloadSequenceSignal(cur->text(0), actions);
        }
        sequenceFile.close();
    }
}

void ArmManage::downReplyActionSlot(QStringList actions)
{
    downActionTable->clearContents();
    if(0<actions.size()){
        downActionTable->setRowCount(actions.size());
        for(int i=0; i<actions.size(); i++){
            downActionTable->setItem(i,0,new QTableWidgetItem(actions.at(i)));
        }
    }
}

void ArmManage::downRefreshActionSlot()
{
    downActionTable->clear();
    emit downRefreshActionSignal();
}

void ArmManage::downRenameActionSlot()
{
    if(!downActionTable->currentItem()){
        return;
    }
    QString oldName = downActionTable->item(downActionTable->currentRow(), 0)->text();
    bool ok;
    QString newName(QInputDialog::getText(this, tr("Change file name"),
                                            tr("New file name"), QLineEdit::Normal,
                                            oldName, &ok));
    if(!newName.endsWith(".bin")){
        newName.append(".bin");
    }
    if(ok && newName.size() > 4 && newName != oldName){
        emit downRenameActionSignal(oldName, newName);
        emit downRefreshActionSignal();
    }else{
        groupWarning(QObject::tr("无效文件名!"));
    }
}

void ArmManage::downExecuteActionSlot()
{
    if(!downActionTable->currentItem()){
        return;
    }
    QString fileName = downActionTable->item(downActionTable->currentRow(), 0)->text();
    emit downExecuteActionSignal(fileName);
}

void ArmManage::downDeleteActionSlot()
{
    if(!downActionTable->currentItem()){
        return;
    }
    QString fileName = downActionTable->item(downActionTable->currentRow(), 0)->text();
    QMessageBox msgBox(this);
    msgBox.setText(tr("Sure to Delete \"") + fileName + tr("\" ?"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    switch (msgBox.exec()) {
    case QMessageBox::Ok:
        emit downDeleteActionSignal(fileName);
        emit downRefreshActionSignal();
        break;
    default:
        break;
    }
}

void ArmManage::downaddActionToSequenceSlot()
{
    if(!(downActionTable->currentItem()&&downSequenceTree->currentItem())){
        return;
    }
    QString fileName = downActionTable->item(downActionTable->currentRow(), 0)->text();
    QTreeWidgetItem* cur = downSequenceTree->currentItem();
    if(NULL != cur->parent()){
        cur = cur->parent();
    }
    cur->addChild(new QTreeWidgetItem(cur, QStringList(QString(fileName))));
}

void ArmManage::downReplySequenceSlot(QStringList sequence)
{
    downSequenceTree->clear();
    for(int i=0; i<sequence.size(); i++){
        new QTreeWidgetItem(downSequenceTree, QStringList(sequence.at(i)));
    }
}

void ArmManage::downUploadSequenceSlot(QString sequence, QStringList actions)
{
    QTreeWidgetItem* target = NULL;
    for(int i=0; i<downSequenceTree->topLevelItemCount(); i++){
        if(sequence == downSequenceTree->topLevelItem(i)->text(0)){
            target = downSequenceTree->topLevelItem(i);
            break;
        }
    }
    if(target == NULL){
        target = new QTreeWidgetItem(downSequenceTree, QStringList(sequence));
    }
    QTreeWidgetItem* child;
    while(target->childCount()>0){
        child = target->child(0);
        target->removeChild(child);
        delete child;
    }
    for(int i=0; i<actions.size(); i++){
        new QTreeWidgetItem(target, QStringList(actions.at(i)));
    }
}

void ArmManage::downRefreshSequenceSlot()
{
    downSequenceTree->clear();
    emit downRefreshSequenceSignal();
}

void ArmManage::downCreateSequenceSlot()
{
    bool ok;
    QString newName(QInputDialog::getText(this, tr("Create sequence"),
                                            tr("Sequence name"), QLineEdit::Normal,
                                            QString(), &ok));
    if(!newName.endsWith(".odr")){
        newName.append(".odr");
    }
    if(ok && newName.size()>4){
        new QTreeWidgetItem(downSequenceTree, QStringList(newName));
    }
}

void ArmManage::downSaveSequenceSlot()
{
    if(!downSequenceTree->currentItem()){
        return;
    }
    QTreeWidgetItem* cur = downSequenceTree->currentItem();
    if(NULL != cur->parent()){
        cur = cur->parent();
    }
    QString fileName = cur->text(0);
    QStringList fileContent;
    for(int i=0; i<cur->childCount(); i++){
        fileContent.push_back(cur->child(i)->text(0));
    }
    emit downloadSequenceSignal(fileName, fileContent);
    emit downRefreshSequenceSignal();
}

void ArmManage::downExecuteSequenceSlot()
{
    if(!downSequenceTree->currentItem()){
        return;
    }
    QTreeWidgetItem* cur = downSequenceTree->currentItem();
    if(NULL != cur->parent()){
        cur = cur->parent();
    }
    QString sequence = cur->text(0);
    emit downExecuteSequenceSignal(sequence);
}

void ArmManage::downRenameSequenceSlot()
{
    if(!downSequenceTree->currentItem()){
        return;
    }
    QTreeWidgetItem* cur = downSequenceTree->currentItem();
    if(NULL != cur->parent()){
        cur = cur->parent();
    }
    QString oldName(cur->text(0));
    bool ok;
    QString newName(QInputDialog::getText(this, tr("Create sequence"),
                                            tr("Sequence name"), QLineEdit::Normal,
                                            QString(), &ok));
    if(!newName.endsWith(".odr")){
        newName.append(".odr");
    }
    if(ok && newName.size()>4){
        emit downRenameSequenceSignal(oldName, newName);
        emit downRefreshSequenceSignal();
    }
}

void ArmManage::downDeleteSequenceSlot()
{
    if(!downSequenceTree->currentItem()){
        return;
    }
    QTreeWidgetItem* cur = downSequenceTree->currentItem();
    if(NULL == cur->parent()){
        emit downDeleteSequenceSignal(cur->text(0));
        emit downRefreshSequenceSignal();
    }else{
        cur->parent()->removeChild(cur);
        delete cur;
    }


}

void ArmManage::downMoveUpActionSlot()
{
    if(!downSequenceTree->currentItem()){
        return;
    }
    QTreeWidgetItem* cur = downSequenceTree->currentItem();
    QTreeWidgetItem* parent = cur->parent();
    if(NULL == parent){
        return;
    }else if(parent->indexOfChild(cur)<=0 ||
             parent->indexOfChild(cur)>parent->childCount()-1){
        return;
    }else{
        QString temp = parent->child(parent->indexOfChild(cur)-1)->text(0);
        parent->child(parent->indexOfChild(cur)-1)->setText(0, cur->text(0));
        cur->setText(0, temp);
    }
    downSequenceTree->setCurrentItem( parent->child(parent->indexOfChild(cur)-1));
}

void ArmManage::downMoveDownActionSlot()
{
    if(!downSequenceTree->currentItem()){
        return;
    }
    QTreeWidgetItem* cur = downSequenceTree->currentItem();
    QTreeWidgetItem* parent = cur->parent();
    if(NULL == parent){
        return;
    }else if(parent->indexOfChild(cur)<0 ||
             parent->indexOfChild(cur)>=parent->childCount()-1){
        return;
    }else{
        QString temp = parent->child(parent->indexOfChild(cur)+1)->text(0);
        parent->child(parent->indexOfChild(cur)+1)->setText(0, cur->text(0));
        cur->setText(0, temp);
    }
    downSequenceTree->setCurrentItem( parent->child(parent->indexOfChild(cur)+1));
    //upSaveSequenceSlot();
}

void ArmManage::upOnActionDoubleClicked(int row, int col)
{
    QString action(upActionTable->item(row, col)->text());
    emit upOpenActionSiganl(paramMap["csvPath"].toString()+"/"+action);
}

void ArmManage::upOnSequenceDoubleClicked(QTreeWidgetItem *cur, int col)
{
    QString file = cur->text(col);
    if(NULL != cur->parent()){
        emit upOpenActionSiganl(paramMap["csvPath"].toString()+"/"+file);
    }else if(0 == cur->childCount()){
        QFile sequenceFile(paramMap["odrPath"].toString()+"/"+file);
        if(!sequenceFile.open(QIODevice::ReadOnly)){
            groupWarning(QObject::tr("Cannot open file!"));
        }else{
            QString fileContent = sequenceFile.readAll();
            if(fileContent.isEmpty()){
                groupWarning(QObject::tr("file is empty!"));
            }else{
                QStringList actions(fileContent.split("||", QString::SkipEmptyParts));
                for(int i=0; i<actions.size(); i++){
                    new QTreeWidgetItem(cur, QStringList(actions.at(i)));
                }
            }
            sequenceFile.close();
        }
    }else{
        cur->setExpanded(cur->isExpanded());
    }
}

void ArmManage::downOnSequenceDoubleClicked(QTreeWidgetItem *cur, int col)
{
    if(NULL != cur->parent()){
        return;
    }else if(0 == cur->childCount()){
        emit downUploadSequenceSiganl(cur->text(col));
    }else{
        cur->setExpanded(cur->isExpanded());
    }
}

void ArmManage::groupWarning(QString str)
{
     QMessageBox::warning(this,QObject::tr("Wrong"),str);
}

void ArmManage::closeEvent(QCloseEvent *event)
{
    emit closed();
    event->ignore();
}

void ArmManage::initSence()
{
        QGridLayout* upActionLayout = new QGridLayout(this);
        {
            upActionTable = new QTableWidget(this);
            upActionTable->setColumnCount(1);
            upActionTable->setColumnWidth(0, 750);
            upActionTable->setSelectionMode(QAbstractItemView::SingleSelection);
            upActionTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
            upActionTable->setHorizontalHeaderLabels(QStringList(QString("微动作数据")));
            upActionTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

            upRefreshActionButton = new QPushButton("刷新列表",this);
            upSaveActionButton = new QPushButton("保存动作",this);
            upRenameActionButton = new QPushButton("重命名",this);
            upCoverActionButton = new QPushButton("覆盖动作",this);
            upDeleteActionButton = new QPushButton("删除动作",this);
            downloadActionButton = new QPushButton("下载动作",this);
            upActionLayout->addWidget(upActionTable, 0, 0, 10, 3);
            upActionLayout->addWidget(upRefreshActionButton, 0, 3);
            upActionLayout->addWidget(upRenameActionButton, 1, 3);
            upActionLayout->addWidget(upDeleteActionButton, 2, 3);
            upActionLayout->addWidget(upSaveActionButton, 3, 3);
            upActionLayout->addWidget(upCoverActionButton, 4, 3);
            upActionLayout->addWidget(downloadActionButton, 5, 3);
        }
        upActionWidget = new QWidget(this);
        upActionWidget->setLayout(upActionLayout);

            upaddActionToSequenceButton = new QPushButton(this);
            upaddActionToSequenceButton->setIcon(QIcon(":/MDI-Image/down.png"));
            upaddActionToSequenceButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            upaddActionToSequenceButton->setFixedHeight(20);

        QGridLayout* upSequenceLayout = new QGridLayout(this);
        {
            upSequenceTree = new QTreeWidget(this);
            upSequenceTree->setHeaderLabels(QStringList(QString("序列文件")));
            upSequenceTree->setSortingEnabled(false);

            upRefreshSequenceButton = new QPushButton("刷新列表",this);
            upCreateSequenceButton = new QPushButton("创建序列",this);
            upSaveSequenceButton = new QPushButton("保存序列",this);
            upRenameSequenceButton = new QPushButton("重命名",this);
            upDeleteSequenceButton = new QPushButton("删除项目",this);
            upMoveUpActionButton = new QPushButton(this);
            upMoveUpActionButton->setIcon(QIcon(":/MDI-Image/upward.png"));
            upMoveDownActionButton = new QPushButton(this);
            upMoveDownActionButton->setIcon(QIcon(":/MDI-Image/downward.png"));
            downloadSequenceButton = new QPushButton(QObject::tr("下载序列"),this);
            upSequenceLayout->addWidget(upSequenceTree, 12, 0, 9, 3);
            upSequenceLayout->addWidget(upRefreshSequenceButton, 12, 3);
            upSequenceLayout->addWidget(upCreateSequenceButton, 13, 3);
            upSequenceLayout->addWidget(upMoveUpActionButton, 14, 3);
            upSequenceLayout->addWidget(upMoveDownActionButton, 15, 3);
            upSequenceLayout->addWidget(upSaveSequenceButton, 16, 3);
            upSequenceLayout->addWidget(upRenameSequenceButton, 17, 3);
            upSequenceLayout->addWidget(upDeleteSequenceButton, 18, 3);
            upSequenceLayout->addWidget(downloadSequenceButton, 19, 3);
        }
        upSequenceWidget = new QWidget(this);
        upSequenceWidget->setLayout(upSequenceLayout);

        upSplitter = new QSplitter(Qt::Vertical, this);
        upSplitter->addWidget(upActionWidget);
        upSplitter->addWidget(upaddActionToSequenceButton);
        upSplitter->addWidget(upSequenceWidget);

        QGridLayout* downActionLayout = new QGridLayout(this);
        {
            downActionTable = new QTableWidget(this);
            downActionTable->setColumnCount(1);
            downActionTable->setColumnWidth(0, 750);
            downActionTable->setSelectionMode(QAbstractItemView::SingleSelection);
            downActionTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
            downActionTable->setHorizontalHeaderLabels(QStringList(tr("微动作数据")));
            downActionTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

            downRefreshActionButton = new QPushButton("刷新列表",this);
            downRenameActionButton = new QPushButton("重命名",this);
            downExecuteActionButton = new QPushButton("执行动作",this);
            downDeleteActionButton = new QPushButton("删除动作",this);
            downActionLayout->addWidget(downActionTable, 0, 0, 11, 3);
            downActionLayout->addWidget(downRefreshActionButton, 0, 3);
            downActionLayout->addWidget(downRenameActionButton, 1, 3);
            downActionLayout->addWidget(downDeleteActionButton, 2, 3);
            downActionLayout->addWidget(downExecuteActionButton, 3, 3);
        }
        downActionWidget = new QWidget(this);
        downActionWidget->setLayout(downActionLayout);

            downaddActionToSequenceButton = new QPushButton(this);
            downaddActionToSequenceButton->setIcon(QIcon(":/MDI-Image/down.png"));
            downaddActionToSequenceButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            downaddActionToSequenceButton->setFixedHeight(20);

        QGridLayout* downSequenceLayout = new QGridLayout(this);
        {
            downSequenceTree = new QTreeWidget(this);
            downSequenceTree->setHeaderLabels(QStringList(QString("序列文件")));
            downSequenceTree->setSortingEnabled(false);

            downRefreshSequenceButton = new QPushButton("刷新列表",this);
            downCreateSequenceButton = new QPushButton("创建序列",this);
            downSaveSequenceButton = new QPushButton("保存序列",this);
            downExecuteSequenceButton = new QPushButton("执行序列",this);
            downRenameSequenceButton = new QPushButton("重命名",this);
            downDeleteSequenceButton = new QPushButton("删除项目",this);
            downMoveUpActionButton = new QPushButton(this);
            downMoveUpActionButton->setIcon(QIcon(":/MDI-Image/upward.png"));
            downMoveDownActionButton = new QPushButton(this);
            downMoveDownActionButton->setIcon(QIcon(":/MDI-Image/downward.png"));
            downSequenceLayout->addWidget(downSequenceTree, 12, 0, 9, 3);
            downSequenceLayout->addWidget(downRefreshSequenceButton, 12, 3);
            downSequenceLayout->addWidget(downCreateSequenceButton, 13, 3);
            downSequenceLayout->addWidget(downMoveUpActionButton, 14, 3);
            downSequenceLayout->addWidget(downMoveDownActionButton, 15, 3);
            downSequenceLayout->addWidget(downSaveSequenceButton, 16, 3);
            downSequenceLayout->addWidget(downRenameSequenceButton, 17, 3);
            downSequenceLayout->addWidget(downDeleteSequenceButton, 18, 3);
            downSequenceLayout->addWidget(downExecuteSequenceButton, 19, 3);
        }
        downSequenceWidget = new QWidget(this);
        downSequenceWidget->setLayout(downSequenceLayout);

        downSplitter = new QSplitter(Qt::Vertical, this);
        downSplitter->addWidget(downActionWidget);
        downSplitter->addWidget(downaddActionToSequenceButton);
        downSplitter->addWidget(downSequenceWidget);

    mainTabWidget = new QTabWidget(this);
    mainTabWidget->addTab(upSplitter, "上位机数据管理");
    mainTabWidget->addTab(downSplitter, "下位机数据管理");
    setWidget(mainTabWidget);
}

void ArmManage::groupSignals()
{
    connect(upRefreshActionButton, &QPushButton::clicked, this, &ArmManage::upRefreshActionSlot);
    connect(upSaveActionButton, &QPushButton::clicked, this, &ArmManage::upSaveActionSlot);
    connect(upRenameActionButton, &QPushButton::clicked, this, &ArmManage::upRenameActionSlot);
    connect(upCoverActionButton, &QPushButton::clicked, this, &ArmManage::upCoverActionSlot);
    connect(upDeleteActionButton, &QPushButton::clicked, this, &ArmManage::upDeleteActionSlot);
    connect(downloadActionButton, &QPushButton::clicked, this, &ArmManage::downloadActionSlot);
    connect(upaddActionToSequenceButton, &QPushButton::clicked, this, &ArmManage::upaddActionToSequenceSlot);
    connect(upRefreshSequenceButton, &QPushButton::clicked, this ,&ArmManage::upRefreshSequenceSlot);
    connect(upCreateSequenceButton, &QPushButton::clicked, this, &ArmManage::upCreateSequenceSlot);
    connect(upSaveSequenceButton, &QPushButton::clicked, this, &ArmManage::upSaveSequenceSlot);
    connect(upRenameSequenceButton, &QPushButton::clicked, this, &ArmManage::upRenameSequenceSlot);
    connect(upDeleteSequenceButton, &QPushButton::clicked, this, &ArmManage::upDeleteSequenceSlot);
    connect(upMoveUpActionButton, &QPushButton::clicked, this, &ArmManage::upMoveUpActionSlot);
    connect(upMoveDownActionButton, &QPushButton::clicked, this, &ArmManage::upMoveDownActionSlot);
    connect(downloadSequenceButton, &QPushButton::clicked, this, &ArmManage::downloadSequenceSlot);
    connect(downRefreshActionButton, &QPushButton::clicked, this, &ArmManage::downRefreshActionSlot);
    connect(downRenameActionButton, &QPushButton::clicked, this, &ArmManage::downRenameActionSlot);
    connect(downExecuteActionButton, &QPushButton::clicked, this, &ArmManage::downExecuteActionSlot);
    connect(downDeleteActionButton, &QPushButton::clicked, this, &ArmManage::downDeleteActionSlot);
    connect(downaddActionToSequenceButton, &QPushButton::clicked, this, &ArmManage::downaddActionToSequenceSlot);
    connect(downRefreshSequenceButton, &QPushButton::clicked, this, &ArmManage::downRefreshSequenceSlot);
    connect(downCreateSequenceButton, &QPushButton::clicked, this, &ArmManage::downCreateSequenceSlot);
    connect(downSaveSequenceButton, &QPushButton::clicked, this, &ArmManage::downSaveSequenceSlot);
    connect(downExecuteSequenceButton, &QPushButton::clicked, this, &ArmManage::downExecuteSequenceSlot);
    connect(downRenameSequenceButton, &QPushButton::clicked, this, &ArmManage::downRenameSequenceSlot);
    connect(downDeleteSequenceButton ,&QPushButton::clicked, this, &ArmManage::downDeleteSequenceSlot);
    connect(downMoveUpActionButton, &QPushButton::clicked, this, &ArmManage::downMoveUpActionSlot);
    connect(downMoveDownActionButton, &QPushButton::clicked, this, &ArmManage::downMoveDownActionSlot);
    connect(upActionTable, &QTableWidget::cellDoubleClicked, this, &ArmManage::upOnActionDoubleClicked);
    connect(upSequenceTree, &QTreeWidget::itemDoubleClicked, this, &ArmManage::upOnSequenceDoubleClicked);
    connect(downSequenceTree, &QTreeWidget::itemDoubleClicked, this, &ArmManage::downOnSequenceDoubleClicked);
}
