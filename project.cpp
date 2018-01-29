#include "project.h"

#include <QFile>
#include <QIODevice>
#include <QStringList>
#include <QDebug>
#include <QDir>
#include <QSaveFile>


Error::Error(ErrorCode errCode) : errCode(errCode)
{
    switch (errCode) {
    case ErrorCode::NoErrors:
        message = "No Errors.";
        break;
    case ErrorCode::InvalidIdentifier:
        message = "Invalid Identifier: Identifier must consist only of letters, digits and underscores, and must not begin with a digit.";
        break;
    case ErrorCode::IdentifierAlreadyExists:
        message = "Identifier already exists.";
        break;
    case ErrorCode::UnknownVariableId:
        message = "Unknown Variable Id.";
        break;
    case ErrorCode::UnknownVariableName:
        message = "Unknown Variable Name.";
        break;
    case ErrorCode::UnknownValueId:
        message = "Unknown Value Id.";
        break;
    case ErrorCode::UnknownValueName:
        message = "Unknown Value Name.";
        break;
    }
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
            Pair p{pair.at(0), pair.at(1)};
            r.ifBlock.append(p);
        }
        
        QStringList thenPart = line.at(1).split("&");
        for (int i = 0; i < thenPart.length(); i++)
        {
            QStringList pair = thenPart.at(i).split("=");
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
    QSaveFile varFile(varFilePath);
    varFile.open(QSaveFile::WriteOnly | QSaveFile::Truncate);
    QTextStream varFileStream(&varFile);
    
    for (int i = 0; i < varNames.length(); i++)
    {
        varFileStream << varNames.at(i);
        if (varValues.at(i).length() > 0)
        {
            varFileStream << ".";
            for (int j = 0; j < varValues.at(i).length() - 1; j++)
            {
                varFileStream << varValues.at(i).at(j) + ".";
            }
            varFileStream << varValues.at(i).at(varValues.at(i).length() - 1);
        }
        varFileStream << "\n";
    }
    varFile.commit();
    
    QSaveFile rulFile(rulFilePath);
    rulFile.open(QSaveFile::WriteOnly | QSaveFile::Truncate);
    QTextStream rulFileStream(&rulFile);
    
    for (int i = 0; i < rules.length(); i++)
    {
        // Output If-Block
        for (int j = 0; j < rules.at(i).ifBlock.length() - 1; j++)
        {
            rulFileStream << rules.at(i).ifBlock.at(j).l + "=" + rules.at(i).ifBlock.at(j).r + "&";
        }
        rulFileStream <<
                         rules.at(i).ifBlock.at(rules.at(i).ifBlock.length() - 1).l + "="
                         + rules.at(i).ifBlock.at(rules.at(i).ifBlock.length() - 1).r + "-";
        
        // Output Then-Block
        for (int j = 0; j < rules.at(i).thenBlock.length() - 1; j++)
        {
            rulFileStream << rules.at(i).thenBlock.at(j).l + "=" + rules.at(i).thenBlock.at(j).r + "&";
        }
        rulFileStream <<
                         rules.at(i).thenBlock.at(rules.at(i).thenBlock.length() - 1).l + "="
                         + rules.at(i).thenBlock.at(rules.at(i).thenBlock.length() - 1).r + "\n";
        
    }
    rulFile.commit();
    
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
    normalize(&varId);
    if (varExists(varId)) return (&varValues.at(varId));
    return nullptr;
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

Error Project::addVarValue(const QStringList &newValues, int varId)
{
    normalize(&varId);
    if (!varExists(varId)) return Error(ErrorCode::UnknownVariableId);
    
    for (const QString &s : newValues)
    {
        if (!isValid(s)) return Error(ErrorCode::InvalidIdentifier);
        if (valueExists(varId, s)) return Error(ErrorCode::IdentifierAlreadyExists);
    }
    varValues[varId].append(newValues);
    saved = false;
    return Error(ErrorCode::NoErrors);
}

Error Project::addVarValue(const QStringList &newValues, const QString &varName)
{
    int varId = getVarId(varName);
    if (varId == -1) return Error(ErrorCode::UnknownVariableName);
    return addVarValue(newValues, varId);
}

Error Project::deleteVar(int varId)
{
    normalize(&varId);
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
    normalize(&varId);
    if (!varExists(varId)) return Error(ErrorCode::UnknownVariableId);
    normalize(varId, &valueId);
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
    normalize(&varId);
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
    normalize(&varId);
    if (!varExists(varId)) return Error(ErrorCode::UnknownVariableId);
    normalize(varId, &valueId);
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
