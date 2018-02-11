#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "project.h"


class Interpreter
{
public:
    Interpreter(const QStringList &varNames, const QList<Rule> &rules);
    
public:
    const QStringList &getRequiredInputVarList() const;
    const QStringList &getOutputVarList() const;
    QMap<QString, QString> interpret(const QMap<QString, QString> &input) const;
    QStringList interpretAndStringify(const QMap<QString, QString> &input) const;
    
private:
    void initialize();
    
private:
    QStringList varNames;
    //QList<QStringList> varValues;
    QList<Rule> rules;
    
    QList<QList<Rule>> structuredRules;
    
//    QStringList ifBlockVars;
//    QStringList thenBlockVars;
    QStringList inputVars;
//    QStringList internalVars;
    QStringList outputVars;
    
    //mutable QMap<QString, QString> enteredValues;
    
};

#endif // INTERPRETER_H
