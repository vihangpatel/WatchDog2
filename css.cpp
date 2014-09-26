#include "css.h"

QString CSS_SUFFIX = "css";
QString CSS_FILE_EXTENSION = ".css";

CSS::CSS(QString basePath) {
    m_qfswCss = new QFileSystemWatcher();
    m_dirCss = new QDir(basePath);
    changeBasePath(basePath);
}

void CSS::connectSignals(){
    this->connect(m_qfswCss,SIGNAL(directoryChanged(QString)),SLOT(on_dir_changed(QString)));
    this->connect(m_qfswCss,SIGNAL(fileChanged(QString)),SLOT(on_file_changed(QString)));
}

void CSS::scanChanges()
{
    emit filesChanged(getFileInfoList());
}

void CSS::changeBasePath(QString strPath){
    m_iFileCount = 0;
    m_strBasePath = strPath + "/" + CSS_SUFFIX;
    m_dirCss->setPath(m_strBasePath);
    deRegisterWatcher();
    registerWatcher();
    emit filesChanged(getFileInfoList());
}

bool CSS::registerWatcher(){

    QString fullPath = m_strBasePath;
    if(!m_dirCss->exists()){
        qWarning() << "Folder doesnt exists.";
        return false;
    }

    if(m_qfswCss != NULL){
        delete m_qfswCss;
        m_qfswCss = new QFileSystemWatcher();
        connectSignals();
    }

    QFileInfoList lstFiles = getFileInfoList();
    m_iFileCount = lstFiles.length();
    for(int i = 0 ; i < m_iFileCount ; i++ ){
        m_qfswCss->addPath(lstFiles.at(i).filePath());
    }

    m_qfswCss->addPath(m_strBasePath);
    // qDebug() << "Files added : " << qfsw_css->files().length();
    return true;
}

QFileInfoList CSS::getFileInfoList(){
    QStringList filters;
    filters << "*.css";
    m_dirCss->setNameFilters(filters);
    return m_dirCss->entryInfoList();
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
    m_qfswCss->removePaths(m_qfswCss->files());
}

void CSS :: deregisterDirs(){
    m_qfswCss->removePaths(m_qfswCss->directories());
}

