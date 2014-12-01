#include "locacc.h"

QString LOC_LANG_FOLDER = "lang";
QString LOC_EN_FOLDER = "en";
QString LOC_DATA_FOLDER = "data";
QString LOC_FILE_NAME = "loc-acc.json";
QString STR_MISCCEL_SCREEN = "miscellaneous-screen";

LOCACC::LOCACC(QString strPath)
{
    m_strListMandatoryScreens << "title-screen" <<	"tab-contents" <<	"overview-tab" <<	"help-screen";
    QStringList treeList;
    treeList << "locAccData";
    m_dirLang = new QDir(strPath);
    m_qtwiRoot = new QTreeWidgetItem(treeList);
}

void LOCACC::changeBasePath(QString strPath)
{
    //return;
    m_strBasePath  = strPath;
    m_dirLang->setPath(getLangFolderPath());
    getAvailableLangugaes();
    emptyTreeWidget(m_qtwiRoot);
    readFile();
    m_qtwiRoot->takeChildren();
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
    if(msgData.at(1).length() != 0)
    {
        locAccObj["loc"] = msgData.at(1);
    }
    if(!isAccTextSame)
    {
        if(msgData.at(2).length() !=0)
        {
            locAccObj["acc"] = msgData.at(2);
        }
    }
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
    if(elementData.at(1).length() != 0 )
    {
        eleObj["accId"] =  elementData.at(1);
    }
    if(elementData.at(2).length() !=0 )
    {
        eleObj["type"] = elementData.at(2);
    }
    if(elementData.at(3).length() != 0)
    {
        eleObj["role"] =  elementData.at(3);
    }
    if(elementData.at(4).length() != 0)
    {
        eleObj["tabIndex"] =  elementData.at(4).toInt();
    }
    if(elementData.length() > 5 && elementData.at(5).length() != 0)
    {
        eleObj["offsetTop"] =  elementData.at(5).toInt();
    }
    if(elementData.length() > 6 && elementData.at(6).length() != 0)
    {
        eleObj["offsetLeft"] =  elementData.at(6).toInt();
    }
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
            << (elementObj.contains("tabIndex") ? QString::number(elementObj["tabIndex"].toInt()) : "")
            << (elementObj.contains("offsetTop") ? QString::number(elementObj["offsetTop"].toInt()) : "")
            << (elementObj.contains("offsetLeft") ? QString::number(elementObj["offsetLeft"].toInt()) : "");
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
            <<  messageObj["message"].toObject().value("acc").toString()
            << messageObj["message"].toObject().value("commonId").toString();
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
     // qDebug() << "informal :" << currentIndex  ;
    if (item && ((currentIndex < newIndex &&  currentIndex < m_qtwiRoot->childCount() - 1)
                  ||  (currentIndex > 0 && currentIndex > newIndex)))
    {
        // Visual Update        
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
    m_jsonMasterObj["id"] = getInteractivityName();
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

bool LOCACC::replaceAll(QString commonLocAccFilePath)
{        
    QFile file(commonLocAccFilePath);
    if(!file.exists())
    {
        qDebug() << "Replacement file is not found";
        return false;
    }
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray rawData = file.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(rawData));
    QJsonObject replacementJSON = doc.object();
    QJsonArray replacementArray = replacementJSON["locAccData"].toArray();
    file.close();

    QJsonObject jsonObj_currentScreen;
    for(int i = 0 ; i < replacementArray.count() ; i++)
    {
        jsonObj_currentScreen = replacementArray.at(i).toObject();
        if(jsonObj_currentScreen["id"] == STR_MISCCEL_SCREEN)
        {
            replacementArray = jsonObj_currentScreen["elements"].toArray();
            break;
        }
    }

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
                    QJsonArray messageArray = tempReplaceObj["messages"].toArray();
                    for(int m = 0 ; m < messageArray.count() ; m++)
                    {
                        QJsonObject messageObject = messageArray.at(m).toObject();
                        QString str_locText = tempLocAccObj["loc"].toString();
                        bool b_compareResult = str_locText.compare(messageObject["message"].toObject()["loc"].toString());
                       /* qDebug() << b_compareResult << " " << str_locText
                                 << "  " << messageObject["message"].toObject()["loc"].toString();*/
                        if(b_compareResult == 0)
                        {
                            // Generate Log Object
                            QJsonObject logObject ;
                            logObject["screenId"] = tempScrnObj["id"];
                            logObject["elementId"] = tempEleObj["id"];
                            logObject["messageId"] = tempMsgObj["id"];
                            logObject["originalText"] = tempLocAccObj["loc"];
                            logObject["commonId"] = tempReplaceObj["id"];
                            jsonLog.append(logObject);                            

                            // Actual Change
                            tempLocAccObj["loc"] = QString("");
                            tempLocAccObj["acc"] = QString("");
                            tempLocAccObj["commonId"] = tempReplaceObj["id"];

                            tempMsgObj["message"] = tempLocAccObj;
                            msgsArray.replace(k,tempMsgObj);
                        }
                    }
                }               
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
    writeHtmlLogFile(jsonLog);
    return true;
}

void LOCACC::writeLogFile(QJsonArray logArray)
{
    QJsonObject jObject;
    jObject.insert("logEntries",logArray);

    QFile logFile(getLogFilePath() + ".log");
    logFile.remove();
    logFile.open(QIODevice::ReadWrite | QIODevice::Text);    

    QJsonDocument doc(jObject);
    logFile.write(doc.toJson());
    logFile.close();
}

void LOCACC::writeHtmlLogFile(QJsonArray logArray)
{
    QString logText = "<html><head><title>"+ getInteractivityName() +
            "</title></head><body><h1 style='text-align:center;'>Replacement Log for " + getInteractivityName() +"</h1>"
            "<table border='1' style='text-align: center;margin: auto'>"
            "<thead style='font-weight: bold;'><td>Screen</td><td>Element</td><td>Message</td><td>LOC Text</td><td>CommonId</td></thead>";
    for(int i = 0 ; i < logArray.count() ; i++)
    {
        QJsonObject obj = logArray.at(i).toObject();
        logText += "<tr><td>" + obj["screenId"].toString()
                     + "</td><td>" + obj["elementId"].toString()
                     + "</td><td>" + obj["messageId"].toString()
                     + "</td><td>" + obj["originalText"].toString()
                     + "</td><td>" + obj["commonId"].toString()
                     + "</td></tr>";
    }
    logText += "</table><h1 style='text-align:center;'>Total changes :" + QString::number(logArray.count()) + "</body></html>";

    QString str_htmlFilePath = getLogFilePath() + ".htm";
    QFile htmlLogFile(str_htmlFilePath);
    htmlLogFile.remove();
    htmlLogFile.open(QIODevice::ReadWrite | QIODevice::Text);
    QTextStream out(&htmlLogFile);
    out << logText;
    htmlLogFile.close();    
    QDesktopServices::openUrl(QUrl(str_htmlFilePath.replace("\\","/")));
}

QString LOCACC::getLogFilePath()
{
    QDir dir(m_strBasePath);
    QString str_logDirPath = "D:\\DE-ReplaceLog\\" + dir.dirName();
    dir.mkpath(str_logDirPath);

    QString str_logFilePath = str_logDirPath + "\\" + dir.dirName() +
            "_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    return str_logFilePath;
}

QString LOCACC::getInteractivityName()
{
    QDir dir(m_strBasePath);
    dir.mkpath("D:\\DE-ReplaceLog");

    return dir.dirName();
}

bool LOCACC::validateLocAccJson()
{
    bool returnFlag = true;
    QString logText = "<html><head><title>"+ getInteractivityName() + " Validation-Report"
            "</title></head><body><h1 style='text-align:center;'>Validation Log for " + getInteractivityName() +"</h1>";

    QString messageLogText = "<div><h3 style='text-align: center;'>Repeated Message IDs</h3>"
            "<table border='1' style='text-align: center;margin: auto'>"
            "<thead><tr><th>Screen Id</th><th>Element Id</th><th>Message Id</th><th>Frequency</th></tr></thead>";

    QString elementLogText = "<div><h3 style='text-align: center;'>Repeated Element IDs</h3>"
            "<table border='1' style='text-align: center;margin: auto'>"
            "<thead><tr><th>Screen Id</th><th>Element Id</th><th>Frequency</th></tr></thead>";

    QTreeWidgetItem *screenItem,*elementItem,*messageItem;
    QHash<QString,int> screenMap,elementMap,messageMap ;
    for(int i = 0 ; i < m_qtwiRoot->childCount(); i++)
    {
        screenItem = m_qtwiRoot->child(i);
        screenMap[screenItem->text(0)] = screenMap[screenItem->text(0)] + 1;
        elementMap.clear();
        // Scan each element for duplicate ID entry
        for(int j = 0 ; j < screenItem->childCount() ; j++)
        {
            elementItem = screenItem->child(j);
            elementMap[elementItem->text(0)] = elementMap[elementItem->text(0)] + 1;
            messageMap.clear();
            // Scan each message for duplicate ID entry
            for(int k = 0 ; k < elementItem->childCount() ; k++)
            {
                messageItem = elementItem->child(k);
                messageMap[messageItem->text(0)] = messageMap[messageItem->text(0)] + 1;
            }
            QList<QString> messageIds = messageMap.keys();
            for(int k = 0 ; k < messageIds.length() ; k++)
            {
                int freq = messageMap[messageIds[k]];
                if(freq > 1)
                {
                    messageLogText += "<tr><td>" + screenItem->text(0) + "</td>"
                                       + "<td>" + elementItem->text(0) + "</td>"
                                       + "<td>" + messageIds[k] + "</td>"
                                       + "<td>" + QString::number(freq) + "</td></tr>";
                    returnFlag = false;
                }
            }
        }
        QList<QString> elementIds = elementMap.keys();
        for(int j = 0 ; j < elementIds.length(); j++)
        {
            int freq = elementMap[elementIds[j]];
            if(freq > 1)
            {
                returnFlag = false;
                elementLogText += "<tr><td>" + screenItem->text(0) + "</td>"
                        + "<td>" + elementIds[j] + "</td>"
                        + "<td>" + QString::number(freq) + "</td></tr>";
            }
        }
    }

    messageLogText += "</table></div>";
    elementLogText += "</table></div>";


    QString missingScreenLog = "<h3 style = 'text-align : center'>Missing Screens</h3>"
            "<table border='1' style='text-align: center;margin: auto'>"
            "<thead><tr><th>Index</th><th>Missing Screen Id</th></tr></thead>";
    for(int i = 0 ; i < m_strListMandatoryScreens.length() ; i++)
    {
        if(!screenMap.contains(m_strListMandatoryScreens.at(i)))
        {
            missingScreenLog += "<tr><td>" + QString::number(i+1) + "</td>"
                             + "<td>" + m_strListMandatoryScreens.at(i) + "</td></tr>";
            returnFlag = false;
        }
    }

    missingScreenLog += "</table>";
    logText += messageLogText + "</br>" + elementLogText + "<br>" + missingScreenLog;

    logText += isMessagesArray();

    QString str_htmlFilePath = getLogFilePath() + "-validation.htm";
    QFile htmlLogFile(str_htmlFilePath);
    htmlLogFile.remove();
    htmlLogFile.open(QIODevice::ReadWrite | QIODevice::Text);
    QTextStream out(&htmlLogFile);
    out << logText;
    htmlLogFile.close();
    QDesktopServices::openUrl(QUrl(str_htmlFilePath.replace("\\","/")));
    return returnFlag;
}

QString LOCACC::isMessagesArray()
{
    QString messagesObj = "<h3 style = 'text-align : center'>Following message list has problem</h3>"
            "<table border='1' style='text-align: center;margin: auto'>"
            "<thead><tr><th>Index</th><th>Screen Id</th><th>Element Id</th></tr></thead>";

    QTreeWidgetItem *screenItem,*elementItem;
    int count=0;
    QJsonObject jo_screen,jo_element;
    for(int i = 0 ; i < m_qtwiRoot->childCount(); i++)
    {
        screenItem = m_qtwiRoot->child(i);
        jo_screen = fetchScreenJObject(QStringList(screenItem->text(0)));
        // Scan each element for duplicate ID entry
        for(int j = 0 ; j < screenItem->childCount() ; j++)
        {
            elementItem = screenItem->child(j);
            jo_element = fetchElementJObject(QStringList(elementItem->text(0)),jo_screen.value("elements").toArray());
            if(!jo_element.value("messages").isArray())
            {
                count++;
                messagesObj += "<tr><td>" + QString::number(count) + "</td><td>" +
                        screenItem->text(0) + "</td><td>" + elementItem->text(0) + "</td></tr>";
            }
        }
    }

    messagesObj += "</table>";
    return messagesObj;
}

LOCACC::~LOCACC()
{
    return;
    delete m_qtwiRoot;
    delete m_dirLang;
}
