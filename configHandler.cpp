
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
    str_basePath = strPath;
    readConfigJson();
}

QString ConfigHandler::getConfigJSONFilePath()
{
    return str_basePath + "/" +  C_DATA_FOLDER + "/" + C_CONFIG_FILE_NAME;
}

QJsonObject ConfigHandler::getConfigModule()
{
    return masterJObj["config"].toObject();
}

QJsonArray ConfigHandler::getCssJArray()
{
    return masterJObj["resources"].toObject().value("css").toArray();
}

QJsonArray ConfigHandler::getJSJArray()
{
    return masterJObj["resources"].toObject().value("js").toArray();
}

QJsonArray ConfigHandler::getTemplateJArray()
{
    return masterJObj["resources"].toObject().value("templates").toArray();
}

void ConfigHandler::setTemplateJArray(QJsonArray array)
{
    QJsonObject obj = masterJObj.find("resources").value().toObject();
    obj["templates"] = array;
    masterJObj["resources"] = obj;
}

void ConfigHandler::setConfigModule(QJsonObject jObject)
{
    masterJObj["config"] = jObject;
}

void ConfigHandler::setCssJArray(QJsonArray array)
{
    QJsonObject obj = masterJObj.find("resources").value().toObject();
    obj["css"] = array;
    masterJObj["resources"] = obj;
}

void ConfigHandler::setJSJArray(QJsonArray array)
{
    QJsonObject obj = masterJObj.find("resources").value().toObject();
    obj["js"] = array;
    masterJObj["resources"] = obj;
}

void ConfigHandler::newInteractivityCreated(QJsonObject newObject)
{
    masterJObj = newObject;
    writeConfigJson();
}

void ConfigHandler::readConfigJson()
{
    configJsonFIle.setFileName(getConfigJSONFilePath());
    configJsonFIle.open(QIODevice::ReadOnly | QIODevice::Text);
    if(!configJsonFIle.exists())
    {
        // qDebug() << " CONFIG READING FAILED :" << getConfigJSONFilePath();
       // QMessageBox::critical(this,"Config File Not Found","Config File doesn't exist",QMessageBox::Cancel);
    }
    QByteArray rawData = configJsonFIle.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(rawData));
    masterJObj = doc.object();
    // qDebug() << "READ : " << masterJObj;
    configJsonFIle.close();
}

void ConfigHandler::writeConfigJson()
{
    configJsonFIle.setFileName(getConfigJSONFilePath());
    configJsonFIle.remove();
    configJsonFIle.open(QIODevice::ReadWrite | QIODevice::Text);
    if(!configJsonFIle.exists())
    {
        // qDebug() << " CONFIG FAILED :" << getConfigJSONFilePath();
    }
    // qDebug() << "WRITE : \n" << masterJObj;
    QJsonDocument doc(masterJObj);
    configJsonFIle.write(doc.toJson());
    configJsonFIle.close();
}
