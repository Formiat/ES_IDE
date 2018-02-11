#include "resultwindow.h"
#include "ui_resultwindow.h"

ResultWindow::ResultWindow(const QMap<QString, QString> &map, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ResultWindow)
{
    ui->setupUi(this);
    
    initialize(map);
}

ResultWindow::~ResultWindow()
{
    delete ui;
}

void ResultWindow::initialize(const QMap<QString, QString> &map)
{
    int varNameMaxLength = 0;
    for (auto key : map.keys())
    {
        varNameMaxLength = ((key.length() > varNameMaxLength) ? key.length() : varNameMaxLength);
    }
    
    foreach (auto key, map.keys())
    {
        ui->varsAndValuesList->addItem(Pair(key, map[key]).stringify(false, (varNameMaxLength - key.length()), false));
    }
}

void ResultWindow::on_closeButton_clicked()
{
    close();
}
