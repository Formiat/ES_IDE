#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "project.h"
#include "interpreterwindow.h"

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
    InterpreterWindow *interpWindow;
    QString windowTitle;
    
    QMap<QString, QString> inputValues;
    
private:
    void closeEvent(QCloseEvent *event);
    
private slots:
    void onProjectOpened();
    void onProjectClosed();
    
    bool askCloseProject();
    
    void on_actionNew_Project_triggered();
    void on_actionOpen_Project_triggered();
    void on_actionSave_Project_triggered();
    void on_actionClose_Project_triggered();
    void on_actionExit_triggered();
    
    void on_actionInterpret_triggered();
    
    void on_actionAbout_triggered();
    
    void on_varList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_valueList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    
    void on_addVarButton_clicked();
    void on_addValueButton_clicked();
    
    void on_renameVarButton_clicked();
    void on_renameValueButton_clicked();
    
    void on_deleteVarButton_clicked();
    void on_deleteValueButton_clicked();
    
    void on_ruleList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_addRuleButton_clicked();
    void on_deleteRuleButton_clicked();
    
    void on_varIfComboBox_currentIndexChanged(const QString &arg1);
    void on_valueIfComboBox_currentIndexChanged(const QString &arg1);
    void on_backspaceIfButton_clicked();
    void on_enterIfButton_clicked();
    
    void on_varThenComboBox_currentIndexChanged(const QString &arg1);
    void on_valueThenComboBox_currentIndexChanged(const QString &arg1);
    void on_backspaceThenButton_clicked();
    void on_enterThenButton_clicked();
    
};

#endif // MAINWINDOW_H
