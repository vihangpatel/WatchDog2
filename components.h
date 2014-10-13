#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>

class components : public QObject
{
    Q_OBJECT
public:

    QString m_strBasePath;
    QString m_strCommonPath;
    QString m_usableComponents;
    QList<QString> m_usedCompList;
    QList<QString> m_unusedCompList;

    explicit components(QString basePath);
    void changeBasePath(QString basePath);
    void setCommonPath(QString commonPath);
    QString getPlayerConfigPath();
    void setUsedComponentList(QList<QString> usedComponent);
    QList<QString> getUsedComponentList();
    QList<QString> getUnusedComponentList();
    void useComponent(int componentId);
    void removeComponent(int componentId);

private:
    void readPlayerConfigFile();
    void filterComponentList();

signals:
    void componentsChanged();

public slots:

};

#endif // COMPONENTS_H
