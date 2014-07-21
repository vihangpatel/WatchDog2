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
    QString str_basePath;
    QFileSystemWatcher *qfsw_jsView;
    QFileSystemWatcher *qfsw_jsModel;
    int viewfileCount ;
    int modelfileCount ;
    QDir *jsDir;
    QDir *viewDir;
    QDir *modelDir;
    QProcess *process;

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
