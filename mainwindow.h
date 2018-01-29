#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "project.h"

#include <QMainWindow>

class QListWidgetItem;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    
    Project *proj;
    
private slots:
    void onProjectOpened();
    void onProjectClosed();
    
    bool askCloseProject();
    
    void on_actionNew_Project_triggered();
    void on_actionOpen_Project_triggered();
    void on_actionSave_Project_triggered();
    void on_actionClose_Project_triggered();
    void on_actionExit_triggered();
    
    void on_varList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_valueList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    
    void on_addVarButton_clicked();
    void on_addValueButton_clicked();
    
    void on_renameVarButton_clicked();
    void on_renameValueButton_clicked();
    
    void on_deleteVarButton_clicked();
    void on_deleteValueButton_clicked();
};

#endif // MAINWINDOW_H
