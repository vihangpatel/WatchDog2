#ifndef TEMPLATES_H
#define TEMPLATES_H

#include <QFileSystemWatcher>
#include <QStringList>
#include <QObject>
#include <QDir>
#include <QProcess>
#include <QFileInfoList>

class Templates : public QObject{
    Q_OBJECT
public:
    Templates(QString);
    ~Templates();
    QString str_basePath;
    QFileSystemWatcher *qfsw_tmplt;
    int fileCount ;
    QDir *tmpltDir;
    QProcess *process;

    void scanChanges();
    bool registerWatcher();
    void deRegisterWatcher();
    void deregisterFiles();
    void deregisterDirs();
    void connectSignals();
    QString getIndividualTemplateCompilePath();
    QString getAllTemplateCompilePath();
    QFileInfoList getFileInfoList();

public slots:
    void on_file_changed(QString path);
    void on_dir_changed(QString path);
    void changeBasePath(QString);
    void compileAllHandleBars();
    void checkBatchFilesExistance();

signals:
    void filesChanged(QFileInfoList);
    void batchDoNotExists();

};


#endif // TEMPLATE_H
