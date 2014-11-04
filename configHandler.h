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
    QFileSystemWatcher *m_qfswConfigFile;
    bool m_bStopUpdating;

    explicit ConfigHandler(QString);
    ~ConfigHandler();
    void changeBasePath(QString);
    QJsonArray getTemplateJArray();
    QJsonArray getJSJArray();
    QJsonArray getImagesJArray();
    QJsonArray getCssJArray();
    QJsonObject getConfigModule();
    QList<QString> getComponentList();

    void setTemplateJArray(QJsonArray);
    void setJSJArray(QJsonArray);
    void setImagesJArray(QJsonArray);
    void setCssJArray(QJsonArray);
    void setConfigModule(QJsonObject);
    void setComponentList(QList<QString> components);
    void setConfigUpdateFlag(bool stopUpdate);

    QString getConfigJSONFilePath();    

    void changeLoadStepOfFiles();
    void readConfigJson();
    void writeConfigJson();
    void setFileSystemWatcher();

public slots:
    void newInteractivityCreated(QJsonObject);
    void configFileChanged(QString filepath);

signals:
    void configManuallyModified(QString filePath);
};

#endif // CONFIGHANDLER_H
