#ifndef LOCACC_H
#define LOCACC_H

#include <QObject>
#include <QJsonObject>
#include <QTreeWidgetItem>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMessageBox>
#include <QDebug>
#include <QIODevice>
#include <QFile>
#include <QString>
#include <QDir>
#include <QUrl>
#include <QDesktopServices>
#include <QHash>
#include <QDateTime>

class LOCACC : public QObject
{
    Q_OBJECT
public:
    explicit LOCACC(QString);
    ~LOCACC();
    QString m_strBasePath;
    QFile m_fileLocAcc;
    QJsonObject m_jsonMasterObj;
    QJsonArray m_jsonMasterArray;
    QTreeWidgetItem *m_qtwiRoot;
    QList<QTreeWidgetItem *> m_listSearchedResult;
    int m_iCurrentSearchIndex = 0;
    int m_iCurrentLangIndex = 0;
    QStringList m_strListAvailableLang;
    QDir *m_dirLang;
    QStringList m_strListMandatoryScreens;

    void changeBasePath(QString strPath);
    QString getLocAccFilePath();
    QString getLangFolderPath();
    QTreeWidgetItem* addScreen(QStringList screenData);
    QTreeWidgetItem* addElement(QStringList elementData,QTreeWidgetItem *parent);
    QTreeWidgetItem* addMessage(QStringList msgData,bool isAccTextSame,QTreeWidgetItem *parent);
    void setMessageTooltip(QTreeWidgetItem *messageItem,QJsonObject messageJObj);
    void readFile();
    void writeFile();
    void emptyTreeWidget(QTreeWidgetItem* parent);
    QTreeWidgetItem * getLocAccTree();
    QTreeWidgetItem *generateScreenTree(QJsonObject screenId);
    QList<QTreeWidgetItem *>generateElementsTree(QJsonObject screenId);
    QList<QTreeWidgetItem *>generateMessageTree(QJsonArray eleId);
    QJsonObject getElementJson(QStringList screenData);
    QJsonObject getMessageJson(QStringList msgData,bool isAccTextSame);

    QStringList getScreenTreeData(QTreeWidgetItem *screenItem);
    QStringList getElementTreeData(QTreeWidgetItem *elementItem);
    QStringList getMessageTreeData(QTreeWidgetItem *messageItem);

    bool changeScreen(QStringList newScreenData,QTreeWidgetItem *currentItem);
    bool changeElement(QStringList newScreenData,QTreeWidgetItem *currentItem);
    bool changeMessage(QStringList newMsgData,QTreeWidgetItem *currentItem);

    bool screenExistance(QStringList screenData,QJsonArray parenScreenJObj);
    bool elementExistance(QStringList elementData,QJsonArray parenScreenJObj);
    bool messageExistance(QStringList messageData,QJsonArray parentEleJArray);

    bool deleteScreen(QTreeWidgetItem *currentItem);
    bool deleteElement(QTreeWidgetItem *currentItem);
    bool deleteMessage(QTreeWidgetItem *currentItem);

    bool updateScreen(QStringList newScreenData , QTreeWidgetItem *currentScreen);
    bool updateElement(QStringList newElementData , QTreeWidgetItem *currentElements);
    bool updateMessage(QStringList newMessageData , bool isAccTextSame , QTreeWidgetItem *currentMessage);

    QList<QTreeWidgetItem*> getSearchResult(QString searchText);

    QTreeWidgetItem* getCurrentSearchResult();
    QTreeWidgetItem* getPrevSeachResult();
    QTreeWidgetItem* getNextSearchResult();
    int getCurrentSearchIndex();
    bool changeOrder(int currentIndex,int newIndex,QTreeWidgetItem *item);

    QJsonObject fetchScreenJObject(QStringList screenData);
    QJsonObject fetchElementJObject(QStringList elementData,QJsonArray parenScreenJObj);
    QJsonObject fetchMessageJObject(QStringList messageData,QJsonArray parentEleJArray);

    bool addNewLanguage(QString lang);
    bool changeLanguage(int m_iCurrentLangIndex);
    void makeNewLangFolder(QString newLang);
    QStringList getAvailableLangugaes();

    QTreeWidgetItem *cloneScreen(QTreeWidgetItem *itemToClone);
    QTreeWidgetItem *cloneElement(QTreeWidgetItem *itemToClone,QTreeWidgetItem *parent);
    QTreeWidgetItem *cloneMessage(QTreeWidgetItem *itemToClone,QTreeWidgetItem *parent);

    void setReplacementFilePath(QString replacementFilePath);
    bool replaceAll(QString commonLocAccFilePath);
    bool validateLocAccJson();

    void writeLogFile(QJsonArray logArray);
    void writeHtmlLogFile(QJsonArray logArray);
    QString getLogFilePath();
    QString getInteractivityName();

    QString isMessagesArray();

 signals:
    void treeUpdated(QTreeWidgetItem *);
};

#endif // LOCACC_H
