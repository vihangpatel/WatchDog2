#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QObject>
#include <QFile>
#include <QDir>
#include <QJsonObject>
#include <QJsonDocument>



class AppConfig : public QObject
{
    Q_OBJECT
public:
    explicit AppConfig(QObject *parent = 0);
    ~AppConfig();
    const int JS_TRUE = 1;
    const int TEMPLATE_TRUE = 2;
    const int CSS_TRUE = 4;
    const int MEDIA_TRUE = 8;
    const int CONFIG_STOP = 16;
    const int SHOW_TIPS = 32;
    const int LAUNCH_ON_STARTUP = 64;
    const int AUTO_COMPILE_ON = 128;
    int tipIndex = 0;
    QString str_rootPath;
    QString m_strCurIntrctvPath;
    QFile m_fileAppConfig;
    QJsonObject m_jsonMasterObj;
    int m_bflagStatus = 0;

    void readSettings();
    void resetFlag();
    void writeSettings();
    void setRootPath(QString strRootPath);
    QString getRootPath();
    void setCurrentInteractivity(QString intrctiveName);
    QString getCurrentInteractivity();
    QString getConfigFileName();

    void setFlags(int m_bflagStatus);
    bool monitorJS();
    bool monitorCSS();
    bool monitorTemplates();
    bool monitorMedia();
    bool monitorConfig();
    bool showTipsOnStartup();
    bool startUpLaunch();
    int getTipIndex();
    bool getAutoCompile();
    
    void setJSFlag(bool isTrue);
    void setCSSFlag(bool isTrue);
    void setTemplateFlag(bool isTrue);
    void setMediaFlag(bool isTrue);
    void setConfigModificationFlag(bool isStop);
    void setShowTipsOnStartup(bool show);
    void setTipIndex(int index);
    void setStartUpLaunch(bool launch);
    void setAutoCompile(bool on);


signals:
    
public slots:
    
};

#endif // APPCONFIG_H
