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
    bool addScreen(QStringList screenData);
    bool addElement(QStringList elementData,QTreeWidgetItem *parent);
    bool addMessage(QStringList msgData,bool isAccTextSame,QTreeWidgetItem *parent);
    void readFile();
    void writeFile();
    void emptyTreeWidget(QTreeWidgetItem* parent);
    QTreeWidgetItem * getLocAccTree();
    QTreeWidgetItem *getScreenTree(QJsonObject screenId);
    QList<QTreeWidgetItem *>getElementsTree(QJsonObject screenId);
    QJsonObject getElementJson(QStringList screenName);

    bool screenExists(QStringList screenData);
    bool elementExists(QStringList elementData,QJsonArray parenScreenJObj);
    bool messageExists(QStringList messageData);

 signals:
    void treeUpdated(QTreeWidgetItem *);
};

#endif // LOCACC_H
