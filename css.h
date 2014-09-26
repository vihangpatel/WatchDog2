#ifndef CSS_H
#define CSS_H

#include <QFileSystemWatcher>
#include <QStringList>
#include <QObject>
#include <QDir>
#include <QFileInfoList>
#include <QDebug>

class CSS : public QObject
{
    Q_OBJECT
public:
    explicit CSS(QString basePath);
    QString m_strBasePath;
    QFileSystemWatcher *m_qfswCss;
    int m_iFileCount ;
    QDir *m_dirCss;

    void scanChanges();
    bool registerWatcher();
    void deRegisterWatcher();
    void deregisterFiles();
    void deregisterDirs();
    void connectSignals();
    QFileInfoList getFileInfoList();

public slots:
    void on_file_changed(QString path);
    void on_dir_changed(QString path);
    void changeBasePath(QString);

signals:
    void filesChanged(QFileInfoList);

    
};

#endif // CSS_H
