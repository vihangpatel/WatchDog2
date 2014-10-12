#ifndef MEDIAIMAGES_H
#define MEDIAIMAGES_H

#include <QFileSystemWatcher>
#include <QStringList>
#include <QObject>
#include <QDir>
#include <QFileInfoList>
#include <QDebug>

class MediaImages : public QObject
{
    Q_OBJECT
public:
    explicit MediaImages(QString basePath);
    ~MediaImages();
    QString m_strBasePath;
    QFileSystemWatcher *m_qfswMediaImg;
    int m_iFileCount ;
    QDir *m_dirMediaImg;

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

#endif // MEDIAIMAGES_H
