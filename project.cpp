#include "project.h"

#include <QFile>
#include <QIODevice>
#include <QStringList>
#include <QDebug>
#include <QDir>
#include <QSaveFile>
#include <QtMath>


Error::Error(ErrorCode errCode) : errCode(errCode)
{
    switch (errCode) {
    case ErrorCode::NoErrors:
        message = QCoreApplication::translate("Error", "No Errors.");
        break;
    case ErrorCode::InvalidIdentifier:
        message = QCoreApplication::translate("Error", "Invalid Identifier: Identifier must consist only of letters, digits and underscores, and must not begin with a digit.");
        break;
    case ErrorCode::IdentifierAlreadyExists:
        message = QCoreApplication::translate("Error", "Identifier already exists.");
        break;
    case ErrorCode::UnknownVariableId:
        message = QCoreApplication::translate("Error", "Unknown Variable Id.");
        break;
    case ErrorCode::UnknownVariableName:
        message = QCoreApplication::translate("Error", "Unknown Variable Name.");
        break;
    case ErrorCode::UnknownValueId:
        message = QCoreApplication::translate("Error", "Unknown Value Id.");
        break;
    case ErrorCode::UnknownValueName:
        message = QCoreApplication::translate("Error", "Unknown Value Name.");
        break;
    case ErrorCode::UnknownRuleId:
        message = QCoreApplication::translate("Error", "Unknown Rule Id.");
        break;
    case ErrorCode::UnknownPairId:
        message = QCoreApplication::translate("Error", "Unknown Pair Id.");
        break;
    case ErrorCode::PairAlreadyExists:
        message = QCoreApplication::translate("Error", "Pair already exists.");
        break;
    }
}

Pair::Pair()
{
    
}

Pair::Pair(const QString &l, const QString &r) : l(l), r(r)
{
    
}

QString Rule::stringify() const
{
    QString result;
    
    result.append("IF " + stringifyIfBlock() + " ");
    result.append("THEN " + stringifyThenBlock());
    
    return result;
}

QString Rule::stringifyIfBlock() const
{
    QString result;
    
    int last = ifBlock.length() - 1;
    if (last >= 0)
    {
        for (int i = 0; i < last; i++)
        {
            result.append("(" + ifBlock.at(i).l + " == " + ifBlock.at(i).r + ")" + " & ");
        }
        result.append("(" + ifBlock.at(last).l + " == " + ifBlock.at(last).r + ")");
    }
    
    return result;
}

QString Rule::stringifyThenBlock() const
{
    QString result;
    
    int last = thenBlock.length() - 1;
    if (last >= 0)
    {
        for (int i = 0; i < last; i++)
        {
            result.append("(" + thenBlock.at(i).l + " <= " + thenBlock.at(i).r + ")" + "; ");
        }
        result.append("(" + thenBlock.at(last).l + " <= " + thenBlock.at(last).r + ")" + ".");
    }
    
    return result;
}

// Constructor for Existing Project; we pass path to ".esp" file
Project::Project(const QString &projFilePath)
    : projFilePath(projFilePath), regexpIdentifier("[_a-zA-Z][_a-zA-Z0-9]*")
{
    QFile projFile(projFilePath);
    projFile.open(QFile::ReadOnly);
    
    QTextStream projFileStream(&projFile);
    
    QString projFolder(projFilePath.mid(0, projFilePath.lastIndexOf("/", -1) + 1));
    projName = projFileStream.readLine();
    
    // Start loading variables
    varFilePath = projFolder + projFileStream.readLine();
    
    QFile varFile(varFilePath);
    varFile.open(QFile::ReadOnly);
    
    QTextStream varFileStream(&varFile);
    
    while (!varFileStream.atEnd())
    {
        QStringList line = varFileStream.readLine().split(".");
        
        varNames.append(line.at(0));
        
        QStringList valueBlock;
        
        for (int i = 1; i < line.length(); i++)
        {
            valueBlock.append(line.at(i));
        }
        
        varValues.append(valueBlock);
    }
    varFile.close();
    // End loading variables
    
    // Start loading rules
    rulFilePath = projFolder + projFileStream.readLine();
    projFile.close();
    
    QFile rulFile(rulFilePath);
    rulFile.open(QFile::ReadOnly);
    
    QTextStream rulFileStream(&rulFile);
    
    while (!rulFileStream.atEnd())
    {
        Rule r;
        QStringList line = rulFileStream.readLine().split("-");
        
        QStringList ifPart = line.at(0).split("&");
        for (int i = 0; i < ifPart.length(); i++)
        {
            QStringList pair = ifPart.at(i).split("=");
            if (pair.length() < 2) continue;
            Pair p{pair.at(0), pair.at(1)};
            r.ifBlock.append(p);
        }
        
        QStringList thenPart = line.at(1).split("&");
        for (int i = 0; i < thenPart.length(); i++)
        {
            QStringList pair = thenPart.at(i).split("=");
            if (pair.length() < 2) continue;
            Pair p{pair.at(0), pair.at(1)};
            r.thenBlock.append(p);
        }
        
        rules.append(r);
    }
    rulFile.close();
    // End loading rules
    
    saved = true;
}

// Constructor for New Project; we pass path to desired project folder
Project::Project(const QString &folderPath, const QString &projName)
    : projName(projName), regexpIdentifier("[_a-zA-Z][_a-zA-Z0-9]*")
{
    QString newProjFolderPath = folderPath + "/" + projName;
    QDir().mkpath(newProjFolderPath);
    QString tempStr = newProjFolderPath + "/" + projName;
    
    projFilePath = tempStr + ".esp";
    QFile projFile(projFilePath);
    projFile.open(QFile::WriteOnly | QFile::Truncate);
    
    QTextStream projFileStream(&projFile);
    
    projFileStream << projName + "\n";
    projFileStream << projName + ".var" + "\n";
    projFileStream << projName + ".rul" + "\n";
    
    projFile.close();
    
    varFilePath = tempStr + ".var";
    QFile varFile(varFilePath);
    varFile.open(QFile::WriteOnly | QFile::Truncate);
    
    varFile.close();
    
    rulFilePath = tempStr + ".rul";
    QFile rulFile(rulFilePath);
    rulFile.open(QFile::WriteOnly | QFile::Truncate);
    
    rulFile.close();
    
    saved = true;
}

void Project::saveProject() const
{
    // Start saving variables
    QSaveFile varFile(varFilePath);
    varFile.open(QSaveFile::WriteOnly | QSaveFile::Truncate);
    QTextStream varFileStream(&varFile);
    
    for (int i = 0; i < varNames.length(); i++)
    {
        varFileStream << varNames.at(i);
        for (int j = 0; j < varValues.at(i).length(); j++)
        {
            varFileStream << "." + varValues.at(i).at(j);
        }
        varFileStream << "\n";
    }
    varFile.commit();
    // End saving variables
    
    // Start saving rules
    QSaveFile rulFile(rulFilePath);
    rulFile.open(QSaveFile::WriteOnly | QSaveFile::Truncate);
    QTextStream rulFileStream(&rulFile);
    
    int last;
    for (int i = 0; i < rules.length(); i++)
    {
        // Output If-Block
        last = rules.at(i).ifBlock.length() - 1;
        if (last >= 0)
        {
            for (int j = 0; j < last; j++)
            {
                rulFileStream << rules.at(i).ifBlock.at(j).l + "=" + rules.at(i).ifBlock.at(j).r + "&";
            }
            rulFileStream << rules.at(i).ifBlock.at(last).l + "=" + rules.at(i).ifBlock.at(last).r;
        }
        rulFileStream << "-";
        
        // Output Then-Block
        last = rules.at(i).thenBlock.length() - 1;
        if (last >= 0)
        {
            for (int j = 0; j < last; j++)
            {
                rulFileStream << rules.at(i).thenBlock.at(j).l + "=" + rules.at(i).thenBlock.at(j).r + "&";
            }
            rulFileStream << rules.at(i).thenBlock.at(last).l + "=" + rules.at(i).thenBlock.at(last).r;
        }
        rulFileStream << "\n";
        
    }
    rulFile.commit();
    // End saving rules
    
    saved = true;
}

const QString &Project::getProjName() const
{
    return projName;
}

const QStringList &Project::getVarNames() const
{
    return varNames;
}

const QList<QStringList> &Project::getVarValues() const
{
    return varValues;
}

const QStringList *Project::getVarValues(int varId) const
{
    normalizeVarId(&varId);
    if (!varExists(varId)) return nullptr;
    return (&varValues.at(varId));
}

const QStringList *Project::getVarValues(const QString &varName) const
{
    int varId = getVarId(varName);
    if (varId == -1) return nullptr;
    return getVarValues(varId);
}

const QList<Rule> &Project::getRules() const
{
    return rules;
}

const Rule *Project::getRule(int ruleId) const
{
    normalizeRuleId(&ruleId);
    if (!ruleExists(ruleId)) return nullptr;
    return (&rules.at(ruleId));
}

const Rule *Project::getRule(const QString &ruleStringified) const
{
    int ruleId = ruleStringified.section(')', 0, 0).toInt() - 1;
    if (!ruleExists(ruleId)) return nullptr;
    return getRule(ruleId);
}

QStringList Project::getRulezzStringified() const
{
    QStringList result;
    int length = rules.length();
    for (int i = 0; i < length; i++)
    {
        result.append(getRuleStringified(i));
    }
    return result;
}

QString Project::getRuleStringified(int ruleId) const
{
    normalizeRuleId(&ruleId);
    if (!ruleExists(ruleId)) return QString();
    return QString(zeros(ruleId + 1, rules.length() + 1) + QString::number(ruleId + 1) + ") " + rules.at(ruleId).stringify());
}

QString Project::getRuleStringified(const QString &ruleStringified) const
{
    int ruleId = ruleStringified.section(')', 0, 0).toInt() - 1;
    if (!ruleExists(ruleId)) return QString();
    return getRuleStringified(ruleId);
}

bool Project::isSaved() const
{
    return saved;
}

Error Project::addVar(const QString &varName, const QStringList &values)
{
    if (!isValid(varName)) return Error(ErrorCode::InvalidIdentifier);
    if (varExists(varName)) return Error(ErrorCode::IdentifierAlreadyExists);
    
    for (const QString &s : values)
    {
        if (!isValid(s)) return Error(ErrorCode::InvalidIdentifier);
    }
    varNames.append(varName);
    varValues.append(values);
    saved = false;
    return Error(ErrorCode::NoErrors);
}

Error Project::addVarValue(const QString &newValue, int varId)
{
    normalizeVarId(&varId);
    if (!varExists(varId)) return Error(ErrorCode::UnknownVariableId);
    if (!isValid(newValue)) return Error(ErrorCode::InvalidIdentifier);
    if (valueExists(varId, newValue)) return Error(ErrorCode::IdentifierAlreadyExists);
    
    varValues[varId].append(newValue);
    saved = false;
    return Error(ErrorCode::NoErrors);
}

Error Project::addVarValue(const QString &newValue, const QString &varName)
{
    int varId = getVarId(varName);
    if (varId == -1) return Error(ErrorCode::UnknownVariableName);
    return addVarValue(newValue, varId);
}

Error Project::deleteVar(int varId)
{
    normalizeVarId(&varId);
    if (!varExists(varId)) return Error(ErrorCode::UnknownVariableId);
    
    varNames.removeAt(varId);
    varValues.removeAt(varId);
    saved = false;
    return Error(ErrorCode::NoErrors);
}

Error Project::deleteVar(const QString &varName)
{
    int varId = getVarId(varName);
    if (varId == -1) return Error(ErrorCode::UnknownVariableName);
    return deleteVar(varId);
}

Error Project::deleteVarValue(int varId, int valueId)
{
    normalizeVarId(&varId);
    if (!varExists(varId)) return Error(ErrorCode::UnknownVariableId);
    normalizeValueId(varId, &valueId);
    if (!valueExists(varId, valueId)) return Error(ErrorCode::UnknownValueId);
    
    varValues[varId].removeAt(valueId);
    saved = false;
    return Error(ErrorCode::NoErrors);
}

Error Project::deleteVarValue(const QString &varName, const QString &valueName)
{
    int varId = getVarId(varName);
    if (varId == -1) return Error(ErrorCode::UnknownVariableName);
    int valueId = getValueId(varId, valueName);
    if (valueId == -1) return Error(ErrorCode::UnknownValueName);
    
    return deleteVarValue(varId, valueId);
}

Error Project::setVarName(const QString &newName, int varId)
{
    normalizeVarId(&varId);
    if (!varExists(varId)) return Error(ErrorCode::UnknownVariableId);
    if (!isValid(newName)) return Error(ErrorCode::InvalidIdentifier);
    if (varExists(newName)) return Error(ErrorCode::IdentifierAlreadyExists);
    
    varNames[varId] = newName;
    saved = false;
    return Error(ErrorCode::NoErrors);
}

Error Project::setVarName(const QString &newName, const QString &oldName)
{
    int varId = getVarId(oldName);
    if (varId == -1) return Error(ErrorCode::UnknownVariableName);
    return setVarName(newName, varId);
}

Error Project::setVarValue(const QString &newValue, int varId, int valueId)
{
    normalizeVarId(&varId);
    if (!varExists(varId)) return Error(ErrorCode::UnknownVariableId);
    normalizeValueId(varId, &valueId);
    if (!valueExists(varId, valueId)) return Error(ErrorCode::UnknownValueId);
    if (!isValid(newValue)) return Error(ErrorCode::InvalidIdentifier);
    if (valueExists(varId, newValue)) return Error(ErrorCode::IdentifierAlreadyExists);
    
    varValues[varId][valueId] = newValue;
    saved = false;
    return Error(ErrorCode::NoErrors);
}

Error Project::setVarValue(const QString &newValue, const QString &varName, const QString &oldValue)
{
    int varId = getVarId(varName);
    if (varId == -1) return Error(ErrorCode::UnknownVariableName);
    int valueId = getValueId(varId, oldValue);
    if (valueId == -1) return Error(ErrorCode::UnknownValueName);
    
    return setVarValue(newValue, varId, valueId);
}

Error Project::addRule(const Rule &rule)
{
    rules.append(rule);
    saved = false;
    return Error(ErrorCode::NoErrors);
}

Error Project::addIfPair(const Pair &ifPair, int ruleId)
{
    normalizeRuleId(&ruleId);
    if (!ruleExists(ruleId)) return Error(ErrorCode::UnknownRuleId);
    if (ifPairExists(ruleId, ifPair)) Error(ErrorCode::PairAlreadyExists);
    
    rules[ruleId].ifBlock.append(ifPair);
    saved = false;
    return Error(ErrorCode::NoErrors);
}

Error Project::addIfPair(const Pair &ifPair, const QString &ruleStringified)
{
    int ruleId = ruleStringified.section(')', 0, 0).toInt() - 1;
    if (!ruleExists(ruleId)) return Error(ErrorCode::UnknownRuleId);
    return addIfPair(ifPair, ruleId);
}

Error Project::addThenPair(const Pair &thenPair, int ruleId)
{
    normalizeRuleId(&ruleId);
    if (!ruleExists(ruleId)) return Error(ErrorCode::UnknownRuleId);
    if (thenPairExists(ruleId, thenPair)) Error(ErrorCode::PairAlreadyExists);
    
    rules[ruleId].thenBlock.append(thenPair);
    saved = false;
    return Error(ErrorCode::NoErrors);
}

Error Project::addThenPair(const Pair &thenPair, const QString &ruleStringified)
{
    int ruleId = ruleStringified.section(')', 0, 0).toInt() - 1;
    if (!ruleExists(ruleId)) return Error(ErrorCode::UnknownRuleId);
    return addThenPair(thenPair, ruleId);
}

Error Project::deleteRule(int ruleId)
{
    normalizeRuleId(&ruleId);
    if (!ruleExists(ruleId)) return Error(ErrorCode::UnknownRuleId);
    
    rules.removeAt(ruleId);
    saved = false;
    return Error(ErrorCode::NoErrors);
}

Error Project::deleteRule(const QString &ruleStringified)
{
    int ruleId = ruleStringified.section(')', 0, 0).toInt() - 1;
    if (!ruleExists(ruleId)) return Error(ErrorCode::UnknownRuleId);
    return deleteRule(ruleId);
}

Error Project::deleteIfPair(int ruleId, int ifPairId)
{
    normalizeRuleId(&ruleId);
    if (!ruleExists(ruleId)) return Error(ErrorCode::UnknownRuleId);
    normalizeIfPairId(ruleId, &ifPairId);
    if (!ifPairExists(ruleId, ifPairId)) return Error(ErrorCode::UnknownPairId);
    
    rules[ruleId].ifBlock.removeAt(ifPairId);
    saved = false;
    return Error(ErrorCode::NoErrors);
}

Error Project::deleteIfPair(const QString &ruleStringified, int ifPairId)
{
    int ruleId = ruleStringified.section(')', 0, 0).toInt() - 1;
    if (!ruleExists(ruleId)) return Error(ErrorCode::UnknownRuleId);
    return deleteIfPair(ruleId, ifPairId);
}

Error Project::deleteThenPair(int ruleId, int thenPairId)
{
    normalizeRuleId(&ruleId);
    if (!ruleExists(ruleId)) return Error(ErrorCode::UnknownRuleId);
    normalizeThenPairId(ruleId, &thenPairId);
    if (!thenPairExists(ruleId, thenPairId)) return Error(ErrorCode::UnknownPairId);
    
    rules[ruleId].thenBlock.removeAt(thenPairId);
    saved = false;
    return Error(ErrorCode::NoErrors);
}

Error Project::deleteThenPair(const QString &ruleStringified, int thenPairId)
{
    int ruleId = ruleStringified.section(')', 0, 0).toInt() - 1;
    if (!ruleExists(ruleId)) return Error(ErrorCode::UnknownRuleId);
    return deleteThenPair(ruleId, thenPairId);
}
