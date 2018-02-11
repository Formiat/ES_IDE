#ifndef RESULTWINDOW_H
#define RESULTWINDOW_H

#include "project.h"

#include <QWidget>

namespace Ui {
class ResultWindow;
}

class ResultWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit ResultWindow(const QMap<QString, QString> &map, QWidget *parent = 0);
    ~ResultWindow();
    
private slots:
    void on_closeButton_clicked();
    
private:
    inline void initialize(const QMap<QString, QString> &map);
    
private:
    Ui::ResultWindow *ui;
};

#endif // RESULTWINDOW_H
