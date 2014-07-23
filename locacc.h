#ifndef LOCACC_H
#define LOCACC_H

#include <QObject>
#include <QJsonObject>
#include <QTreeWidgetItem>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>
#include <QIODevice>
#include <QFile>
#include <QString>

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

    void changeBasePath(QString strPath);
    QString getLocAccFilePath();
    void addScreen(QString screenID ,QString screenName);
    void addElement(QString parentScreen,QString elementName);
    void addMessage(QString parentScreen,QString eleName,QString msgId , QString locMessage,QString accMessage,bool isAccTextSame);
    void readFile();
    void writeFile();
    void emptyTreeWidget(QTreeWidgetItem* parent);
    QTreeWidgetItem * getLocAccTree();
    QTreeWidgetItem *getScreenTree(QJsonObject screenId);
    QList<QTreeWidgetItem *>getElementsTree(QJsonObject screenId);
    QJsonObject getElementJson(QString screenName);

 signals:
    void treeUpdated(QTreeWidgetItem *);
};

#endif // LOCACC_H
