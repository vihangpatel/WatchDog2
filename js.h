#ifndef JS_H
#define JS_H

#include <QFileSystemWatcher>
#include <QStringList>
#include <QObject>
#include <QDir>
#include <QProcess>
#include <QFileInfoList>

class JS  : public QObject{
    Q_OBJECT
public :

    JS(QString);
    ~JS();
    QString m_strBasePath;
    QFileSystemWatcher *m_qfswJsView;
    QFileSystemWatcher *m_qfswJsModel;
    int m_iViewFileCount ;
    int m_iModelFileCount ;
    QDir *m_dirJs;
    QDir *m_dirView;
    QDir *m_dirModel;
    QProcess *m_process;

    void scanChanges();
    bool registerWatcher();
    void deRegisterWatcher();
    void deregisterFiles();
    void deregisterDirs();
    void connectSignals();
    void disconnectSignals();
    QString getViewDirPath();
    QString getModelDirPath();
    void setBasePath(QString);
    QFileInfoList getViewFileInfoList();
    QFileInfoList getModelFileInfoList();
    QFileInfoList getJSFolderInfoList();
    QFileInfoList getAllFileInfoList();

public slots:
    void on_viewDir_changed(QString path);
    void on_modelDir_changed(QString path);
    void changeBasePath(QString);

signals:
    void viewfilesChanged(QFileInfoList);
    void modelfilesChanged(QFileInfoList);
    void jsfilesChanged(QFileInfoList);

};

#endif // JS_H
