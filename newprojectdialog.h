#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <QDialog>

namespace Ui {
class NewProjectDialog;
}

class NewProjectDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit NewProjectDialog(QWidget *parent = 0);
    ~NewProjectDialog();
    
public:
    QString getNewProjectName() const;
    QString getNewProjectFolder() const;
    bool isClosed() const;
    
private slots:
    void on_pushButton_clicked();
    
    void on_buttonBox_accepted();
    
    void on_buttonBox_rejected();
    
private:
    Ui::NewProjectDialog *ui;
    
    bool dialogClosed;
};

#endif // NEWPROJECTDIALOG_H
