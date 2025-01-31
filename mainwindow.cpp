#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QFileDialog>
#include<QMessageBox>
#include<QSqlQueryModel>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setAlternatingRowColors(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

 void MainWindow::do_currentChanged(const QModelIndex &current,const QModelIndex &previous)
{
     Q_UNUSED(current);
     Q_UNUSED(previous);
    ui->actSave->setEnabled(tabModel->isDirty());
    ui->actCancel->setEnabled(tabModel->isDirty());
 }
void MainWindow::do_currentRowChanged(const QModelIndex &current,const QModelIndex &previous)
{
     Q_UNUSED(previous);
    if(!current.isValid())
     {
         ui->labelPic->clear();
        ui->plainTextEdit->clear();
     }
    dataMapper->setCurrentModelIndex(current);
    bool first=(current.row()==0);
    bool last=(current.row()==qryModel->rowCount()-1);
    ui->actFirst->setEnabled(!first);
    ui->actPrevious->setEnabled(!first);
    ui->actNext->setEnabled(!last);
    ui->actLast->setEnabled(!last);

    int curRecNo=selectModel->currentIndex().row();
    QSqlRecord curRec=qryModel->record(curRecNo);
    int empNo=curRec.value("EmpNo").toInt();


    QSqlQuery query;
    query.prepare("select Memo, Photo from employee where EmpNo= :ID");
    query.bindValue(":ID",empNo);
    query.exec();
    query.first();
    QVariant va=query.value("Photo");
    if(!va.isValid())
        ui->labelPic->clear();
    else
    {
        QByteArray data=va.toByteArray();
        QPixmap pic;
        pic.loadFromData(data);
        ui->labelPic->setPixmap(pic.scaledToWidth(ui->labelPic->size().width() ));
    }
    QVariant va2=query.value("Memo");
    ui->plainTextEdit->setPlainText(va2.toString());

}
void MainWindow::on_actOpenDB_triggered()
{
    QString aFile=QFileDialog::getOpenFileName(this,"选择文件","","QSLite数据库(*.db3)");
    if(aFile.isEmpty())
        return;
    DB=QSqlDatabase::addDatabase("QSQLITE");
    DB.setDatabaseName(aFile);
    if(DB.open())
        selectData();
        //openTable();
    else
        QMessageBox::warning(this,"错误","打开数据库失败");
}

void MainWindow::showRecordCount()
{
    ui->statusbar->showMessage(QString("记录条数:%1").arg(tabModel->rowCount()));
}
void MainWindow::selectData()
{
    QString str="SELECT empNo,Name,Gender,Birthday,Province,department,"
                  "Salary From employee ORDER BY empNo";
    qryModel =new QSqlQueryModel(this);
    qryModel->setQuery(str);
    if(qryModel->lastError().isValid())
    {
        QMessageBox::critical(this,"错误","数据表查询错误,错误信息\n"
                              +qryModel->lastError().text());
        return;
    }
    ui->statusbar->showMessage(QString("记录条数：%1").arg(qryModel->rowCount()));
    QSqlRecord rec=qryModel->record();
    qryModel->setHeaderData(rec.indexOf("empNo"),Qt::Horizontal,"工号");
    qryModel->setHeaderData(rec.indexOf("Name"),Qt::Horizontal,"姓名");
    qryModel->setHeaderData(rec.indexOf("Gender"),Qt::Horizontal,"性别");
    qryModel->setHeaderData(rec.indexOf("Birthday"),Qt::Horizontal,"出生日期");
    qryModel->setHeaderData(rec.indexOf("Province"),Qt::Horizontal,"省份");
    qryModel->setHeaderData(rec.indexOf("Department"),Qt::Horizontal,"部门");
    qryModel->setHeaderData(rec.indexOf("Salary"),Qt::Horizontal,"工资");
    //创建数据模型
    selectModel=new QItemSelectionModel(qryModel,this);
    connect(selectModel,&QItemSelectionModel::currentRowChanged,this,&MainWindow::do_currentRowChanged);
    ui->tableView->setModel(qryModel);
    ui->tableView->setSelectionModel(selectModel);

    // //字段与widget映射
    dataMapper=new QDataWidgetMapper(this);
    dataMapper->setModel(qryModel);
    dataMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    dataMapper->addMapping(ui->spinBoxEmpNo,rec.indexOf("empNo"));
    dataMapper->addMapping(ui->lineEditName,rec.indexOf("Name"));
    dataMapper->addMapping(ui->comboBoxSex,rec.indexOf("Gender"));
    dataMapper->addMapping(ui->dateTimeEditBirth,rec.indexOf("Birthday"));
    dataMapper->addMapping(ui->comboBoxProvince,rec.indexOf("Province"));
    dataMapper->addMapping(ui->spinBoxSalary,rec.indexOf("Salary"));
    dataMapper->addMapping(ui->comboBoxDep,rec.indexOf("Department"));

    dataMapper->toFirst();


     ui->actOpenDB->setEnabled(false);


}
void MainWindow::refreshTableView()
{
    int index=dataMapper->currentIndex();
    QModelIndex curIndex=qryModel->index(index,1);
    selectModel->clearSelection();
    selectModel->setCurrentIndex(curIndex,QItemSelectionModel::Select);
}
void MainWindow::openTable()
{
    tabModel =new QSqlTableModel(this,DB);//数据模型
    tabModel ->setTable("employee");//设置数据表
    tabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    tabModel->setSort(tabModel->fieldIndex("empNo"),Qt::AscendingOrder);
    if(!(tabModel->select()))
    {
        QMessageBox::critical(this,"错误信息","打开数据表错误,错误信息:\n"+tabModel->lastError().text());
        return;
    }
    showRecordCount();//显示记录条数
    //设置字段显示标题
    tabModel->setHeaderData(tabModel->fieldIndex("empNo"),Qt::Horizontal,"工号");
    tabModel->setHeaderData(tabModel->fieldIndex("Name"),Qt::Horizontal,"姓名");
    tabModel->setHeaderData(tabModel->fieldIndex("Gender"),Qt::Horizontal,"性别");
    tabModel->setHeaderData(tabModel->fieldIndex("Birthday"),Qt::Horizontal,"出生日期");
    tabModel->setHeaderData(tabModel->fieldIndex("Province"),Qt::Horizontal,"省份");
    tabModel->setHeaderData(tabModel->fieldIndex("Department"),Qt::Horizontal,"部门");
    tabModel->setHeaderData(tabModel->fieldIndex("Salary"),Qt::Horizontal,"工资");
    tabModel->setHeaderData(tabModel->fieldIndex("Memo"),Qt::Horizontal,"备注");
    tabModel->setHeaderData(tabModel->fieldIndex("Photo"),Qt::Horizontal,"照片");

    //创建选择模型
    selectModel=new QItemSelectionModel(tabModel,this);
 //行发生变化时
    connect(selectModel,&QItemSelectionModel::currentChanged,this,&MainWindow::do_currentChanged);
    connect(selectModel,&QItemSelectionModel::currentRowChanged,this,&MainWindow::do_currentRowChanged);
    //model/View 结构
    ui->tableView->setModel(tabModel);//设置数据模型
    ui->tableView->setSelectionModel(selectModel);//设置选择模型
    ui->tableView->setColumnHidden(tabModel->fieldIndex("Memo"),true);
    ui->tableView->setColumnHidden(tabModel->fieldIndex("Photo"),true);

    //为tableView中的性别和部门两个字段设置自定义代理组件
     QStringList strList;
     strList<<"男"<<"女";
     delegateSex.setItem(strList,false);
     ui->tableView->setItemDelegateForColumn(tabModel->fieldIndex("Gender"),&delegateSex);

    strList.clear();
    strList<<"技术部"<<"销售部"<<"市场部";
    delegateDepart.setItem(strList,false);
    ui->tableView->setItemDelegateForColumn(tabModel->fieldIndex("Department"),&delegateDepart);

    //字段与widget映射
    dataMapper=new QDataWidgetMapper(this);
    dataMapper->setModel(tabModel);
    dataMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    dataMapper->addMapping(ui->spinBoxEmpNo,tabModel->fieldIndex("empNo"));
    dataMapper->addMapping(ui->lineEditName,tabModel->fieldIndex("Name"));
    dataMapper->addMapping(ui->comboBoxSex,tabModel->fieldIndex("Gender"));
    dataMapper->addMapping(ui->dateTimeEditBirth,tabModel->fieldIndex("Birthday"));
    dataMapper->addMapping(ui->comboBoxProvince,tabModel->fieldIndex("Province"));
    dataMapper->addMapping(ui->spinBoxSalary,tabModel->fieldIndex("Salary"));
    dataMapper->addMapping(ui->comboBoxDep,tabModel->fieldIndex("Department"));
    dataMapper->addMapping(ui->plainTextEdit,tabModel->fieldIndex("Memo"));
    dataMapper->toFirst();

    ui->actOpenDB->setEnabled(false);
    ui->actAppend->setEnabled(true);
    ui->actInsert->setEnabled(true);
    ui->actDelete->setEnabled(true);
    ui->actSave->setEnabled(true);
    ui->groupBoxSort->setEnabled(true);
    ui->groupBoxFilter->setEnabled(true);
    ui->actAddMoney->setEnabled(true);

    QSqlRecord emptyRec=tabModel->record();
    for(int i=0;i<emptyRec.count();i++)
    {
        ui->comboBoxFiles->addItem(emptyRec.fieldName(i));
    }
}

void MainWindow::on_actAppend_triggered()
{
    QSqlRecord rec=tabModel->record();
    rec.setValue(tabModel->fieldIndex("Gender"),"男");
    tabModel->insertRecord(tabModel->rowCount(),rec);
    selectModel->clearSelection();
    selectModel->setCurrentIndex(tabModel->index(tabModel->rowCount()-1,1),
                                    QItemSelectionModel::Select);
    ui->statusbar->showMessage(QString("记录条数：%1").arg(tabModel->rowCount()));
}


void MainWindow::on_actInsert_triggered()
{
    QModelIndex curIndex=ui->tableView->currentIndex();
    QSqlRecord rec=tabModel->record();
    rec.setValue(tabModel->fieldIndex("Gender"),"男");
    tabModel->insertRecord(curIndex.row(),rec);
    selectModel->clearSelection();
    selectModel->setCurrentIndex(curIndex,
                                 QItemSelectionModel::Select);
    ui->statusbar->showMessage(QString("记录条数：%1").arg(tabModel->rowCount()));
}


void MainWindow::on_actDelete_triggered()
{
    QModelIndex curIndex=ui->tableView->currentIndex();
    tabModel->removeRow(curIndex.row());
    ui->statusbar->showMessage(QString("记录条数：%1").arg(tabModel->rowCount()));

}


void MainWindow::on_actSetPic_triggered()
{
    QString aFile=QFileDialog::getOpenFileName(this,"选择图片文件","","照片(*.jpg)");
    if(aFile.isEmpty())
        return;

    QByteArray data;
    QFile *file=new QFile(aFile);
    file->open(QIODevice::ReadOnly);
    data=file->readAll();
    file->close();
    delete file;

    int curNo=selectModel->currentIndex().row();
    QSqlRecord curRec=tabModel->record(curNo);
    curRec.setValue("Photo",data);
    tabModel->setRecord(curNo,curRec);

    QPixmap pic;
    pic.load(aFile);
    ui->labelPic->setPixmap(pic.scaledToWidth(ui->labelPic->width()));

}


void MainWindow::on_actClearPic_triggered()
{
    QSqlRecord curRec=tabModel->record(selectModel->currentIndex().row());
    curRec.setNull("Photo");
    tabModel->setRecord(selectModel->currentIndex().row(),curRec);
    ui->labelPic->clear();
}

void MainWindow::on_actAddMoney_triggered()
{
    if(tabModel->rowCount()==0)
        return;
    for(int i=0;i<tabModel->rowCount();i++)
    {
        QSqlRecord aRec=tabModel->record(i);
        aRec.setValue("Salary",aRec.value("Salary").toFloat()*1.1);
        tabModel->setRecord(i,aRec);
    }
    if(tabModel->submitAll())
        QMessageBox::information(this,"消息","涨工资计算完毕");
}
void MainWindow::on_actSave_triggered()
{
    bool res=tabModel->submitAll();
    if(!res)
    {
        QMessageBox::information(this,"消息","数据库保存失败\n"+tabModel->lastError().text());
    }else
    {
        ui->actSave->setEnabled(false);
        ui->actCancel->setEnabled(false);
    }
      ui->statusbar->showMessage(QString("记录条数：%1").arg(tabModel->rowCount()));
}


void MainWindow::on_actCancel_triggered()
{
    tabModel->revertAll();
    ui->actSave->setEnabled(false);
    ui->actCancel->setEnabled(false);
    ui->statusbar->showMessage(QString("记录条数：%1").arg(tabModel->rowCount()));
}

void MainWindow::on_comboBoxFiles_currentIndexChanged(int index)
{
    if(ui->radioButtonAscend->isChecked())
        tabModel->setSort(index,Qt::AscendingOrder);
    else
        tabModel->setSort(index,Qt:: DescendingOrder);
    tabModel->select();//更新model  触发view更新
}


void MainWindow::on_radioButtonAscend_clicked()
{
    tabModel->sort(ui->comboBoxFiles->currentIndex(),Qt::AscendingOrder);
}


void MainWindow::on_radioButtonDscend_clicked()
{
    tabModel->sort(ui->comboBoxFiles->currentIndex(),Qt::DescendingOrder);
}


void MainWindow::on_radioButtonMan_clicked()
{
    tabModel->setFilter("Gender='男'");
    ui->statusbar->showMessage(QString("记录条数：%1").arg(tabModel->rowCount()));
}


void MainWindow::on_radioButtonWoman_clicked()
{
    tabModel->setFilter("Gender='女'");
    ui->statusbar->showMessage(QString("记录条数：%1").arg(tabModel->rowCount()));
}


void MainWindow::on_radioButtonMWanALL_clicked()
{
    tabModel->setFilter("");
    ui->statusbar->showMessage(QString("记录条数：%1").arg(tabModel->rowCount()));
}


void MainWindow::on_actFirst_triggered()
{
    dataMapper->toFirst();
     refreshTableView();
}


void MainWindow::on_actPrevious_triggered()
{
    dataMapper->toPrevious();
     refreshTableView();
}


void MainWindow::on_actNext_triggered()
{
    dataMapper->toNext();
     refreshTableView();
}


void MainWindow::on_actLast_triggered()
{
    dataMapper->toLast();
    refreshTableView();
}

