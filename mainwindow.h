#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "tcomboboxdelegate.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QSqlDatabase DB;//数据库连接
    QSqlTableModel *tabModel;//数据模型
    QItemSelectionModel *selectModel;//选择模型
    QDataWidgetMapper *dataMapper;//数据映射

    TComboBoxDelegate delegateSex;//自定义数据代理 性别
    TComboBoxDelegate delegateDepart;//自定义数据代理 部门

    void openTable();
    void showRecordCount();
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actOpenDB_triggered();

    void do_currentChanged(const QModelIndex &current,const QModelIndex &previous);
    void do_currentRowChanged(const QModelIndex &current,const QModelIndex &previous);

    void on_actAppend_triggered();

    void on_actInsert_triggered();

    void on_actDelete_triggered();

    void on_actSetPic_triggered();

    void on_actClearPic_triggered();

    void on_actAddMoney_triggered();

    void on_actSave_triggered();

    void on_actCancel_triggered();

    void on_comboBoxFiles_currentIndexChanged(int index);

    void on_radioButtonAscend_clicked();

    void on_radioButtonDscend_clicked();

    void on_radioButtonMan_clicked();

    void on_radioButtonWoman_clicked();

    void on_radioButtonMWanALL_clicked();

private:
    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H
