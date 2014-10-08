
#include "configHandler.h"
#include <QDebug>
#include "templates.h"

QString C_TEMPLATE_FOLDER = "templates";
QString C_JS_FOLDER = "js";
QString C_JS_VIEW_FOLDER = "views";
QString C_JS_MODEL_FOLDER = "models";
QString C_CSS_FOLDER = "css";
QString C_DATA_FOLDER = "data";
QString  C_LANG_FOLDER = "lang";
QString C_MEDIA_FOLDER = "media";
QString C_EN_FOLDER = "en";
QString C_CONFIG_FILE_NAME = "interactivity-config.json";

ConfigHandler::ConfigHandler(QString strPath)
{
    changeBasePath(strPath);
}

void ConfigHandler::changeBasePath(QString strPath)
{
    m_strBasePath = strPath;
    readConfigJson();
}

QString ConfigHandler::getConfigJSONFilePath()
{
    return m_strBasePath + "/" +  C_DATA_FOLDER + "/" + C_CONFIG_FILE_NAME;
}

QJsonObject ConfigHandler::getConfigModule()
{
    return m_jsonMasterObj["config"].toObject();
}

QJsonArray ConfigHandler::getCssJArray()
{
    return m_jsonMasterObj["resources"].toObject().value("css").toArray();
}

QJsonArray ConfigHandler::getJSJArray()
{
    return m_jsonMasterObj["resources"].toObject().value("js").toArray();
}

QJsonArray ConfigHandler::getTemplateJArray()
{
    return m_jsonMasterObj["resources"].toObject().value("templates").toArray();
}

void ConfigHandler::setTemplateJArray(QJsonArray array)
{
    QJsonObject obj = m_jsonMasterObj.find("resources").value().toObject();
    obj["templates"] = array;
    m_jsonMasterObj["resources"] = obj;
}

void ConfigHandler::setConfigModule(QJsonObject jObject)
{
    m_jsonMasterObj["config"] = jObject;
}

void ConfigHandler::setCssJArray(QJsonArray array)
{
    QJsonObject obj = m_jsonMasterObj.find("resources").value().toObject();
    obj["css"] = array;
    m_jsonMasterObj["resources"] = obj;
}

void ConfigHandler::setJSJArray(QJsonArray array)
{
    QJsonObject obj = m_jsonMasterObj.find("resources").value().toObject();
    obj["js"] = array;
    m_jsonMasterObj["resources"] = obj;
}

void ConfigHandler::newInteractivityCreated(QJsonObject newObject)
{
    m_jsonMasterObj = newObject;
    bool b_configUpdate = m_bStopUpdating;
    m_bStopUpdating = false;
    writeConfigJson();
    m_bStopUpdating = b_configUpdate;
}

void ConfigHandler::setConfigUpdateFlag(bool stopUpdate)
{
    m_bStopUpdating = stopUpdate;
}

void ConfigHandler::readConfigJson()
{
    m_fileConfigJson.setFileName(getConfigJSONFilePath());
    m_fileConfigJson.open(QIODevice::ReadOnly | QIODevice::Text);
    if(!m_fileConfigJson.exists())
    {
        // qDebug() << " CONFIG READING FAILED :" << getConfigJSONFilePath();
       // QMessageBox::critical(this,"Config File Not Found","Config File doesn't exist",QMessageBox::Cancel);
    }
    QByteArray rawData = m_fileConfigJson.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(rawData));
    m_jsonMasterObj = doc.object();
    // qDebug() << "READ : " << masterJObj;
    m_fileConfigJson.close();
}

void ConfigHandler::writeConfigJson()
{
    if(m_bStopUpdating)
    {
        return;
    }
    m_fileConfigJson.setFileName(getConfigJSONFilePath());
    m_fileConfigJson.remove();
    m_fileConfigJson.open(QIODevice::ReadWrite | QIODevice::Text);
    if(!m_fileConfigJson.exists())
    {
        // qDebug() << " CONFIG FAILED :" << getConfigJSONFilePath();
    }
    // qDebug() << "WRITE : \n" << masterJObj;
    QJsonDocument doc(m_jsonMasterObj);
    m_fileConfigJson.write(doc.toJson());
    m_fileConfigJson.close();
}
