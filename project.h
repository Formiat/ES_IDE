#ifndef PROJECT_H
#define PROJECT_H

#include <memory>
#include <QString>
#include <QList>
#include <QStringList>
#include <QRegExp>


enum class ErrorCode : int
{
    NoErrors = 0,
    InvalidIdentifier,
    IdentifierAlreadyExists,
    UnknownVariableId,
    UnknownVariableName,
    UnknownValueId,
    UnknownValueName
};

struct Error
{
    Error(ErrorCode errCode);
    
    inline QString text() const
    {
        return (*this ? "Error code " + QString::number(static_cast<int>(errCode)) + ":\n" : "") + message;
    }
    
    inline operator bool() const
    {
        return (errCode != ErrorCode::NoErrors);
    }
    
private:
    ErrorCode errCode;
    QString message;
    
};

// Will be used to store pairs variable--value
struct Pair
{
    QString l; // Variable name
    QString r; // Variable value
};

struct Rule
{
    QList<Pair> ifBlock;
    QList<Pair> thenBlock;
};

class Project
{
public:
    // Constructor for Existing Project; we pass path to ".esp" file
    Project(const QString &projFilePath);
    // Constructor for New Project; we pass path to desired project folder
    Project(const QString &newProjFolderPath, const QString &projName);
    
    
    // Saves Project to its Files
    void saveProject() const;
    
    // Getters
    const QString &getProjName() const;
    const QStringList &getVarNames() const;
    const QList<QStringList> &getVarValues() const;
    
    // Returns "nullptr" if some errors occurred
    // "-1" means last added Variable Name
    const QStringList *getVarValues(int varId = -1) const;
    // Returns "nullptr" if some errors occurred
    const QStringList *getVarValues(const QString &varName) const;
    
    const QList<Rule> &getRules() const;
    
    bool isSaved() const;
    
    // Setters
    Error addVar(const QString &varName, const QStringList &values);
    // "-1" means last added Variable Name
    Error addVarValue(const QStringList &newValues, int varId = -1);
    Error addVarValue(const QStringList &newValues, const QString &varName);
    
    // "-1" means last added Variable Name
    Error deleteVar(int varId = -1);
    Error deleteVar(const QString &varName);
    // "-1" means last added Variable Name and Value respectively
    Error deleteVarValue(int varId = -1, int valueId = -1);
    Error deleteVarValue(const QString &varName, const QString &valueName);
    
    // "-1" means last added Variable Name
    Error setVarName(const QString &newName, int varId = -1);
    Error setVarName(const QString &newName, const QString &oldName);
    // "-1" means last added Variable Name and Value respectively
    Error setVarValue(const QString &newValue, int varId = -1, int valueId = -1);
    Error setVarValue(const QString &newValue, const QString &varName, const QString &oldValue);
    
private:
    inline bool isValid(const QString &name) const { return (regexpIdentifier.exactMatch(name)); }
    
    inline int getVarId(const QString &varName) const { return varNames.indexOf(varName); }
    inline int getValueId(int varId, const QString &valueName) const { return varValues.at(varId).indexOf(valueName); }
    
    inline bool varExists(int varId) const { return (varId < varNames.length() && varId >= 0); }    
    inline bool varExists(const QString &varName) const { return (varNames.indexOf(varName) != -1); }
    
    inline bool valueExists(int varId, const QString &valueName) const { return (varValues.at(varId).indexOf(valueName) != -1); }
    inline bool valueExists(int varId, int valueId) const { return (valueId < varValues.at(varId).length() && valueId >= 0); }
    
    inline void normalize(int *varId) const { if (*varId == -1) *varId = varNames.length() - 1; }
    inline void normalize(int varId, int *valueId) const { if (*valueId == -1) *valueId = varValues.at(varId).length() - 1; }
    
private:
    QString projName;
    
    QString projFilePath;
    QString varFilePath;
    QString rulFilePath;
    
    QStringList varNames;
    QList<QStringList> varValues;
    QList<Rule> rules;
    
    QRegExp regexpIdentifier;
    
    mutable bool saved;
    
};

#endif // PROJECT_H
