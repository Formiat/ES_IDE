#include "newprojectdialog.h"
#include "ui_newprojectdialog.h"

#include <QFileDialog>

NewProjectDialog::NewProjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewProjectDialog)
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

void NewProjectDialog::on_pushButton_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("New Project"));
    if (path.isNull()) return;
    ui->newProjectFolderEdit->setText(path);
}
