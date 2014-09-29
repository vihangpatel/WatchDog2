#include "locacc.h"

QString LOC_LANG_FOLDER = "lang";
QString LOC_EN_FOLDER = "en";
QString LOC_DATA_FOLDER = "data";
QString LOC_FILE_NAME = "loc-acc.json";

LOCACC::LOCACC(QString strPath)
{
    QStringList treeList;
    treeList << "locAccData";
    m_dirLang = new QDir("D:\DE");
    m_qtwiRoot = new QTreeWidgetItem(treeList);
    changeBasePath(strPath);
}

void LOCACC::changeBasePath(QString strPath)
{
    m_strBasePath  = strPath;
    m_dirLang->setPath(getLangFolderPath());
    getAvailableLangugaes();
    emptyTreeWidget(m_qtwiRoot);
    readFile();
    m_qtwiRoot->takeChildren();
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

QTreeWidgetItem* LOCACC::addScreen(QStringList screenData)
{
    QJsonObject newJObjScreen;

    if(screenExistance(screenData,m_jsonMasterObj["locAccData"].toArray()))
    {
        qDebug() << "screen can not be added";
        return NULL;
    }
    newJObjScreen["id"] = screenData.at(0);
    newJObjScreen["name"] = screenData.at(1);
    QJsonArray tempJArray;
    newJObjScreen["elements"] = tempJArray;
    QJsonArray locDataJArray = m_jsonMasterObj["locAccData"].toArray();
    locDataJArray.append(newJObjScreen);
    m_jsonMasterObj["locAccData"] = locDataJArray;
    QStringList newScreen;
    newScreen << screenData.at(0); ;
    QTreeWidgetItem *newScreenWidget = new QTreeWidgetItem(newScreen);
    m_qtwiRoot->addChild(newScreenWidget);
    writeFile();
    return newScreenWidget;
}

QTreeWidgetItem* LOCACC::addElement(QStringList elementData, QTreeWidgetItem *parent)
{
    QJsonArray jArray = m_jsonMasterObj["locAccData"].toArray();
    QString parentScreen = parent->text(0);
    QJsonObject tempObj ;
    QTreeWidgetItem *newEleWidget;
    for(int i = 0 ; i < jArray.count() ; i++ )
    {
        tempObj = jArray.at(i).toObject();
        if(tempObj["id"] == parentScreen)
        {
            QJsonArray eleJArray = tempObj["elements"].toArray();
            if(messageExistance(elementData,eleJArray))
            {
                return false;
            }
            QJsonObject newEleObj = getElementJson(elementData);
            eleJArray.append(newEleObj);
            tempObj["elements"] = eleJArray;
            jArray.replace(i,tempObj);

            QStringList strList;
            strList << elementData;
            newEleWidget  = new QTreeWidgetItem(strList);
            parent->addChild(newEleWidget);
            break;
        }
    }
    m_jsonMasterObj["locAccData"] = jArray;
    writeFile();
    return newEleWidget;
}

QTreeWidgetItem* LOCACC::addMessage(QStringList msgData, bool isAccTextSame, QTreeWidgetItem *parent)
{
    QString eleId = parent->text(0);
    QJsonArray jArray = m_jsonMasterObj["locAccData"].toArray();
    QString parentScreen = parent->parent()->text(0);
    QJsonObject tempObj ;
    QTreeWidgetItem *newMsg;
    for(int i = 0 ; i < jArray.count() ; i++ )
    {
        tempObj = jArray.at(i).toObject();
        if(tempObj["id"] == parentScreen)
        {
            QJsonArray eleJArray = tempObj["elements"].toArray();
            QJsonObject tempEleObj;
            for(int j = 0 ; j < eleJArray.count(); j++)
            {
                tempEleObj = eleJArray.at(j).toObject();
                if(tempEleObj["id"] == eleId)
                {
                    QJsonArray msgsArray = tempEleObj["messages"].toArray();
                    if(messageExistance(msgData,msgsArray))
                    {
                        return false;
                    }
                    QJsonObject newMsgJson = getMessageJson(msgData,isAccTextSame);
                    msgsArray.append(newMsgJson);
                    QStringList msgList;
                    msgList << msgData.at(0);
                    newMsg  = new QTreeWidgetItem(msgList);
                    setMessageTooltip(newMsg,newMsgJson);
                    parent->addChild(newMsg);
                    tempEleObj["messages"] = msgsArray;
                    eleJArray.replace(j,tempEleObj);
                    tempObj["elements"] = eleJArray;
                    jArray.replace(i,tempObj);
                    break;
                }
            }
        }
    }
    m_jsonMasterObj["locAccData"] = jArray;
    writeFile();
    return newMsg;
}

QJsonObject LOCACC::getMessageJson(QStringList msgData,bool isAccTextSame)
{
    QJsonObject newMsgObj;
    newMsgObj["id"] = msgData.at(0);
    newMsgObj["isAccTextSame"] = isAccTextSame;
    QJsonObject locAccObj;
    locAccObj["loc"] = msgData.at(1);
    locAccObj["acc"] = msgData.at(2);
    newMsgObj["message"] = locAccObj;
    return newMsgObj;
}

QJsonObject LOCACC::fetchScreenJObject(QStringList screenData)
{
    QString screenId = screenData.at(0);
    QJsonArray locArray = m_jsonMasterObj["locAccData"].toArray();
    QJsonObject tempObj;
    for(int i = 0 ; i < locArray.count() ; i++)
    {
        tempObj = locArray.at(i).toObject();
        if(tempObj["id"] == screenId)
        {
            return tempObj;
        }
    }
    return tempObj;
}

QJsonObject LOCACC::fetchElementJObject(QStringList elementData, QJsonArray parentScreenJArray)
{
    QString eleId = elementData.at(0);
    QJsonObject tempObj;
    for(int i = 0 ; i < parentScreenJArray.count(); i++)
    {
        tempObj = parentScreenJArray.at(i).toObject();
        if(tempObj["id"] == eleId)
        {
            return tempObj;
        }
    }
    return tempObj;
}

QJsonObject LOCACC::fetchMessageJObject(QStringList messageData,QJsonArray msgsArray)
{
    QString msgId = messageData.at(0);
    QJsonObject tempObj;
    for(int i = 0 ; i < msgsArray.count(); i++)
    {
        tempObj = msgsArray.at(i).toObject();
        if(tempObj["id"] == msgId)
        {
            return tempObj;
        }
    }
    return tempObj;
}

bool LOCACC::screenExistance(QStringList screenData, QJsonArray locArray)
{
    QString screenId = screenData.at(0);
    QJsonObject tempObj;
    for(int i = 0 ; i < locArray.count() ; i++)
    {
        tempObj = locArray.at(i).toObject();
        if(tempObj["id"] == screenId)
        {
            return true;
        }
    }
    return false;
}

bool LOCACC::elementExistance(QStringList elementData, QJsonArray parentScreenJArray)
{
    QString eleId = elementData.at(0);
    QJsonObject tempObj;
    for(int i = 0 ; i < parentScreenJArray.count(); i++)
    {
        tempObj = parentScreenJArray.at(i).toObject();
        if(tempObj["id"] == eleId)
        {
            return true;
        }
    }
    return false;
}

bool LOCACC::messageExistance(QStringList messageData, QJsonArray msgsArray)
{
    QString msgId = messageData.at(0);
    QJsonObject tempObj;
    for(int i = 0 ; i < msgsArray.count(); i++)
    {
        tempObj = msgsArray.at(i).toObject();
        if(tempObj["id"] == msgId)
        {
            return true;
        }
    }
    return false;
}

QTreeWidgetItem * LOCACC::getLocAccTree()
{
    QJsonArray locAccArray = m_jsonMasterObj["locAccData"].toArray();
    QJsonObject screenJObj;
    for(int i = 0 ; i < locAccArray.count() ; i++)
    {
        screenJObj = locAccArray.at(i).toObject();
        m_qtwiRoot->addChild(generateScreenTree(screenJObj));
    }
    return m_qtwiRoot;
}

QTreeWidgetItem* LOCACC::generateScreenTree(QJsonObject screenJObj)
{
    QStringList treeList;
    treeList << screenJObj["id"].toString();
    QTreeWidgetItem *screenItemWidget = new QTreeWidgetItem(treeList);
    screenItemWidget->addChildren(generateElementsTree(screenJObj));
    return screenItemWidget;
}

QList<QTreeWidgetItem *> LOCACC::generateElementsTree(QJsonObject screenJObj)
{
    QStringList treeList;
    QJsonObject eleJObject;
    QList<QTreeWidgetItem *> eleTreeItemlist;
    QJsonArray eleJArray = screenJObj["elements"].toArray();
    for(int i = 0 ; i < eleJArray.count(); i++)
    {
        treeList.clear();
        eleJObject = eleJArray.at(i).toObject();
        treeList << eleJObject["id"].toString();
        QTreeWidgetItem *ele = new QTreeWidgetItem(treeList);
        // qDebug() << eleJArray.at(i).toString();
        eleTreeItemlist.append(ele);
        ele->addChildren(generateMessageTree(eleJObject["messages"].toArray()));
    }
    return eleTreeItemlist;
}

QList<QTreeWidgetItem *> LOCACC::generateMessageTree(QJsonArray  msgArray)
{
    QList<QTreeWidgetItem *> msgChildWidgetList;
    QJsonObject msgObj;
    QStringList msgList;
    // qDebug() << " in  message generation : " << msgArray;
    for(int i = 0 ; i < msgArray.count() ; i++)
    {
        msgList.clear();
        msgObj = msgArray.at(i).toObject();
        // qDebug() << " in  message generation : loop  " << msgObj;
        msgList << msgObj["id"].toString();
        QTreeWidgetItem *msgChild = new QTreeWidgetItem(msgList);
        setMessageTooltip(msgChild,msgObj);
        msgChildWidgetList.append(msgChild);
    }
    return msgChildWidgetList;
}

QJsonObject LOCACC::getElementJson(QStringList elementData)
{
    QJsonArray tempMsgArray;
    QJsonObject eleObj ;
    eleObj["id"] = elementData.at(0);
    eleObj["accId"] =  elementData.at(1);
    eleObj["type"] = elementData.at(2);
    eleObj["role"] =  elementData.at(3);
    eleObj["tabIndex"] =  elementData.at(4);
    eleObj["messages"] = tempMsgArray;
    return eleObj;
}

/**************************************************************
 *  D E L E T E    M E S S A G E   D A T A
**************************************************************/

bool LOCACC::deleteScreen(QTreeWidgetItem *currentItem)
{
    QString screenId = currentItem->text(0);
    QJsonArray locArray = m_jsonMasterObj["locAccData"].toArray();
    QJsonObject tempObj;
    for(int i = 0 ; i < locArray.count() ; i++)
    {
        tempObj = locArray.at(i).toObject();
        if(tempObj["id"] == screenId)
        {
            locArray.removeAt(i);
            break;
        }
    }
    m_jsonMasterObj["locAccData"] = locArray;
    currentItem->parent()->removeChild(currentItem);
    emptyTreeWidget(currentItem);
    writeFile();
    return true;
}

bool LOCACC::deleteElement(QTreeWidgetItem *currentItem)
{
    QString elementId = currentItem->text(0);
    QJsonArray jArray = m_jsonMasterObj["locAccData"].toArray();
    QString parentScreen = currentItem->parent()->text(0);
    QJsonObject tempObj ;
    for(int i = 0 ; i < jArray.count() ; i++ )
    {
        tempObj = jArray.at(i).toObject();
        if(tempObj["id"] == parentScreen)
        {
            QJsonArray eleJArray = tempObj["elements"].toArray();
            QJsonObject eleObject ;
            for(int j = 0 ; j < eleJArray.count() ; j++)
            {
                eleObject = eleJArray.at(j).toObject();
                if(eleObject["id"] == elementId)
                {
                    eleJArray.removeAt(j);
                    tempObj["elements"] = eleJArray;
                    jArray.replace(i,tempObj);
                    break;
                }
            }
        }
    }
    m_jsonMasterObj["locAccData"] = jArray;
    currentItem->parent()->removeChild(currentItem);
    emptyTreeWidget(currentItem);
    writeFile();
    return true;
}

bool LOCACC::deleteMessage(QTreeWidgetItem *currentItem)
{
    QString messageId = currentItem->text(0);
    QString eleId = currentItem->parent()->text(0);
    QJsonArray jArray = m_jsonMasterObj["locAccData"].toArray();
    QString parentScreen = currentItem->parent()->parent()->text(0);
    QJsonObject tempObj ;
    for(int i = 0 ; i < jArray.count() ; i++ )
    {
        tempObj = jArray.at(i).toObject();
        if(tempObj["id"] == parentScreen)
        {
            QJsonArray eleJArray = tempObj["elements"].toArray();
            QJsonObject tempEleObj;
            for(int j = 0 ; j < eleJArray.count(); j++)
            {
                tempEleObj = eleJArray.at(j).toObject();
                if(tempEleObj["id"] == eleId)
                {
                    QJsonArray msgsArray = tempEleObj["messages"].toArray();
                    QJsonObject tempMsgObj;
                    for(int k = 0; k < msgsArray.count() ; k++)
                    {
                        tempMsgObj = msgsArray.at(k).toObject();
                        if(tempMsgObj["id"] == messageId)
                        {
                            msgsArray.removeAt(k);
                            tempEleObj["messages"] = msgsArray;
                            eleJArray.replace(j,tempEleObj);
                            tempObj["elements"] = eleJArray;
                            jArray.replace(i,tempObj);
                            break;
                        }
                    }
                }
            }
        }
    }
    m_jsonMasterObj["locAccData"] = jArray;
    currentItem->parent()->removeChild(currentItem);
    emptyTreeWidget(currentItem);
    writeFile();
    return true;
}

/**************************************************************
 *  G E T   M E S S A G E   D A T A
**************************************************************/

QStringList LOCACC::getScreenTreeData(QTreeWidgetItem *elementItem)
{
    QJsonObject screenObj = fetchScreenJObject(QStringList(elementItem->text(0)));
    QStringList screenData;
    screenData << screenObj["id"].toString() << screenObj["name"].toString() ;
    return screenData;
}

QStringList LOCACC :: getElementTreeData(QTreeWidgetItem *elementItem)
{
    QJsonObject screenObj = fetchScreenJObject(QStringList(elementItem->parent()->text(0)));
    QJsonObject elementObj = fetchElementJObject(QStringList (elementItem->text(0)),screenObj["elements"].toArray());
    QStringList elementData;
    elementData << elementObj["id"].toString()  << elementObj["accId"].toString()  << elementObj["type"].toString()
            << elementObj["role"].toString()
            << elementObj["tabIndex"].toString() ;
    return elementData;
}

QStringList LOCACC::getMessageTreeData(QTreeWidgetItem *messageItem)
{

    QJsonObject screenObj = fetchScreenJObject(QStringList(messageItem->parent()->parent()->text(0)));
    QJsonObject elementObj = fetchElementJObject(QStringList(messageItem->parent()->text(0)),screenObj["elements"].toArray());
    QJsonObject messageObj = fetchMessageJObject(QStringList(messageItem->text(0)),elementObj["messages"].toArray());

    QStringList messageData ;
    messageData << messageObj["id"].toString()  << (messageObj["isAccTextSame"].toBool() ? "true" : "false")
            << messageObj["message"].toObject().value("loc").toString()
            <<  messageObj["message"].toObject().value("acc").toString()  ;
    return messageData;
}

/**************************************************************
 *  U P D A T E    M E S S A G E   D A T A
**************************************************************/

bool LOCACC::updateScreen(QStringList newScreenData, QTreeWidgetItem *currentItem)
{
    QString screenId = currentItem->text(0);
    QJsonArray locArray = m_jsonMasterObj["locAccData"].toArray();
    QJsonObject tempObj;
    for(int i = 0 ; i < locArray.count() ; i++)
    {
        tempObj = locArray.at(i).toObject();
        if(tempObj["id"] == screenId)
        {
            locArray.removeAt(i);
            if(screenExistance(newScreenData,locArray))
            {
                return false;
            }
            tempObj["id"] = newScreenData.at(0);
            tempObj["name"] = newScreenData.at(1);
            locArray.insert(i,tempObj);
            break;
        }
    }
    m_jsonMasterObj["locAccData"] = locArray;
    currentItem->setText(0,newScreenData.at(0));
    writeFile();
    return true;
}

bool LOCACC::updateElement(QStringList newElementData, QTreeWidgetItem *currentItem)
{
    QString elementId = currentItem->text(0);
    QJsonArray jArray = m_jsonMasterObj["locAccData"].toArray();
    QString parentScreen = currentItem->parent()->text(0);
    QJsonObject tempObj ;
    for(int i = 0 ; i < jArray.count() ; i++ )
    {
        tempObj = jArray.at(i).toObject();
        if(tempObj["id"] == parentScreen)
        {
            QJsonArray eleJArray = tempObj["elements"].toArray();
            QJsonObject eleObject ;
            for(int j = 0 ; j < eleJArray.count() ; j++)
            {
                eleObject = eleJArray.at(j).toObject();
                if(eleObject["id"] == elementId)
                {
                    eleJArray.removeAt(j);
                    if(elementExistance(newElementData,eleJArray))
                    {
                        return false;
                    }
                    QJsonObject newEleJson = getElementJson(newElementData);
                    newEleJson["messages"] = eleObject["messages"];
                    eleJArray.insert(j,newEleJson);
                    tempObj["elements"] = eleJArray;
                    jArray.replace(i,tempObj);
                    break;
                }
            }
        }
    }
    m_jsonMasterObj["locAccData"] = jArray;
    currentItem->setText(0,newElementData.at(0));
    writeFile();
    return true;
}

bool LOCACC::updateMessage(QStringList newMessageData, bool isAccTextSame, QTreeWidgetItem *currentItem)
{
    QString messageId = currentItem->text(0);
    QString eleId = currentItem->parent()->text(0);
    QJsonArray jArray = m_jsonMasterObj["locAccData"].toArray();
    QString parentScreen = currentItem->parent()->parent()->text(0);
    QJsonObject tempObj ;
    for(int i = 0 ; i < jArray.count() ; i++ )
    {
        tempObj = jArray.at(i).toObject();
        if(tempObj["id"] == parentScreen)
        {
            QJsonArray eleJArray = tempObj["elements"].toArray();
            QJsonObject tempEleObj;
            for(int j = 0 ; j < eleJArray.count(); j++)
            {
                tempEleObj = eleJArray.at(j).toObject();
                if(tempEleObj["id"] == eleId)
                {
                    QJsonArray msgsArray = tempEleObj["messages"].toArray();
                    QJsonObject tempMsgObj;
                    for(int k = 0; k < msgsArray.count() ; k++)
                    {
                        tempMsgObj = msgsArray.at(k).toObject();
                        if(tempMsgObj["id"] == messageId)
                        {
                            msgsArray.removeAt(k);
                            if(messageExistance(newMessageData,msgsArray))
                            {
                                return false;
                            }
                            QJsonObject newMsgObj = getMessageJson(newMessageData,isAccTextSame);
                            msgsArray.insert(k,newMsgObj);
                            tempEleObj["messages"] = msgsArray;
                            eleJArray.replace(j,tempEleObj);
                            tempObj["elements"] = eleJArray;
                            jArray.replace(i,tempObj);
                            setMessageTooltip(currentItem,newMsgObj);
                            break;
                        }
                    }
                }
            }
        }
    }
    m_jsonMasterObj["locAccData"] = jArray;
    currentItem->setText(0,newMessageData.at(0));
    writeFile();
    return true;
}

/***********************************************
 * S E A R C H   R E S U L T   F U N C T I O N
 **********************************************/

QList<QTreeWidgetItem *> LOCACC::getSearchResult(QString searchText)
{
    QList<QTreeWidgetItem *> searchedResult;
    QTreeWidgetItem *currentScreenItem, *currentElementItem , *currentMessageItem;
    QStringList msgStringList;
    for(int i = 0 ; i < m_qtwiRoot->childCount() ; i++)
    {
        currentScreenItem = m_qtwiRoot->child(i);
        // qDebug() << "TOP : " << currentScreenItem->text(0);
        for( int j = 0 ; j < currentScreenItem->childCount() ; j++)
        {
            currentElementItem = currentScreenItem->child(j);
            // qDebug() << "ELE : " << currentElementItem->text(0);
            for(int  k = 0 ; k < currentElementItem->childCount() ; k++)
            {
                currentMessageItem = currentElementItem->child(k);
                msgStringList = getMessageTreeData(currentMessageItem);

                if(msgStringList.at(2).contains(searchText,Qt::CaseInsensitive)
                        || msgStringList.at(3).contains(searchText,Qt::CaseInsensitive))
                {
                    searchedResult.append(currentMessageItem);
                }

            }
        }
    }
    m_listSearchedResult = searchedResult;
    m_iCurrentSearchIndex = searchedResult.length() > 0 ? 0 : -1;
    return searchedResult;
}

QTreeWidgetItem *LOCACC::getCurrentSearchResult()
{
    return m_iCurrentSearchIndex > 0 && m_iCurrentSearchIndex < m_listSearchedResult.length() ? m_listSearchedResult.at(m_iCurrentSearchIndex) : NULL;
}

QTreeWidgetItem *LOCACC::getPrevSeachResult()
{
    m_iCurrentSearchIndex =  m_iCurrentSearchIndex > 0 ? m_iCurrentSearchIndex - 1 : 0;
    return m_listSearchedResult.length() > m_iCurrentSearchIndex ?
                m_listSearchedResult.at(m_iCurrentSearchIndex) : NULL;
}

QTreeWidgetItem *LOCACC::getNextSearchResult()
{
    m_iCurrentSearchIndex =  m_iCurrentSearchIndex < m_listSearchedResult.length() - 1  ? m_iCurrentSearchIndex + 1 : m_listSearchedResult.length();
    return m_listSearchedResult.length() > m_iCurrentSearchIndex ?
                m_listSearchedResult.at(m_iCurrentSearchIndex) :  NULL;
}

int LOCACC::getCurrentSearchIndex()
{
    return m_iCurrentSearchIndex;
}

bool LOCACC::changeOrder(int currentIndex, int newIndex, QTreeWidgetItem *item)
{
      qDebug() << "informal :" << currentIndex  ;
    if (item && ((currentIndex < newIndex &&  currentIndex < m_qtwiRoot->childCount() - 1)
                  ||  (currentIndex > 0 && currentIndex > newIndex)))
    {
        // Visual Update
        qDebug() << currentIndex  ;
       QList<QTreeWidgetItem *> items = m_qtwiRoot->takeChildren();
       items.removeAt(currentIndex);
       items.insert(newIndex,item);
       m_qtwiRoot->insertChildren(0,items);

        // Actual Update
       QJsonArray locAccArray = m_jsonMasterObj.find("locAccData").value().toArray();
       QJsonObject objectToRemove = locAccArray.at(currentIndex).toObject();
       locAccArray.removeAt(currentIndex);
       locAccArray.insert(newIndex,objectToRemove);
       m_jsonMasterObj["locAccData"] = locAccArray;
       //writeFile();  // Do not update file on change in order.
       return true;
    }
    return false;
}

/***********************************************************************
 ************************  T O O L T I P     T E X T **************************
 **********************************************************************/

void LOCACC::setMessageTooltip(QTreeWidgetItem *messageItem, QJsonObject messageJObj)
{
    QJsonDocument doc(messageJObj);
    QString text(doc.toJson());
    messageItem->setToolTip(0,text);
}

/***********************************************************************
 ********************* A D D   N E W   L A N G U A G E **********************
 **********************************************************************/

bool LOCACC::addNewLanguage(QString lang)
{
   bool contains =  m_strListAvailableLang.contains(lang,Qt::CaseSensitive);
   if(contains)
   {
        return false;
   }
   m_dirLang->setPath(getLangFolderPath());
   makeNewLangFolder(lang);
   m_strListAvailableLang.append(lang);
   return true;
}

void LOCACC::makeNewLangFolder(QString newLang)
{
    QString newLangFolderPath = m_strBasePath + "/" +  LOC_LANG_FOLDER + "/" + newLang + "/" + LOC_DATA_FOLDER;
    QDir dir(newLangFolderPath);
    if(!dir.exists())
    {
        m_dirLang->mkpath(newLangFolderPath);
    }
    QFile file(newLangFolderPath + "/" + LOC_FILE_NAME);
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    file.close();
}

QStringList LOCACC::getAvailableLangugaes()
{
    m_strListAvailableLang.clear();
    m_iCurrentLangIndex = 0;
    m_strListAvailableLang = m_dirLang->entryList();
    m_strListAvailableLang.removeOne(".");
    m_strListAvailableLang.removeOne("..");
    // qDebug() << availableLangList;
    return m_strListAvailableLang;
}

bool LOCACC::changeLanguage(int currentIndex)
{
    m_iCurrentLangIndex = m_iCurrentLangIndex > m_strListAvailableLang.count() ?  0 :  currentIndex;
    // qDebug() << currentLangIndex;
    emptyTreeWidget(m_qtwiRoot);
    readFile();
    m_qtwiRoot->takeChildren();
    getLocAccTree();
}

QString LOCACC :: getLocAccFilePath()
{
    QString lang;
    lang = m_iCurrentLangIndex >= m_strListAvailableLang.length() ? LOC_EN_FOLDER :  m_strListAvailableLang.at(m_iCurrentLangIndex);
    return m_strBasePath + "/" + LOC_LANG_FOLDER
            + "/"  +   lang
            + "/" + LOC_DATA_FOLDER  + "/" + LOC_FILE_NAME;
}

QString LOCACC::getLangFolderPath()
{
    return m_strBasePath + "/" + LOC_LANG_FOLDER;
}

void LOCACC::readFile()
{
    m_fileLocAcc.setFileName(getLocAccFilePath());
    m_fileLocAcc.close();
    m_fileLocAcc.open(QIODevice::ReadOnly | QIODevice::Text);
    if(!m_fileLocAcc.exists())
    {
        // qDebug() << " LOC ACC READING FAILED :" << getLocAccFilePath();
        return;
    }
    // qDebug() << " LOC ACC READING  :" << getLocAccFilePath();
    QByteArray rawData = m_fileLocAcc.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(rawData));
    m_jsonMasterObj = doc.object();
    // qDebug() << "READ : " << masterJObj;
    m_fileLocAcc.close();
}

void LOCACC :: writeFile()
{
    QString filePath = getLocAccFilePath();
    m_fileLocAcc.setFileName(filePath);
    m_fileLocAcc.remove();
    m_fileLocAcc.open(QIODevice::ReadWrite | QIODevice::Text);
    if(!m_fileLocAcc.exists())
    {
        // qDebug() << " LOC ACC DO NOT EXIST . NEW WILL BE CREATED :" << getLocAccFilePath();
    }
    // qDebug() << "WRITE : \n" << masterJObj;
    QJsonDocument doc(m_jsonMasterObj);
    m_fileLocAcc.write(doc.toJson());
    m_fileLocAcc.close();
}

QTreeWidgetItem* LOCACC :: cloneScreen(QTreeWidgetItem *itemToClone)
{
    QStringList stringData = getScreenTreeData(itemToClone);
    QTreeWidgetItem *clonedScreen ;
    int counter = 0;
    QStringList newStringData(stringData);

    do
    {
        clonedScreen = addScreen(newStringData);
        counter++;
        newStringData[0] = stringData.at(0) + "-copy (" + QString::number(counter) + ")";
        newStringData[1] = stringData.at(1) + "-copy (" + QString::number(counter) + ")";
    }
    while (clonedScreen==NULL);

    int elementCount = itemToClone->childCount();
    for(int i = 0 ; i < elementCount ; i++)
    {
        QTreeWidgetItem *currentElement = itemToClone->child(i);
        cloneElement(currentElement,clonedScreen);
    }
    return clonedScreen;
}

QTreeWidgetItem* LOCACC :: cloneElement(QTreeWidgetItem *itemToClone,QTreeWidgetItem *parent)
{
    QStringList stringData = getElementTreeData(itemToClone);
    QStringList newStringData(stringData);
    QTreeWidgetItem *clonedElement;
    int counter = 0;
    do
    {
        clonedElement = addElement(newStringData,parent);
        counter++;
        newStringData[0] = stringData[0] + "-copy (" + QString::number(counter) + ")";
        newStringData[1] = stringData[1] + "-copy (" + QString::number(counter) + ")";
    }
    while(clonedElement == NULL);

    int msgCount = itemToClone->childCount();
    for(int i = 0 ; i < msgCount ; i++)
    {
        QTreeWidgetItem *currentMsg = itemToClone->child(i);
        cloneMessage(currentMsg,clonedElement);
    }
    return clonedElement;
}

QTreeWidgetItem* LOCACC :: cloneMessage(QTreeWidgetItem *itemToClone, QTreeWidgetItem *parent)
{
    QStringList stringData = getMessageTreeData(itemToClone);
    bool isAccTextSame =  stringData.at(1) == "true";
    QStringList newStringData;
    for(int  i  =  0 ; i < stringData.count() ; i++ )
    {
        if( i == 1)
        {
            continue;
        }
        newStringData << stringData.at(i);
    }

    QTreeWidgetItem *clonedMessage;
    int counter = 0;
    do
    {
        clonedMessage = addMessage(newStringData,isAccTextSame, parent);
        counter++;
        newStringData[0] = stringData[0] + "-copy (" + QString::number(counter) + ")";
    }
    while(clonedMessage == NULL);
    return clonedMessage;
}

/*****************************************************************
 * REPLACEMENT POLICY *******************************************
 *****************************************************************/

void LOCACC::setReplacementFilePath(QString replacementFilePath)
{
    m_strReplacementFilePath = replacementFilePath;
}

bool LOCACC::replaceAll()
{
    QFile file(m_strReplacementFilePath);
    if(!file.exists())
    {
        qDebug() << "Replacement file is not found";
        return false;
    }
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray rawData = file.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(rawData));
    QJsonObject replacementJSON = doc.object();
    QJsonArray replacementArray = replacementJSON["replacementArray"].toArray();
    file.close();

    QJsonArray jsonLog;
    QJsonArray scrnJArray = m_jsonMasterObj["locAccData"].toArray();
    QJsonObject tempScrnObj ;
    for(int i = 0 ; i < scrnJArray.count() ; i++ )
    {
        tempScrnObj = scrnJArray.at(i).toObject();
        QJsonArray eleJArray = tempScrnObj["elements"].toArray();
        QJsonObject tempEleObj;
        for(int j = 0 ; j < eleJArray.count(); j++)
        {
            tempEleObj = eleJArray.at(j).toObject();
            QJsonArray msgsArray = tempEleObj["messages"].toArray();
            QJsonObject tempMsgObj;
            for(int k = 0; k < msgsArray.count() ; k++)
            {
                tempMsgObj = msgsArray.at(k).toObject();
                QJsonObject tempLocAccObj = tempMsgObj["message"].toObject();
                QJsonObject tempReplaceObj;
                for(int l = 0 ; l < replacementArray.count() ; l++ )
                {
                    tempReplaceObj = replacementArray.at(l).toObject();
                    if(tempLocAccObj["loc"] == tempReplaceObj["text"])
                    {
                        // Generate Log Object
                        QJsonObject logObject ;
                        logObject["screenId"] = tempScrnObj["id"];
                        logObject["elementId"] = tempEleObj["id"];
                        logObject["messageId"] = tempMsgObj["id"];
                        logObject["originalText"] = tempLocAccObj["loc"];
                        logObject["commonId"] = tempReplaceObj["commonId"];
                        jsonLog.append(logObject);
                        qDebug() << "replaced ";

                        // Actual Change
                        tempLocAccObj["loc"] = QString("");
                        tempLocAccObj["acc"] = QString("");
                        tempLocAccObj["commonId"] = tempReplaceObj["commonId"];
                    }
                }
                tempMsgObj["message"] = tempLocAccObj;
                msgsArray.replace(k,tempMsgObj);
            }
            tempEleObj["messages"] = msgsArray;
            eleJArray.replace(j,tempEleObj);
        }
        tempScrnObj["elements"] = eleJArray;
        scrnJArray.replace(i,tempScrnObj);
    }

    m_jsonMasterObj["locAccData"] = scrnJArray;
    writeFile();
    writeLogFile(jsonLog);
    return true;
}

void LOCACC::writeLogFile(QJsonArray logArray)
{
    QJsonObject jObject;
    jObject.insert("logEntries",logArray);

    QDir dir(m_strBasePath);
    dir.mkpath("D:\\DE-ReplaceLog");

    QString str_logFilePath = "D:\\DE-ReplaceLog\\" + dir.dirName() + ".log" ;
    QFile logFile(str_logFilePath);
    logFile.open(QIODevice::ReadWrite | QIODevice::Text);

    QJsonDocument doc(jObject);
    logFile.write(doc.toJson());
    logFile.close();
}
