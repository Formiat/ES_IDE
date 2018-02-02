#ifndef PROJECT_H
#define PROJECT_H

#include <memory>
#include <QString>
#include <QList>
#include <QStringList>
#include <QRegExp>
#include <QCoreApplication>

//#include <cmath>


enum class ErrorCode : int
{
    NoErrors = 0,
    InvalidIdentifier,
    IdentifierAlreadyExists,
    UnknownVariableId,
    UnknownVariableName,
    UnknownValueId,
    UnknownValueName,
    UnknownRuleId,
    UnknownPairId,
    PairAlreadyExists
};

struct Error
{
public:
    Error(ErrorCode errCode);
    
    inline QString text() const
    {
        return (*this ? QCoreApplication::translate("Error", "Error code ") + QString::number(static_cast<int>(errCode)) + (":\n") : ("")) + message;
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
    Pair();
    Pair(const QString &l, const QString &r);
    
    inline bool operator==(const Pair &pair) const { return (l == pair.l && r == pair.r); }
    
    QString l; // Variable name
    QString r; // Variable value
};

struct Rule
{
    QList<Pair> ifBlock;
    QList<Pair> thenBlock;
    
    QString stringify() const;
    QString stringifyIfBlock() const;
    QString stringifyThenBlock() const;
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
    const Rule *getRule(int ruleId = -1) const;
    const Rule *getRule(const QString &ruleStringified) const;
    
    // Returns all Rules Stringified
    QStringList getRulezzStringified() const;
    // "-1" means last added Rule
    QString getRuleStringified(int ruleId = -1) const;
    // Used to get rule representation after changes
    QString getRuleStringified(const QString &ruleStringified) const;
    
    bool isSaved() const;
    
    
    // Setters
    
    // Variables
    
    Error addVar(const QString &varName, const QStringList &values);
    // "-1" means last added Variable Name
    Error addVarValue(const QString &newValue, int varId = -1);
    Error addVarValue(const QString &newValue, const QString &varName);
    
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
    
    
    // Rules
    
    Error addRule(const Rule &rule);
    // "-1" means last added Rule
    Error addIfPair(const Pair &ifPair, int ruleId = -1);
    Error addIfPair(const Pair &ifPair, const QString &ruleStringified);
    // "-1" means last added Rule
    Error addThenPair(const Pair &thenPair, int ruleId = -1);
    Error addThenPair(const Pair &thenPair, const QString &ruleStringified);
    
    // "-1" means last added Rule
    Error deleteRule(int ruleId = -1);
    Error deleteRule(const QString &ruleStringified);
    // "-1" means last added Rule and Pair respectively
    Error deleteIfPair(int ruleId = -1, int ifPairId = -1);
    // "-1" means last added Pair respectively
    Error deleteIfPair(const QString &ruleStringified, int ifPairId = -1);
    // "-1" means last added Rule and Pair respectively
    Error deleteThenPair(int ruleId = -1, int thenPairId = -1);
    // "-1" means last added Pair respectively
    Error deleteThenPair(const QString &ruleStringified, int thenPairId = -1);
    
    
private:
    inline bool isValid(const QString &name) const { return regexpIdentifier.exactMatch(name); }
    
    inline int getVarId(const QString &varName) const { return varNames.indexOf(varName); }
    inline int getValueId(int varId, const QString &valueName) const { return varValues.at(varId).indexOf(valueName); }
    
    inline bool varExists(int varId) const { return (varId < varNames.length() && varId >= 0); }    
    inline bool varExists(const QString &varName) const { return varNames.contains(varName); }
    
    inline bool valueExists(int varId, int valueId) const { return (valueId < varValues.at(varId).length() && valueId >= 0); }
    inline bool valueExists(int varId, const QString &valueName) const { return varValues.at(varId).contains(valueName); }
    
    inline void normalizeVarId(int *varId) const { if (*varId == -1) *varId = varNames.length() - 1; }
    inline void normalizeValueId(int varId, int *valueId) const { if (*valueId == -1) *valueId = varValues.at(varId).length() - 1; }
    
    inline bool ruleExists(int ruleId) const { return (ruleId < rules.length() && ruleId >= 0); }

    inline bool ifPairExists(int ruleId, int ifPairId) const { return (ifPairId < rules.at(ruleId).ifBlock.length() && ifPairId >= 0); }
    inline bool ifPairExists(int ruleId, const Pair &ifPair) const { return rules.at(ruleId).ifBlock.contains(ifPair); }
    
    inline bool thenPairExists(int ruleId, int thenPairId) const { return (thenPairId < rules.at(ruleId).thenBlock.length() && thenPairId >= 0); }
    inline bool thenPairExists(int ruleId, const Pair &thenPair) const { return rules.at(ruleId).thenBlock.contains(thenPair); }
    
    inline void normalizeRuleId(int *ruleId) const { if (*ruleId == -1) *ruleId = rules.length() - 1; }
    inline void normalizeIfPairId(int ruleId, int *ifPairId) const { if (*ifPairId == -1) *ifPairId = rules.at(ruleId).ifBlock.length() - 1; }
    inline void normalizeThenPairId(int ruleId, int *thenPairId) const { if (*thenPairId == -1) *thenPairId = rules.at(ruleId).thenBlock.length() - 1; }
    
    
    inline QString zeros(int num, int maxNum) const
    {
        QString result;
        int zerosNum = static_cast<int>(log10(maxNum)) - static_cast<int>(log10(num));
        for (int i = 0; i < zerosNum; i++)
        {
            result.append(" ");
        }
        return result;
    }
    
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
