#include "locacc.h"

QString LOC_LANG_FOLDER = "lang";
QString LOC_EN_FOLDER = "en";
QString LOC_DATA_FOLDER = "data";
QString LOC_FILE_NAME = "loc-acc.json";

LOCACC::LOCACC(QString strPath)
{
    QStringList treeList;
    treeList << "locAccData";
    root = new QTreeWidgetItem(treeList);
    changeBasePath(strPath);
}

void LOCACC::changeBasePath(QString strPath)
{
    str_basePath  = strPath;
    readFile();
    emptyTreeWidget(root);
    getLocAccTree();
}

void LOCACC::emptyTreeWidget(QTreeWidgetItem *parent)
{
    QTreeWidgetItem *child;
    for(int i = 0 ; i < parent->childCount(); i++)
    {
        child = parent->child(i);
        emptyTreeWidget(child);
        parent->removeChild(child);
        delete child;
    }
}

void LOCACC::addScreen(QString screenID , QString screenName)
{
    QJsonObject newJObjScreen;
    newJObjScreen["id"] = screenID;
    newJObjScreen["name"] = screenName;
    QJsonArray tempJArray;
    newJObjScreen["elements"] = tempJArray;
    QJsonArray locDataJArray = masterJObj["locAccData"].toArray();
    locDataJArray.append(newJObjScreen);
    masterJObj["locAccData"] = locDataJArray;
    QStringList newScreen;
    newScreen << screenID ;
    QTreeWidgetItem *newScreenWidget = new QTreeWidgetItem(newScreen);
    root->addChild(newScreenWidget);
    writeFile();
}

QTreeWidgetItem * LOCACC::getLocAccTree()
{
    QJsonArray locAccArray = masterJObj["locAccData"].toArray();
        QJsonObject screenJObj;
        for(int i = 0 ; i < locAccArray.count() ; i++)
        {
            screenJObj = locAccArray.at(i).toObject();
            root->addChild(getScreenTree(screenJObj));
        }
    return root;
}

QTreeWidgetItem* LOCACC::getScreenTree(QJsonObject screenJObj)
{
    QJsonDocument doc(screenJObj);
    QString str_toolTip(doc.toJson());
    QStringList treeList;
    treeList << screenJObj["id"].toString();
    QTreeWidgetItem *screenItemWidget = new QTreeWidgetItem(treeList);
    screenItemWidget->addChildren(getElementsTree(screenJObj));
    screenItemWidget->setToolTip(0,str_toolTip);
    return screenItemWidget;
}

QList<QTreeWidgetItem *> LOCACC::getElementsTree(QJsonObject screenJObj)
{
    QStringList treeList;
    QJsonObject eleJObject;
    QJsonDocument doc;
    QList<QTreeWidgetItem *> eleTreeItemlist;
    QJsonArray eleJArray = screenJObj["elements"].toArray();
    for(int i = 0 ; i < eleJArray.count(); i++)
    {
        treeList.clear();
        eleJObject = eleJArray.at(i).toObject();
        treeList << eleJObject["id"].toString();
        QTreeWidgetItem *ele = new QTreeWidgetItem(treeList);
        qDebug() << eleJArray.at(i).toString();
        eleTreeItemlist.append(ele);

        doc.setObject(eleJObject);
        QString str_toolTip(doc.toJson());
        ele->setToolTip(0,str_toolTip);
    }
    return eleTreeItemlist;
}

void LOCACC::addElement(QString parentScreen, QString elementName)
{
    QJsonArray jArray = masterJObj["locAccData"].toArray();
    QJsonObject tempObj ;
    for(int i = 0 ; i < jArray.count() ; i++ )
    {
        tempObj = jArray.at(i).toObject();
        if(tempObj["id"] == parentScreen)
        {
            QJsonArray eleJArray = tempObj["elements"].toArray();
            eleJArray.append(getElementJson(elementName));
            tempObj["elements"] = eleJArray;
            jArray.replace(i,tempObj);
            break;
        }
    }
    masterJObj["locAccData"] = jArray;
    writeFile();
}

QJsonObject LOCACC::getElementJson(QString elementName)
{
    QJsonArray tempMsgArray;
    QJsonObject eleObj ;
    eleObj["id"] = elementName;
    eleObj["accId"] = elementName;
    eleObj["type"] = QString("text");
    eleObj["tabIndex"] = QString("-1") ;
    eleObj["messages"] = tempMsgArray;
    return eleObj;
}

void LOCACC::addMessage(QString parentScreen, QString eleName, QString msgId, QString locMessage, QString accMessage , bool isAccTextSame)
{

}

QString LOCACC :: getLocAccFilePath()
{
    return str_basePath + "/" + LOC_LANG_FOLDER
            + "/" + LOC_EN_FOLDER +  + "/" + LOC_DATA_FOLDER  + "/" + LOC_FILE_NAME;
}

void LOCACC::readFile()
{
    locAccFile.setFileName(getLocAccFilePath());
    locAccFile.open(QIODevice::ReadOnly | QIODevice::Text);
    if(!locAccFile.exists())
    {
        qDebug() << " LOC ACC READING FAILED :" << getLocAccFilePath();
    }
    QByteArray rawData = locAccFile.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(rawData));
    masterJObj = doc.object();
    qDebug() << "READ : " << masterJObj;
}

void LOCACC :: writeFile()
{
    QString filePath = getLocAccFilePath();
    locAccFile.setFileName(filePath);
    locAccFile.remove();
    locAccFile.open(QIODevice::ReadWrite | QIODevice::Text);
    if(!locAccFile.exists())
    {
        qDebug() << " LOC ACC FAILED :" << getLocAccFilePath();
    }
    qDebug() << "WRITE : \n" << masterJObj;
    QJsonDocument doc(masterJObj);
    locAccFile.write(doc.toJson());
    locAccFile.close();
}
