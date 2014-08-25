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
class LOCACC : public QObject
{
    Q_OBJECT
public:
    explicit LOCACC(QString);
    QString str_basePath;
    QFile locAccFile;
    QJsonObject masterJObj;
    QJsonArray masterArray;
    QTreeWidgetItem *root;
    QList<QTreeWidgetItem *> searchedResultList;
    int currentSearchIndex = 0;
    int currentLangIndex = 0;
    QStringList availableLangList;
    QDir *langDir;

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

    bool screenExistance(QStringList screenData);
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

    QJsonObject fetchScreenJObject(QStringList screenData);
    QJsonObject fetchElementJObject(QStringList elementData,QJsonArray parenScreenJObj);
    QJsonObject fetchMessageJObject(QStringList messageData,QJsonArray parentEleJArray);

    bool addNewLanguage(QString lang);
    bool changeLanguage(int currentLangIndex);
    void makeNewLangFolder(QString newLang);
    QStringList getAvailableLangugaes();


 signals:
    void treeUpdated(QTreeWidgetItem *);
};

#endif // LOCACC_H
