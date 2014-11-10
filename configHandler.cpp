
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
    m_qfswConfigFile = NULL;
    changeBasePath(strPath);
}

void ConfigHandler::changeBasePath(QString strPath)
{
    m_strBasePath = strPath;
    readConfigJson();
    //setFileSystemWatcher();
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

QJsonArray ConfigHandler::getImagesJArray()
{
    return m_jsonMasterObj["resources"].toObject().value("media").toObject()["image"].toArray();
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

void ConfigHandler::setImagesJArray(QJsonArray array)
{
    QJsonObject obj = m_jsonMasterObj.find("resources").value().toObject();
    QJsonObject mediaObj = obj["media"].toObject();
    mediaObj["image"] = array;
    obj["media"] = mediaObj;
    m_jsonMasterObj["resources"] = obj;
}

void ConfigHandler::newInteractivityCreated(QJsonObject newObject)
{
    m_jsonMasterObj = newObject;
    bool b_configUpdate = m_bStopUpdating;
    m_bStopUpdating = false;
    changeLoadStepOfFiles();
    writeConfigJson();
    m_bStopUpdating = b_configUpdate;
}

void ConfigHandler::changeLoadStepOfFiles()
{
    // Set CSS file's next step load to false which is the first entry in css object.
    m_jsonMasterObj["resources"].toObject()["css"].toArray().at(0).toObject()["isNextStepLoad"] = false;

    // Set JS files' next step load to false.
    m_jsonMasterObj["resources"].toObject()["js"].toArray().at(0).toObject()["isNextStepLoad"] = false; // initialize.js
    m_jsonMasterObj["resources"].toObject()["js"].toArray().at(1).toObject()["isNextStepLoad"] = false; // models/model.js
    m_jsonMasterObj["resources"].toObject()["js"].toArray().at(1).toObject()["isNextStepLoad"] = false; // views/view.js

    // Set overview TEMPLATE entry next step load to false.
    m_jsonMasterObj["resources"].toObject()["templates"].toArray().at(0).toObject()["isNextStepLoad"] = false; // templates/overview.js
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
        return;
    }
    QByteArray rawData = m_fileConfigJson.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(rawData));
    m_jsonMasterObj = doc.object();
    // qDebug() << "READ : " << masterJObj;
    m_fileConfigJson.close();
}

void ConfigHandler::setFileSystemWatcher()
{
    if(m_qfswConfigFile != NULL)
    {
        disconnect(m_qfswConfigFile,SIGNAL(fileChanged(QString)),this,SLOT(configFileChanged(QString)));
        delete m_qfswConfigFile;
    }
    m_qfswConfigFile = new QFileSystemWatcher(this);
    connect(m_qfswConfigFile,SIGNAL(fileChanged(QString)),this,SLOT(configFileChanged(QString)));
    m_qfswConfigFile->addPath(getConfigJSONFilePath());
}

void ConfigHandler::configFileChanged(QString filepath)
{
    qDebug() << "event fired";
    emit configManuallyModified(m_strBasePath);
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
    changeLoadStepOfFiles();
    QJsonDocument doc(m_jsonMasterObj);
    m_fileConfigJson.write(doc.toJson());
    m_fileConfigJson.close();
}

QList<QString> ConfigHandler::getComponentList()
{
    QList<QString> components;
    QJsonArray jArray_component = m_jsonMasterObj["resources"].toObject()["components"].toArray();
    for(int i = 0 ;i < jArray_component.count(); i++)
    {
        components.append(jArray_component.at(i).toString());
    }
    return components;
}

void ConfigHandler::setComponentList(QList<QString> components)
{
    QJsonArray jarray;
    for(int i = 0 ; i < components.length();i++)
    {
        jarray.append(components.at(i));
    }

    QJsonObject jObj_resources = m_jsonMasterObj["resources"].toObject();
    jObj_resources["components"] = jarray;
    m_jsonMasterObj["resources"] = jObj_resources;
    qDebug() << jarray;
}

QString ConfigHandler::getIdPrefix()
{
    return m_jsonMasterObj["config"].toObject().value("idPrefix").toString();
}

QString ConfigHandler::getModuleName()
{
    return m_jsonMasterObj["config"].toObject().value("module").toString();
}

ConfigHandler::~ConfigHandler()
{

}

