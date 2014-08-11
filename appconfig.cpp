#include "appconfig.h"
#include <QDebug>

const QString APP_CONFIG_FILE = "appconfig.json";

AppConfig::AppConfig(QObject *parent) :
    QObject(parent)
{
    fileAppConfig.setFileName(getConfigFileName());
    if(!fileAppConfig.exists())
    {
        qDebug() << "New Config has been generated";
        fileAppConfig.open(QIODevice::ReadWrite | QIODevice::Text);
        fileAppConfig.close();
    }
    readSettings();
}

void AppConfig::setRootPath(QString strRootPath)
{
    masterJObj["root"] = strRootPath;
    writeSettings();
}

QString AppConfig::getRootPath()
{
    return masterJObj.value("root").toString();
}

void AppConfig::setCurrentInteractivity(QString intrctiveName)
{
    masterJObj["currentFolder"] = intrctiveName;
    writeSettings();
}

void AppConfig::setFlags(int flags)
{
    flagStatus = flags;
}

bool AppConfig::monitorJS()
{
       return (flagStatus & JS_TRUE) == JS_TRUE;
}

bool AppConfig::monitorCSS()
{
       return (flagStatus & CSS_TRUE) == CSS_TRUE;
}

bool AppConfig::monitorMedia()
{
    return (flagStatus & MEDIA_TRUE) == MEDIA_TRUE;
}

bool AppConfig::monitorTemplates()
{     
     return (flagStatus & TEMPLATE_TRUE) == TEMPLATE_TRUE;
}

void AppConfig::setJSFlag(bool isTrue)
{
    if(isTrue)
    {
        flagStatus = flagStatus | JS_TRUE;
    }  
}

void AppConfig::setCSSFlag(bool isTrue)
{
    if(isTrue)
    {
        flagStatus = flagStatus | CSS_TRUE;
    }   
}

void AppConfig::setTemplateFlag(bool isTrue)
{
    if(isTrue)
    {
        flagStatus = flagStatus | TEMPLATE_TRUE;
    }  
}

void AppConfig::setMediaFlag(bool isTrue)
{
    if(isTrue)
    {
        flagStatus = flagStatus | MEDIA_TRUE;
    }    
}

QString AppConfig::getCurrentInteractivity()
{
    return masterJObj.value("currentFolder").toString();
}


QString AppConfig::getConfigFileName()
{
     QString strCurrentPath = QDir::currentPath();
     return strCurrentPath + "/" + APP_CONFIG_FILE;
}

void AppConfig::resetFlag()
{
    flagStatus = 0;
}

void AppConfig::readSettings()
{
    fileAppConfig.setFileName(getConfigFileName());
    fileAppConfig.open(QIODevice::ReadOnly | QIODevice::Text);
    if(!fileAppConfig.exists())
    {
        return;
    }
    QByteArray rawData = fileAppConfig.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(rawData));
    masterJObj = doc.object();
    flagStatus = masterJObj["flags"].toDouble();
     qDebug() << "READ : " << flagStatus;
    fileAppConfig.close();
}

void AppConfig::writeSettings()
{
    fileAppConfig.setFileName(getConfigFileName());
    fileAppConfig.remove();
    fileAppConfig.open(QIODevice::ReadWrite | QIODevice::Text);
    if(!fileAppConfig.exists())
    {
        // qDebug() << " LOC ACC DO NOT EXIST . NEW WILL BE CREATED :" << getLocAccFilePath();
    }
    masterJObj.insert("flags",flagStatus);
    qDebug() << "WRITE : \n" << masterJObj;
    QJsonDocument doc(masterJObj);
    fileAppConfig.write(doc.toJson());
    fileAppConfig.close();
}
