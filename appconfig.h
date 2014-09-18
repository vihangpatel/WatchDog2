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
    const int JS_TRUE = 1;
    const int TEMPLATE_TRUE = 2;
    const int CSS_TRUE = 4;
    const int MEDIA_TRUE = 8;
    const int CONFIG_STOP = 16;
    QString str_rootPath;
    QString str_curIntrctvPath;
    QFile fileAppConfig;
    QJsonObject masterJObj;
    int flagStatus = 0;

    void readSettings();
    void resetFlag();
    void writeSettings();
    void setRootPath(QString strRootPath);
    QString getRootPath();
    void setCurrentInteractivity(QString intrctiveName);
    QString getCurrentInteractivity();
    QString getConfigFileName();

    void setFlags(int flagStatus);
    bool monitorJS();
    bool monitorCSS();
    bool monitorTemplates();
    bool monitorMedia();
    bool monitorConfig();
    
    void setJSFlag(bool isTrue);
    void setCSSFlag(bool isTrue);
    void setTemplateFlag(bool isTrue);
    void setMediaFlag(bool isTrue);
    void setConfigModificationFlag(bool isStop);

signals:
    
public slots:
    
};

#endif // APPCONFIG_H
