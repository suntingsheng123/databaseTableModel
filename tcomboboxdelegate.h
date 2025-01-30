#ifndef TCOMBOBOXDELEGATE_H
#define TCOMBOBOXDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>
#include<QtSql>
#include<QDataWidgetMapper>

class TComboBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    TComboBoxDelegate(QObject *parent=nullptr);
private:

    QStringList m_itemlist;//选择列表
    bool m_editable;

    // QAbstractItemDelegate interface
public:
    void setItem(QStringList items,bool editable);
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};


#endif // TComboBoxDelegate_H
