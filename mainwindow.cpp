#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "limits"
#include "newprojectdialog.h"

#include <QCloseEvent>
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
    delete proj;
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (proj) 
    {
        if (!askCloseProject())
        {
            event->ignore();
            return;
        }
    }
    event->accept();
}

void MainWindow::onProjectOpened()
{
    windowTitle = proj->getProjName() + " - ES IDE";
    this->setWindowTitle(windowTitle);
    
    ui->actionNew_Project->setDisabled(true);
    ui->actionOpen_Project->setDisabled(true);
    
    ui->actionSave_Project->setEnabled(true);
    ui->actionClose_Project->setEnabled(true);
    ui->actionCheck->setEnabled(true);
    ui->actionInterpret->setEnabled(true);
    ui->actionGenerate_Source_Code->setEnabled(true);
    ui->tabWidget->setEnabled(true);
    
    ui->varList->addItems(proj->getVarNames());
    ui->varErrorsEdit->setText(Error(ErrorCode::NoErrors).text());
    
    ui->ruleList->addItems(proj->getRulezzStringified());
    ui->ruleErrorsEdit->setText(Error(ErrorCode::NoErrors).text());
    
}

void MainWindow::onProjectClosed()
{
    delete proj;
    proj = nullptr;
    
    setWindowTitle("ES IDE");
    
    ui->actionNew_Project->setEnabled(true);
    ui->actionOpen_Project->setEnabled(true);
    
    ui->actionSave_Project->setDisabled(true);
    ui->actionClose_Project->setDisabled(true);
    ui->actionCheck->setDisabled(true);
    ui->actionInterpret->setDisabled(true);
    ui->actionGenerate_Source_Code->setDisabled(true);
    ui->tabWidget->setDisabled(true);
    
    ui->varErrorsEdit->clear();
    ui->varList->clear();
    ui->valueList->clear();
    ui->varNameEdit->clear();
    ui->varValueEdit->clear();
    
    ui->ruleErrorsEdit->clear();
    ui->ruleList->clear();
    
    ui->ifBlockEdit->clear();
    ui->varIfComboBox->clear();
    ui->valueIfComboBox->clear();
    
    ui->thenBlockEdit->clear();
    ui->varThenComboBox->clear();
    ui->valueThenComboBox->clear();
    
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
    
    if (d.result() != NewProjectDialog::Accepted) return;
    
    proj = new Project(d.getNewProjectFolder(), d.getNewProjectName());
    
    onProjectOpened();
}

void MainWindow::on_actionOpen_Project_triggered()
{
    QString selectedFilter = tr("ES Projects (*.esp)");
    QString path = QFileDialog::getOpenFileName(this, tr("Open Project"), QString(), tr("ES Projects (*.esp);;All files (*.*)"), &selectedFilter);
    
    if (path.isNull()) return;
    
    proj = new Project(path);
    
    onProjectOpened();
}

void MainWindow::on_actionSave_Project_triggered()
{
    setWindowTitle(windowTitle);
    
    proj->saveProject();
    
    ui->varErrorsEdit->setText(Error(ErrorCode::NoErrors).text());
}

void MainWindow::on_actionClose_Project_triggered()
{
    if (!askCloseProject()) return;
    onProjectClosed();
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox msgBox;
    msgBox.setText(tr("About the program ES IDE"));
    QString text;
    text.append(tr("Author") + ":\n");
    text.append(tr("Alexander Aglerimov") + "\n");
    text.append(QString("https://github.com/Formiat/ES_IDE") + "\n");
    text.append(QString("formiat95@gmail.com") + "\n");
    text.append(tr("Russia, Taganrog") + "\n");
    msgBox.setInformativeText(text);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
}

void MainWindow::on_varList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current) return;
    ui->varNameEdit->setText(current->text());
    
    ui->valueList->clear();
    ui->valueList->addItems(*proj->getVarValues(current->text()));
    ui->varErrorsEdit->setText(Error(ErrorCode::NoErrors).text());
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
    ui->varErrorsEdit->setText(Error(ErrorCode::NoErrors).text());
}

void MainWindow::on_addVarButton_clicked()
{
    QString newVarName;
    for (int i = 1; i <= INT_MAX - 1; i++)
    {
        newVarName = "__NewVar_" + QString::number(i);
        if (!proj->addVar(newVarName, QStringList())) break;
    }
    ui->varList->addItem(newVarName);
    ui->varErrorsEdit->setText(Error(ErrorCode::NoErrors).text());
    this->setWindowTitle("* " + windowTitle);
}

void MainWindow::on_addValueButton_clicked()
{
    if (!ui->varList->currentItem()) return;
    QString newValueName;
    for (int i = 1; i <= INT_MAX - 1; i++)
    {
        newValueName = "__NewValue_" + QString::number(i);
        if (!proj->addVarValue(newValueName, ui->varList->currentItem()->text())) break;
    }
    ui->valueList->addItem(newValueName);
    ui->varErrorsEdit->setText(Error(ErrorCode::NoErrors).text());
    this->setWindowTitle("* " + windowTitle);
}

void MainWindow::on_renameVarButton_clicked()
{
    if (!ui->varList->currentItem()) return;
    Error err = proj->setVarName(ui->varNameEdit->text(), ui->varList->currentItem()->text());
    if (err)
    {
        ui->varErrorsEdit->setText(tr("Rename Variable Error!") + "\n\n" + err.text());
        return;
    }
    ui->varErrorsEdit->setText(err.text());
    ui->varList->currentItem()->setText(ui->varNameEdit->text());
    this->setWindowTitle("* " + windowTitle);
}

void MainWindow::on_renameValueButton_clicked()
{
    if (!ui->valueList->currentItem()) return;
    Error err = proj->setVarValue(ui->varValueEdit->text(), ui->varList->currentItem()->text(), ui->valueList->currentItem()->text());
    if (err)
    {
        ui->varErrorsEdit->setText(tr("Rename Variable Value Error!") + "\n\n" + err.text());
        return;
    }
    ui->varErrorsEdit->setText(err.text());
    ui->valueList->currentItem()->setText(ui->varValueEdit->text());
    this->setWindowTitle("* " + windowTitle);
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
    ui->varErrorsEdit->setText(Error(ErrorCode::NoErrors).text());
    this->setWindowTitle("* " + windowTitle);
}

void MainWindow::on_deleteValueButton_clicked()
{
    if (!ui->valueList->currentItem()) return;
    proj->deleteVarValue(ui->varList->currentItem()->text(), ui->valueList->currentItem()->text());
    //ui->valueList->removeItemWidget(ui->valueList->currentItem());
    delete ui->valueList->currentItem();
    ui->valueList->setCurrentItem(nullptr);
    ui->varValueEdit->clear();
    ui->varErrorsEdit->setText(Error(ErrorCode::NoErrors).text());
    this->setWindowTitle("* " + windowTitle);
}

void MainWindow::on_ruleList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current) return;
    
    auto rule = proj->getRule(current->text());
    
    ui->ifBlockEdit->setText(rule->stringifyIfBlock());
    ui->thenBlockEdit->setText(rule->stringifyThenBlock());
    
    ui->varIfComboBox->clear();
    ui->varIfComboBox->addItems(proj->getVarNames());
    ui->varThenComboBox->clear();
    ui->varThenComboBox->addItems(proj->getVarNames());
    ui->ruleErrorsEdit->setText(Error(ErrorCode::NoErrors).text());
}

void MainWindow::on_addRuleButton_clicked()
{
    proj->addRule(Rule());
    ui->ruleList->addItem(proj->getRuleStringified());
    this->setWindowTitle("* " + windowTitle);
    ui->ruleErrorsEdit->setText(Error(ErrorCode::NoErrors).text());
}

void MainWindow::on_deleteRuleButton_clicked()
{
    if (!ui->ruleList->currentItem()) return;
    
    proj->deleteRule(ui->ruleList->currentItem()->text());
    ui->ruleList->clear();
    ui->ruleList->addItems(proj->getRulezzStringified());
    
    ui->ifBlockEdit->clear();
    ui->varIfComboBox->clear();
    ui->valueIfComboBox->clear();
    
    ui->thenBlockEdit->clear();
    ui->varThenComboBox->clear();
    ui->valueThenComboBox->clear();
    
    ui->ruleErrorsEdit->setText(Error(ErrorCode::NoErrors).text());
    this->setWindowTitle("* " + windowTitle);
}

void MainWindow::on_varIfComboBox_currentIndexChanged(const QString &arg1)
{
    if (arg1.isEmpty()) return;
    
    auto result = proj->getVarValues(arg1);
    if (!result) return;
    
    ui->valueIfComboBox->clear();
    ui->valueIfComboBox->addItems(*result);
    ui->ruleErrorsEdit->setText(Error(ErrorCode::NoErrors).text());
}

void MainWindow::on_valueIfComboBox_currentIndexChanged(const QString &arg1)
{
    ui->ruleErrorsEdit->setText(Error(ErrorCode::NoErrors).text());
}

void MainWindow::on_backspaceIfButton_clicked()
{
    if (!ui->ruleList->currentItem()) return;
    
    proj->deleteIfPair(ui->ruleList->currentItem()->text());
    ui->ruleList->currentItem()->setText(proj->getRuleStringified(ui->ruleList->currentItem()->text()));
    ui->ifBlockEdit->setText(proj->getRule(ui->ruleList->currentItem()->text())->stringifyIfBlock());
    
    this->setWindowTitle("* " + windowTitle);
}

void MainWindow::on_enterIfButton_clicked()
{
    if (!ui->ruleList->currentItem()) return;
    if (ui->varIfComboBox->currentText().isEmpty()) return;
    if (ui->valueIfComboBox->currentText().isEmpty()) return;
    
    Error err = proj->addIfPair(Pair(ui->varIfComboBox->currentText(), ui->valueIfComboBox->currentText()), ui->ruleList->currentItem()->text());
    if (err)
    {
        ui->ruleErrorsEdit->setText(tr("Add Pair Error!") + "\n\n" + err.text());
        return;
    }
    ui->ruleErrorsEdit->setText(err.text());
    
    ui->ruleList->currentItem()->setText(proj->getRuleStringified(ui->ruleList->currentItem()->text()));
    ui->ifBlockEdit->setText(proj->getRule(ui->ruleList->currentItem()->text())->stringifyIfBlock());
    
    this->setWindowTitle("* " + windowTitle);
}

void MainWindow::on_varThenComboBox_currentIndexChanged(const QString &arg1)
{
    if (arg1.isEmpty()) return;
    
    auto result = proj->getVarValues(arg1);
    if (!result) return;
    
    ui->valueThenComboBox->clear();
    ui->valueThenComboBox->addItems(*result);
    ui->ruleErrorsEdit->setText(Error(ErrorCode::NoErrors).text());
}

void MainWindow::on_valueThenComboBox_currentIndexChanged(const QString &arg1)
{
    ui->ruleErrorsEdit->setText(Error(ErrorCode::NoErrors).text());
}

void MainWindow::on_backspaceThenButton_clicked()
{
    if (!ui->ruleList->currentItem()) return;
    
    proj->deleteThenPair(ui->ruleList->currentItem()->text());
    ui->ruleList->currentItem()->setText(proj->getRuleStringified(ui->ruleList->currentItem()->text()));
    ui->thenBlockEdit->setText(proj->getRule(ui->ruleList->currentItem()->text())->stringifyThenBlock());
    
    this->setWindowTitle("* " + windowTitle);
}

void MainWindow::on_enterThenButton_clicked()
{
    if (!ui->ruleList->currentItem()) return;
    if (ui->varThenComboBox->currentText().isEmpty()) return;
    if (ui->valueThenComboBox->currentText().isEmpty()) return;
    
    Error err = proj->addThenPair(Pair(ui->varThenComboBox->currentText(), ui->valueThenComboBox->currentText()), ui->ruleList->currentItem()->text());
    if (err)
    {
        ui->ruleErrorsEdit->setText(tr("Add Pair Error!") + "\n\n" + err.text());
        return;
    }
    ui->ruleErrorsEdit->setText(err.text());
    
    ui->ruleList->currentItem()->setText(proj->getRuleStringified(ui->ruleList->currentItem()->text()));
    ui->thenBlockEdit->setText(proj->getRule(ui->ruleList->currentItem()->text())->stringifyThenBlock());
    
    this->setWindowTitle("* " + windowTitle);
}
