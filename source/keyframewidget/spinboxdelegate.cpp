#include "spinboxdelegate.h"

#include <QSpinBox>

SpinBoxDelegate::SpinBoxDelegate(QObject *parent)
    :QItemDelegate(parent)
{
}

QWidget* SpinBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                       const QModelIndex &index) const
{
    QSpinBox* spinbox= new QSpinBox(parent);
    spinbox->setRange(0, 1023);
    return spinbox;
}

void SpinBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index)
{
    int value = index.model()->data( index, Qt::DisplayRole).toInt();
    static_cast<QSpinBox*>( editor )->setValue( value );
}

void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index)
{
    model->setData(index, static_cast<QSpinBox*>( editor )->value(), Qt::DisplayRole);
}

void SpinBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                           const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
