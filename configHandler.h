#ifndef CONFIGHANDLER_H
#define CONFIGHANDLER_H

#include <QFileSystemWatcher>
#include <QStringList>
#include <QObject>
#include <QDir>
#include <QProcess>
#include <QFileInfoList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>

class ConfigHandler  : public  QObject{
    Q_OBJECT
public:
    bool m_bConfigLoaded;
    QFile m_fileConfigJson;
    QJsonObject m_jsonMasterObj;
    QString m_strBasePath;
    bool m_bStopUpdating;

    explicit ConfigHandler(QString);
    ~ConfigHandler();
    void changeBasePath(QString);
    QJsonArray getTemplateJArray();
    QJsonArray getJSJArray();
    QJsonArray getCssJArray();
    QJsonObject getConfigModule();

    void setTemplateJArray(QJsonArray);
    void setJSJArray(QJsonArray);
    void setCssJArray(QJsonArray);
    void setConfigModule(QJsonObject);
    void setConfigUpdateFlag(bool stopUpdate);

    QString getConfigJSONFilePath();

    void changeLoadStepOfFiles();
    void readConfigJson();
    void writeConfigJson();

public slots:
    void newInteractivityCreated(QJsonObject);

};

#endif // CONFIGHANDLER_H
