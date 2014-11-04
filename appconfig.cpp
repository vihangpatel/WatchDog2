#include "appconfig.h"
#include <QDebug>

const QString APP_CONFIG_FILE = "appconfig.json";

AppConfig::AppConfig(QObject *parent) :
    QObject(parent)
{
    m_fileAppConfig.setFileName(getConfigFileName());
    if(!m_fileAppConfig.exists())
    {
        //qDebug() << "New Config has been generated";
        m_fileAppConfig.open(QIODevice::ReadWrite | QIODevice::Text);
        m_fileAppConfig.close();
    }
    readSettings();
}

void AppConfig::setRootPath(QString strRootPath)
{
    m_jsonMasterObj["root"] = strRootPath;
    writeSettings();
}

QString AppConfig::getRootPath()
{
    return m_jsonMasterObj.value("root").toString();
}

void AppConfig::setCurrentInteractivity(QString intrctiveName)
{
    m_jsonMasterObj["currentFolder"] = intrctiveName;
    writeSettings();
}

void AppConfig::setFlags(int flags)
{
    m_bflagStatus = flags;
}

bool AppConfig::monitorJS()
{
       return (m_bflagStatus & JS_TRUE) == JS_TRUE;
}

bool AppConfig::monitorCSS()
{
       return (m_bflagStatus & CSS_TRUE) == CSS_TRUE;
}

bool AppConfig::monitorMedia()
{
    return (m_bflagStatus & MEDIA_TRUE) == MEDIA_TRUE;
}

bool AppConfig::monitorTemplates()
{     
     return (m_bflagStatus & TEMPLATE_TRUE) == TEMPLATE_TRUE;
}

bool AppConfig::monitorConfig()
{

    return (m_bflagStatus & CONFIG_STOP) == CONFIG_STOP;
}

void AppConfig::setJSFlag(bool isTrue)
{
    if(isTrue)
    {
        m_bflagStatus = m_bflagStatus | JS_TRUE;
    }  
}

void AppConfig::setCSSFlag(bool isTrue)
{
    if(isTrue)
    {
        m_bflagStatus = m_bflagStatus | CSS_TRUE;
    }   
}

void AppConfig::setTemplateFlag(bool isTrue)
{
    if(isTrue)
    {
        m_bflagStatus = m_bflagStatus | TEMPLATE_TRUE;
    }  
}

void AppConfig::setMediaFlag(bool isTrue)
{
    if(isTrue)
    {
        m_bflagStatus = m_bflagStatus | MEDIA_TRUE;
    }    
}

void AppConfig::setConfigModificationFlag(bool isStop)
{
    if(isStop)
    {
        m_bflagStatus = m_bflagStatus | CONFIG_STOP;
    }
}

void AppConfig::setShowTipsOnStartup(bool show)
{
    if(show)
    {
        m_bflagStatus = m_bflagStatus | SHOW_TIPS;
    }
}

void AppConfig::setTipIndex(int index)
{
    tipIndex = index;
    m_jsonMasterObj["currentTipIndex"] = QString::number(index);
}

int AppConfig::getTipIndex()
{
    return m_jsonMasterObj.value("currentTipIndex").toString().toInt();
}

bool AppConfig::showTipsOnStartup()
{
    return (m_bflagStatus & SHOW_TIPS) == SHOW_TIPS;
}

QString AppConfig::getCurrentInteractivity()
{
    return m_jsonMasterObj.value("currentFolder").toString();
}


QString AppConfig::getConfigFileName()
{
     QString strCurrentPath = QDir::currentPath();
     return strCurrentPath + "/" + APP_CONFIG_FILE;
}

void AppConfig::resetFlag()
{
    m_bflagStatus = 0;
}

void AppConfig::setStartUpLaunch(bool launch)
{
    if(launch)
    {
        m_bflagStatus = m_bflagStatus | LAUNCH_ON_STARTUP;
    }
}

bool AppConfig::startUpLaunch()
{
    return (m_bflagStatus & LAUNCH_ON_STARTUP) == LAUNCH_ON_STARTUP;
}

void AppConfig::readSettings()
{
    m_fileAppConfig.setFileName(getConfigFileName());
    m_fileAppConfig.open(QIODevice::ReadOnly | QIODevice::Text);
    if(!m_fileAppConfig.exists())
    {
        return;
    }
    QByteArray rawData = m_fileAppConfig.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(rawData));
    m_jsonMasterObj = doc.object();
    m_bflagStatus = m_jsonMasterObj["flags"].toDouble();
  //qDebug() << "READ : " << m_bflagStatus;
    m_fileAppConfig.close();
}

void AppConfig::writeSettings()
{
    m_fileAppConfig.setFileName(getConfigFileName());
    m_fileAppConfig.remove();
    m_fileAppConfig.open(QIODevice::ReadWrite | QIODevice::Text);
    if(!m_fileAppConfig.exists())
    {
        // qDebug() << " LOC ACC DO NOT EXIST . NEW WILL BE CREATED :" << getLocAccFilePath();
    }
    m_jsonMasterObj.insert("flags",m_bflagStatus);
    //qDebug() << "WRITE : \n" << m_jsonMasterObj;
    QJsonDocument doc(m_jsonMasterObj);
    m_fileAppConfig.write(doc.toJson());
    m_fileAppConfig.close();
}

AppConfig::~AppConfig()
{

}
