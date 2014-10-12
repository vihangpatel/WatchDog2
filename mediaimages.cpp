#include "mediaimages.h"

QString MEDIA_SUFFIX = "media/image";
QString MEDIA_FILE_EXTENSION = ".css";

MediaImages::MediaImages(QString basePath) {
    m_qfswMediaImg = new QFileSystemWatcher();
    m_dirMediaImg = new QDir(basePath);
}

void MediaImages::connectSignals(){
    this->connect(m_qfswMediaImg,SIGNAL(directoryChanged(QString)),SLOT(on_dir_changed(QString)));
    this->connect(m_qfswMediaImg,SIGNAL(fileChanged(QString)),SLOT(on_file_changed(QString)));
}

void MediaImages::scanChanges()
{
    emit filesChanged(getFileInfoList());
}

void MediaImages::changeBasePath(QString strPath){
    m_iFileCount = 0;
    m_strBasePath = strPath + "/" + MEDIA_SUFFIX;
    m_dirMediaImg->setPath(m_strBasePath);
    deRegisterWatcher();
    registerWatcher();
    emit filesChanged(getFileInfoList());
}

bool MediaImages::registerWatcher(){

    QString fullPath = m_strBasePath;
    if(!m_dirMediaImg->exists()){
        qWarning() << "Folder doesnt exists.";
        return false;
    }

    if(m_qfswMediaImg != NULL){
        delete m_qfswMediaImg;
        m_qfswMediaImg = new QFileSystemWatcher();
        connectSignals();
    }

    QFileInfoList lstFiles = getFileInfoList();
    m_iFileCount = lstFiles.length();
    for(int i = 0 ; i < m_iFileCount ; i++ ){
        m_qfswMediaImg->addPath(lstFiles.at(i).filePath());
    }

    m_qfswMediaImg->addPath(m_strBasePath);
    // qDebug() << "Files added : " << qfsw_MediaImages->files().length();
    return true;
}

QFileInfoList MediaImages::getFileInfoList(){
    QStringList filters;
    filters << "*.jpg" << "*.png" << "*.jpeg" << "*.svg";
    m_dirMediaImg->setNameFilters(filters);
    return m_dirMediaImg->entryInfoList();
}

void MediaImages::on_file_changed(QString strFilePath){
    // qDebug() << "File changed : " << strFilePath;
    QFile file(strFilePath);
    if(!file.exists()){
        // qDebug()  << "File do not exits";
         emit filesChanged(getFileInfoList());
        return;
    }
}

void MediaImages::on_dir_changed(QString strDirPath){
    // qDebug() << "Dir changed : " << strDirPath;
    deregisterFiles();
    registerWatcher();
    emit filesChanged(getFileInfoList());
}


void MediaImages::deRegisterWatcher(){
    deregisterDirs();
    deregisterFiles();
}

void MediaImages::deregisterFiles(){
    m_qfswMediaImg->removePaths(m_qfswMediaImg->files());
}

void MediaImages :: deregisterDirs(){
    m_qfswMediaImg->removePaths(m_qfswMediaImg->directories());
}

MediaImages::~MediaImages()
{
    return;
    deRegisterWatcher();
    delete m_qfswMediaImg;
    delete m_dirMediaImg;
}
