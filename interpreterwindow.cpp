#include "interpreterwindow.h"
#include "ui_interpreterwindow.h"

#include <cstdlib>
#include <ctime>


InterpreterWindow::InterpreterWindow(const Project &proj, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InterpreterWindow),
    proj(proj),
    interp(proj.getVarNames(), proj.getRules()),
    varNameMaxLength(0)
{
    ui->setupUi(this);
    
    initialize();
}

InterpreterWindow::~InterpreterWindow()
{
    delete ui;
}

void InterpreterWindow::initialize()
{
    std::srand(unsigned(std::time(0)));
    
    QStringList inputVars = interp.getRequiredInputVarList();
    ui->varComboBox->addItems(inputVars);
    
    for (auto str : inputVars)
    {
        varNameMaxLength = ((str.length() > varNameMaxLength) ? str.length() : varNameMaxLength);
    }
    
    // Converting StringList to Map
    for (int i = 0; i < inputVars.length(); i++)
    {
        QStringList temp = *proj.getVarValues(inputVars.at(i));
        
        inputVarsMap[inputVars.at(i)] = temp.at(std::rand() % temp.length());
    }
    
    refreshVarsAndValuesList();
    ui->errorsEdit->setText(Error(ErrorCode::NoErrors).text());
}

void InterpreterWindow::refreshVarsAndValuesList()
{
    ui->varsAndValuesList->clear();
    
    foreach (auto key, inputVarsMap.keys())
    {
        ui->varsAndValuesList->addItem(Pair(key, inputVarsMap[key]).stringify(false, (varNameMaxLength - key.length()), false));
    }
    
    ui->errorsEdit->setText(Error(ErrorCode::NoErrors).text());
}

void InterpreterWindow::on_varComboBox_currentIndexChanged(const QString &arg1)
{
    if (arg1.isEmpty()) return;
    
    auto result = proj.getVarValues(arg1);
    if (!result) return;
    
    ui->valueComboBox->clear();
    ui->valueComboBox->addItems(*result);
    ui->errorsEdit->setText(Error(ErrorCode::NoErrors).text());
}

void InterpreterWindow::on_enterButton_clicked()
{
    if (ui->varComboBox->currentText().isEmpty()) return;
    if (ui->valueComboBox->currentText().isEmpty()) return;
    
    inputVarsMap[ui->varComboBox->currentText()] = ui->valueComboBox->currentText();
    refreshVarsAndValuesList();
}

void InterpreterWindow::on_interpretButton_clicked()
{
    for (auto value : inputVarsMap)
    {
        if (value.isEmpty())
        {
            ui->errorsEdit->setText("You should enter value for all Variables.");
            return;
        }
    }
    
    ui->errorsEdit->setText(Error(ErrorCode::NoErrors).text());
    
    QMap<QString, QString> result = interp.interpret(inputVarsMap);
    
    resWindow = new ResultWindow(result);
    resWindow->show();
}
