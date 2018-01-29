#include "newprojectdialog.h"
#include "ui_newprojectdialog.h"

#include <QFileDialog>

NewProjectDialog::NewProjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewProjectDialog),
    dialogClosed(false)
{
    ui->setupUi(this);
}

NewProjectDialog::~NewProjectDialog()
{
    delete ui;
}

QString NewProjectDialog::getNewProjectName() const
{
    return ui->newProjectNameEdit->text();
}

QString NewProjectDialog::getNewProjectFolder() const
{
    return ui->newProjectFolderEdit->text();
}

bool NewProjectDialog::isClosed() const
{
    return dialogClosed;
}

void NewProjectDialog::on_pushButton_clicked()
{
    QString path = QFileDialog().getExistingDirectory();
    if (path.isNull()) return;
    ui->newProjectFolderEdit->setText(path);
}

void NewProjectDialog::on_buttonBox_accepted()
{
    dialogClosed = true;
}

void NewProjectDialog::on_buttonBox_rejected()
{
    dialogClosed = true;
}
