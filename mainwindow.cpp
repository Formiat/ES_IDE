#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "limits"
#include "newprojectdialog.h"

//#include <QString>
//#include <QStringList>

//#include <QList>
//#include <QAction>
//#include <QListWidgetItem>
//#include <QMap>

#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    onProjectClosed();
}

MainWindow::~MainWindow()
{
    //delete proj;
    delete ui;
}

void MainWindow::onProjectOpened()
{
    this->setWindowTitle(proj->getProjName() + " - ES IDE");
    
    ui->actionNew_Project->setDisabled(true);
    ui->actionOpen_Project->setDisabled(true);
    
    ui->actionSave_Project->setEnabled(true);
    ui->actionClose_Project->setEnabled(true);
    ui->actionCheck->setEnabled(true);
    ui->actionInterpret->setEnabled(true);
    ui->actionGenerate_Source_Code->setEnabled(true);
    ui->tabWidget->setEnabled(true);
    
    ui->varList->addItems(proj->getVarNames());
    ui->errorsEdit->setText(Error(ErrorCode::NoErrors).text());
}

void MainWindow::onProjectClosed()
{
    delete proj;
    proj = nullptr;
    
    this->setWindowTitle("ES IDE");
    
    ui->actionNew_Project->setEnabled(true);
    ui->actionOpen_Project->setEnabled(true);
    
    ui->actionSave_Project->setDisabled(true);
    ui->actionClose_Project->setDisabled(true);
    ui->actionCheck->setDisabled(true);
    ui->actionInterpret->setDisabled(true);
    ui->actionGenerate_Source_Code->setDisabled(true);
    ui->tabWidget->setDisabled(true);
    ui->errorsEdit->setText("");
    ui->varList->clear();
    ui->valueList->clear();
    ui->varNameEdit->clear();
    ui->varValueEdit->clear();
}

bool MainWindow::askCloseProject()
{
    if (!proj->isSaved())
    {
        QMessageBox msgBox;
        msgBox.setText("The Project has been modified.");
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        
        switch (ret) {
        case QMessageBox::Save:
            proj->saveProject();
            break;
        case QMessageBox::Cancel:
            return false;
            break;
        }
    }
    return true;
}

void MainWindow::on_actionNew_Project_triggered()
{
    NewProjectDialog d;
    d.exec();
    
    while (!d.isClosed()) ;
    
    if (d.result() != NewProjectDialog::Accepted) return;
    
    proj = new Project(d.getNewProjectFolder(), d.getNewProjectName());
    
    onProjectOpened();
}

void MainWindow::on_actionOpen_Project_triggered()
{
    QString path = QFileDialog().getOpenFileName();
    
    if (path.isNull()) return;
    
    proj = new Project(path);
    
    onProjectOpened();
}

void MainWindow::on_actionSave_Project_triggered()
{
    proj->saveProject();
    
    ui->errorsEdit->setText(Error(ErrorCode::NoErrors).text());
}

void MainWindow::on_actionClose_Project_triggered()
{
    if (!askCloseProject()) return;
    onProjectClosed();
}

void MainWindow::on_actionExit_triggered()
{
    if (proj) 
    {
        if (!askCloseProject()) return;
    }
    close();
}

void MainWindow::on_varList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!ui->varList->currentItem()) return;
    ui->varNameEdit->setText(current->text());
    
    ui->valueList->clear();
    ui->valueList->addItems(*proj->getVarValues(current->text()));
    ui->errorsEdit->setText(Error(ErrorCode::NoErrors).text());
}

void MainWindow::on_valueList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (current)
    {
        ui->varValueEdit->setText(current->text());
    }
    else
    {
        ui->varValueEdit->clear();
    }
}

void MainWindow::on_addVarButton_clicked()
{
    QString newVarName;
    for (int i = 1; i <= INT_MAX; i++)
    {
        newVarName = "NewVar_" + QString::number(i);
        if (!proj->addVar(newVarName, QStringList())) break;
    }
    ui->varList->addItem(newVarName);
    ui->errorsEdit->setText(Error(ErrorCode::NoErrors).text());
}

void MainWindow::on_addValueButton_clicked()
{
    if (!ui->varList->currentItem()) return;
    QString newValueName;
    for (int i = 1; i <= INT_MAX; i++)
    {
        newValueName = "NewValue_" + QString::number(i);
        if (!proj->addVarValue(QStringList(newValueName), ui->varList->currentItem()->text())) break;
    }
    ui->valueList->addItem(newValueName);
    ui->errorsEdit->setText(Error(ErrorCode::NoErrors).text());
}

void MainWindow::on_renameVarButton_clicked()
{
    if (!ui->varList->currentItem()) return;
    Error err = proj->setVarName(ui->varNameEdit->text(), ui->varList->currentItem()->text());
    if (err)
    {
        ui->errorsEdit->setText("Rename Variable Error!\n\n" + err.text());
        return;
    }
    ui->errorsEdit->setText(err.text());
    ui->varList->currentItem()->setText(ui->varNameEdit->text());
}

void MainWindow::on_renameValueButton_clicked()
{
    if (!ui->valueList->currentItem()) return;
    Error err = proj->setVarValue(ui->varValueEdit->text(), ui->varList->currentItem()->text(), ui->valueList->currentItem()->text());
    if (err)
    {
        ui->errorsEdit->setText("Rename Variable Value Error!\n\n" + err.text());
        return;
    }
    ui->errorsEdit->setText(err.text());
    ui->valueList->currentItem()->setText(ui->varValueEdit->text());
}

void MainWindow::on_deleteVarButton_clicked()
{
    if (!ui->varList->currentItem()) return;
    proj->deleteVar(ui->varList->currentItem()->text());
    //ui->varList->removeItemWidget(ui->varList->currentItem());
    delete ui->varList->currentItem();
    ui->varList->setCurrentItem(nullptr);
    ui->varNameEdit->clear();
    ui->valueList->clear();
    ui->varValueEdit->clear();
    ui->errorsEdit->setText(Error(ErrorCode::NoErrors).text());
}

void MainWindow::on_deleteValueButton_clicked()
{
    if (!ui->valueList->currentItem()) return;
    proj->deleteVarValue(ui->varList->currentItem()->text(), ui->valueList->currentItem()->text());
    //ui->valueList->removeItemWidget(ui->valueList->currentItem());
    delete ui->valueList->currentItem();
    ui->valueList->setCurrentItem(nullptr);
    ui->varValueEdit->clear();
    ui->errorsEdit->setText(Error(ErrorCode::NoErrors).text());
}
