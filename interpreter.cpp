#include "interpreter.h"

#include <QDebug>

Interpreter::Interpreter(const QStringList &varNames, const QList<Rule> &rules) :
    varNames(varNames),
    rules(rules)
{
    initialize();
}

const QStringList &Interpreter::getRequiredInputVarList() const
{
    return inputVars;
}

const QStringList &Interpreter::getOutputVarList() const
{
    return outputVars;
}

QMap<QString, QString> Interpreter::interpret(const QMap<QString, QString> &input) const
{
    QMap<QString, QString> output;
    QMap<QString, QString> internal;
    internal = input;
    
    qDebug() << "\n " << "Input vars:";
    foreach (auto key, input.keys())
    {
        qDebug() << Pair(key, input[key]).stringify(true);
    }
    
    qDebug() << "\n " << "Internal vars:";
    foreach (auto key, internal.keys())
    {
        qDebug() << Pair(key, internal[key]).stringify(true);
    }
    
    for (auto level : structuredRules)
    {
        
        qDebug() << "\n " << "Level processing started";
        
        for (auto rule : level)
        {
            
            qDebug() << "\n " << "Rule : " << rule.stringify();
            
            bool result = true;
            
            for (auto ifPair : rule.ifBlock)
            {
                
                qDebug() << "IF-Pair : " << ifPair.stringify(true);
                
                if (internal.value(ifPair.var) != ifPair.value)
                {
                    
                    qDebug() << "Pair gives False";
                    
                    result = false;
                    break;
                }
            }
            
            if (result)
            {
                for (auto thenPair : rule.thenBlock)
                {
                    
                    qDebug() << "Assign : " << Pair(thenPair.var, thenPair.value).stringify(false);
                    
                    internal[thenPair.var] = thenPair.value;
                }
            }
        }
    }
    
    qDebug() << "\n " << "Output vars:";
    // Making output map
    for (auto var : outputVars)
    {
        output[var] = internal[var];
        
        qDebug() << Pair(var, output[var]).stringify(false);
        
    }
    
    return output;
}

QStringList Interpreter::interpretAndStringify(const QMap<QString, QString> &input) const
{
    auto output = interpret(input);
    
    QStringList result;
    
    foreach (auto key, output.keys())
    {
        result.append(key + " <= " + output.value(key));
    }
    
    return result;
}

void Interpreter::initialize()
{
    QStringList ifBlockVars;
    QStringList thenBlockVars;
    //QStringList inputVars;
    QStringList internalVars;
    //QStringList outputVars;
    
    structuredRules.append(QList<Rule>());
    
    for (auto rule : rules)
    {
        // Initialization of "structuredRules"
        structuredRules[0].append(rule);
        
        // Find all If Block Vars
        for (auto ifPair : rule.ifBlock)
        {
            if (!ifBlockVars.contains(ifPair.var)) ifBlockVars.append(ifPair.var);
        }
        
        // Find all Then Block Vars
        for (auto thenPair : rule.thenBlock)
        {
            if (!thenBlockVars.contains(thenPair.var)) thenBlockVars.append(thenPair.var);
        }
    }
    
    // Vars distribution on Input, Internal and Output
    for (auto var : varNames)
    {
        bool isInIfBlock = ifBlockVars.contains(var);
        bool isInThenBlock = thenBlockVars.contains(var);
        
        if (isInIfBlock)
        {
            if (isInThenBlock)
            {
                internalVars.append(var);
            }
            else
            {
                inputVars.append(var);
            }
        }
        else if (isInThenBlock)
        {
            outputVars.append(var);
        }
    }
    
    
    QStringList definedVars;
    
    definedVars.append(inputVars);
    
    // Rule distribution by levels
    for (int i = 0; i < structuredRules.length(); i++)
    {
        //QList<Rule> &level = structuredRules[i];
        bool nextLevelExists = false;
        
        qDebug() << "\n " << "Level " << i << " processing started";
        
        for (int j = 0; j < structuredRules[i].length(); j++)
        {
            //Rule &rule = structuredRules[i][j];
            bool ruleLifted = false;
            
            qDebug() << "\n " << "Rule " << j << " : " << structuredRules[i][j].stringify();
            
            for (int k = 0; k < structuredRules[i][j].ifBlock.length(); k++)
            {
                //Pair &ifPair = structuredRules[i][j].ifBlock[k];
                
                qDebug() << "IF-Pair " << k << " : " << structuredRules[i][j].ifBlock[k].stringify(true);
                
                if (!definedVars.contains(structuredRules[i][j].ifBlock[k].var))
                {
                    
                    qDebug() << "Defined-Vars not contains " << structuredRules[i][j].ifBlock[k].var;
                    
                    if (!nextLevelExists)
                    {
                        
                        qDebug() << "Next-Level not exists";
                        
                        structuredRules.append(QList<Rule>());
                        nextLevelExists = true;
                    }
                    
                    structuredRules[i + 1].append(structuredRules[i].takeAt(j));
                    j--;
                    ruleLifted = true;
                    break;
                }
            }
            
            if (ruleLifted)
            {
                
                qDebug() << "Rule Lifted. Level length: " << structuredRules[i].length();
                
                continue;
            }
        }
        
        qDebug() << "\n ";
        
        // Add to "definedVars" all Vars from current level
        for (auto rule : structuredRules[i])
        {
            for (auto thenPair : rule.thenBlock)
            {
                if (!definedVars.contains(thenPair.var))
                {
                    
                    qDebug() << "Var " << thenPair.var << " added";
                    
                    definedVars.append(thenPair.var);
                }
            }
        }
    }
    
    for (int i = 0; i < structuredRules.length(); i++)
    {
        QList<Rule> &level = structuredRules[i];
        qDebug() << "\n " << "Level: " << i;
        
        for (int j = 0; j < level.length(); j++)
        {
            Rule &rule = level[j];
            qDebug() << "Rule: " << rule.stringify();
            
        }
    }
    
    qDebug() << "\n " << "Input vars:";
    for (int i = 0; i < inputVars.length(); i++)
    {
        qDebug() << inputVars[i];
    }
    
    qDebug() << "\n " << "Output vars:";
    for (int i = 0; i < outputVars.length(); i++)
    {
        qDebug() << outputVars[i];
    }
    
}
