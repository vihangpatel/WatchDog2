#include "templates.h"
#include <QDebug>
QString TEMPLATE_SUFFIX = "templates";
QString TEMPLATE_FILE_EXTENSION = ".handlebars";
QString TEMPLATE_COMPILE_COMMAND = "C:/Users/Vihang/Desktop/test.bat";
QString TEMPLATE_FOLDER_COMPILE_COMMAND = "C:/Users/Vihang/Desktop/test.bat";

Templates::Templates(QString basePath) {
    qfsw_tmplt = new QFileSystemWatcher();
    tmpltDir = new QDir(basePath);
    process = new QProcess();
    changeBasePath(basePath);
}

void Templates::connectSignals(){
    this->connect(qfsw_tmplt,SIGNAL(directoryChanged(QString)),SLOT(on_dir_changed(QString)));
    this->connect(qfsw_tmplt,SIGNAL(fileChanged(QString)),SLOT(on_file_changed(QString)));
}

void Templates::scanChanges()
{
    emit filesChanged(getFileInfoList());
}

void Templates::changeBasePath(QString strPath){
    fileCount = 0;
    str_basePath = strPath + "/" + TEMPLATE_SUFFIX;
    tmpltDir->setPath(str_basePath);
    deRegisterWatcher();
    registerWatcher();
    emit filesChanged(getFileInfoList());
}

bool Templates::registerWatcher(){

    QString fullPath = str_basePath;
    if(!tmpltDir->exists()){
        qWarning() << "Folder doesnt exists.";
        return false;
    }

    if(qfsw_tmplt != NULL){
        delete qfsw_tmplt;
        qfsw_tmplt = new QFileSystemWatcher();
        connectSignals();
    }

    QFileInfoList lstFiles = getFileInfoList();
    fileCount = lstFiles.length();
    for(int i = 0 ; i < fileCount ; i++ ){
        qfsw_tmplt->addPath(lstFiles.at(i).filePath());
    }

    qfsw_tmplt->addPath(str_basePath);
    qDebug() << "Files added : " << qfsw_tmplt->files().length();
    return true;
}

QFileInfoList Templates::getFileInfoList(){
    QStringList filters;
    filters << "*.handlebars";
    tmpltDir->setNameFilters(filters);
    return tmpltDir->entryInfoList();
}

void Templates::on_file_changed(QString strFilePath){
    qDebug() << "File changed : " << strFilePath;
    QFile file(strFilePath);
    if(!file.exists()){
        qDebug()  << "File do not exits";
         emit filesChanged(getFileInfoList());
        return;
    }
    process->execute(TEMPLATE_COMPILE_COMMAND);
}

void Templates::on_dir_changed(QString strDirPath){
    qDebug() << "Dir changed : " << strDirPath;
    deregisterFiles();
    registerWatcher();
    emit filesChanged(getFileInfoList());
}


void Templates::deRegisterWatcher(){
    deregisterDirs();
    deregisterFiles();
}

void Templates::deregisterFiles(){
    qfsw_tmplt->removePaths(qfsw_tmplt->files());
}

void Templates :: deregisterDirs(){
    qfsw_tmplt->removePaths(qfsw_tmplt->directories());
}
