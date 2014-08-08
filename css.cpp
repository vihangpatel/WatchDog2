#include "css.h"

QString CSS_SUFFIX = "css";
QString CSS_FILE_EXTENSION = ".css";

CSS::CSS(QString basePath) {
    qfsw_css = new QFileSystemWatcher();
    cssDir = new QDir(basePath);
    changeBasePath(basePath);
}

void CSS::connectSignals(){
    this->connect(qfsw_css,SIGNAL(directoryChanged(QString)),SLOT(on_dir_changed(QString)));
    this->connect(qfsw_css,SIGNAL(fileChanged(QString)),SLOT(on_file_changed(QString)));
}

void CSS::scanChanges()
{
    emit filesChanged(getFileInfoList());
}

void CSS::changeBasePath(QString strPath){
    fileCount = 0;
    str_basePath = strPath + "/" + CSS_SUFFIX;
    cssDir->setPath(str_basePath);
    deRegisterWatcher();
    registerWatcher();
    emit filesChanged(getFileInfoList());
}

bool CSS::registerWatcher(){

    QString fullPath = str_basePath;
    if(!cssDir->exists()){
        qWarning() << "Folder doesnt exists.";
        return false;
    }

    if(qfsw_css != NULL){
        delete qfsw_css;
        qfsw_css = new QFileSystemWatcher();
        connectSignals();
    }

    QFileInfoList lstFiles = getFileInfoList();
    fileCount = lstFiles.length();
    for(int i = 0 ; i < fileCount ; i++ ){
        qfsw_css->addPath(lstFiles.at(i).filePath());
    }

    qfsw_css->addPath(str_basePath);
    // qDebug() << "Files added : " << qfsw_css->files().length();
    return true;
}

QFileInfoList CSS::getFileInfoList(){
    QStringList filters;
    filters << "*.css";
    cssDir->setNameFilters(filters);
    return cssDir->entryInfoList();
}

void CSS::on_file_changed(QString strFilePath){
    // qDebug() << "File changed : " << strFilePath;
    QFile file(strFilePath);
    if(!file.exists()){
        // qDebug()  << "File do not exits";
         emit filesChanged(getFileInfoList());
        return;
    }
}

void CSS::on_dir_changed(QString strDirPath){
    // qDebug() << "Dir changed : " << strDirPath;
    deregisterFiles();
    registerWatcher();
    emit filesChanged(getFileInfoList());
}


void CSS::deRegisterWatcher(){
    deregisterDirs();
    deregisterFiles();
}

void CSS::deregisterFiles(){
    qfsw_css->removePaths(qfsw_css->files());
}

void CSS :: deregisterDirs(){
    qfsw_css->removePaths(qfsw_css->directories());
}

