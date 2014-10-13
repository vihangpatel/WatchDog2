#include "components.h"
#include <QDebug>
#include <QStringList>
#include <QJsonDocument>

QString PLAYER_CONFIG_PATH = "data/player-config.json";

components::components(QString basePath)
{
    changeBasePath(basePath);
}

void components::changeBasePath(QString basePath)
{
    m_strBasePath = basePath;
    readPlayerConfigFile();
}

void components::setCommonPath(QString commonPath)
{
    m_strCommonPath = commonPath;
    readPlayerConfigFile();
}

QString components::getPlayerConfigPath()
{
    return m_strCommonPath + "/" + PLAYER_CONFIG_PATH;
}

QList<QString> components::getUnusedComponentList()
{
    filterComponentList();
    return m_unusedCompList;
}

QList<QString> components::getUsedComponentList()
{
    return m_usedCompList;
}

void components::useComponent(int componentId)
{
    QString str_removedComponent = m_unusedCompList.at(componentId);
    m_usedCompList.append(str_removedComponent);
    m_unusedCompList.removeAt(componentId);
    emit componentsChanged();
}

void components::removeComponent(int componentId)
{
    QString str_removedComponent = m_usedCompList.at(componentId);
    m_unusedCompList.append(str_removedComponent);
    m_usedCompList.removeAt(componentId);
    emit componentsChanged();
}

void components::setUsedComponentList(QList<QString> usedComponent)
{
    m_usedCompList = usedComponent;
    filterComponentList();
    emit componentsChanged();
}

void components::filterComponentList()
{
    for(int i = 0 ; i < m_usedCompList.length() ; i++)
    {
        for(int j = 0 ; j < m_unusedCompList.length() ;j++)
        {
            if(m_usedCompList.at(i) == m_unusedCompList.at(j))
            {
                m_unusedCompList.removeAt(j);
                break;
            }
        }

    }
}

void components::readPlayerConfigFile()
{
    QFile file_playerConfig(getPlayerConfigPath());
    if(!file_playerConfig.exists())
    {
        qDebug() << "player config file does not exists";
        return;
    }
    file_playerConfig.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray rawData = file_playerConfig.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(rawData));
    QJsonObject m_jsonMasterObj = doc.object();
    QJsonObject jObj_resources = m_jsonMasterObj["resources"].toObject();
    QJsonObject jObj_components = jObj_resources["components"].toObject();
    QStringList componentsKeys = jObj_components.keys();
    QString currentKey ;
    for(int i = 0 ; i < componentsKeys.length() ; i++)
    {
        currentKey = componentsKeys.at(i);
        QJsonObject currentComponent = jObj_components[currentKey].toObject();
        if(currentComponent["componentType"].toInt() == 0)
        {
            continue;
        }
        m_unusedCompList.append(currentKey);
    }
    file_playerConfig.close();
}

