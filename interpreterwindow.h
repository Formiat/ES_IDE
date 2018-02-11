#ifndef INTERPRETERWINDOW_H
#define INTERPRETERWINDOW_H

#include <QWidget>

#include "project.h"
#include "interpreter.h"
#include "resultwindow.h"


namespace Ui {
class InterpreterWindow;
}

class InterpreterWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit InterpreterWindow(const Project &proj, QWidget *parent = 0);
    ~InterpreterWindow();
    
private slots:
    void on_varComboBox_currentIndexChanged(const QString &arg1);
    
    void on_enterButton_clicked();
    
    void on_interpretButton_clicked();
    
private:
    void initialize();
    void refreshVarsAndValuesList();
    
private:
    Ui::InterpreterWindow *ui;
    
    Project proj;
    Interpreter interp;
    ResultWindow *resWindow;
    
    QMap<QString, QString> inputVarsMap;
    
    int varNameMaxLength;
    
};

#endif // INTERPRETERWINDOW_H
