#include "js.h"
#include <QDebug>

QString JS_SUFFIX = "js";
QString JS_VIEW_SUFFIX = "views";
QString JS_MODEL_SUFFIX = "models";

JS::JS(QString basePath){
    m_qfswJsView = new QFileSystemWatcher();
    m_qfswJsModel = new QFileSystemWatcher();
    m_dirJs = new QDir();
    m_dirView = new QDir();
    m_dirModel = new QDir();
    m_process = new QProcess();
    changeBasePath(basePath);
}

void JS::connectSignals(){
    this->connect(m_qfswJsView,SIGNAL(directoryChanged(QString)),SLOT(on_viewDir_changed(QString)));
    this->connect(m_qfswJsModel,SIGNAL(directoryChanged(QString)),SLOT(on_modelDir_changed(QString)));
}

QString JS::getViewDirPath(){
    return m_strBasePath + "/" + JS_VIEW_SUFFIX;
}

QString JS::getModelDirPath(){
    return m_strBasePath + "/" + JS_MODEL_SUFFIX;
}

void JS::scanChanges()
{
    emit jsfilesChanged(getAllFileInfoList());
}

void JS::setBasePath(QString basePath){
    m_strBasePath = basePath + "/" + JS_SUFFIX;
}

void JS::changeBasePath(QString strPath){
    m_iViewFileCount = 0;
    m_iModelFileCount = 0;
    setBasePath(strPath);
    m_dirJs->setPath(m_strBasePath);
    m_dirView->setPath(getViewDirPath());
    m_dirModel->setPath(getModelDirPath());
    deRegisterWatcher();
    if(!m_dirJs->exists()){
        // qDebug() << "JS folder doesn't exist. : " << str_basePath;
        return;
    }
    registerWatcher();
    emit viewfilesChanged(getViewFileInfoList());
    emit modelfilesChanged(getModelFileInfoList());
    emit jsfilesChanged(getAllFileInfoList());
}

QFileInfoList JS::getAllFileInfoList()
{
    QFileInfoList viewList = getViewFileInfoList();
    QFileInfoList modelList  = getModelFileInfoList();
    QFileInfoList  jsFolderList = getJSFolderInfoList();
    jsFolderList.append( modelList);
    jsFolderList.append(viewList);
    return jsFolderList;
}

QFileInfoList JS::getJSFolderInfoList()
{
    QStringList filters;
    filters << "*.js";
    m_dirJs->setNameFilters(filters);
    return m_dirJs->entryInfoList();
}

QFileInfoList JS::getViewFileInfoList(){
    QStringList filters;
    filters << "*.js";
    m_dirView->setNameFilters(filters);
    return m_dirView->entryInfoList();
}

QFileInfoList JS::getModelFileInfoList(){
    QStringList filters;
    filters << "*.js";
    m_dirModel->setNameFilters(filters);
    return m_dirModel->entryInfoList();
}

bool JS::registerWatcher(){
    if(m_qfswJsView != NULL) {
        delete m_qfswJsView;
    }
    if( m_qfswJsModel != NULL){
        delete m_qfswJsModel;
    }
    m_qfswJsView = new QFileSystemWatcher();
    m_qfswJsView->addPath(getViewDirPath());
    m_qfswJsModel = new QFileSystemWatcher();
    m_qfswJsModel->addPath(getModelDirPath());
    disconnectSignals();
    connectSignals();
    return true;
}

void JS :: disconnectSignals(){
    disconnect(m_qfswJsView,SIGNAL(directoryChanged(QString)),this,SLOT(on_viewDir_changed(QString)));
    disconnect(m_qfswJsModel,SIGNAL(directoryChanged(QString)),this,SLOT(on_modelDir_changed(QString)));
}

void JS::on_viewDir_changed(QString strFilePath){
    // qDebug() << "View folder changed : " << strFilePath;
    emit viewfilesChanged(getViewFileInfoList());
    emit jsfilesChanged(getAllFileInfoList());
}

void JS::on_modelDir_changed(QString strDirPath){
    // qDebug() << "Model folder changed : " << strDirPath;
    emit modelfilesChanged(getModelFileInfoList());
    emit jsfilesChanged(getAllFileInfoList());
}

void JS::deRegisterWatcher(){
    m_qfswJsModel->removePaths(m_qfswJsModel->directories());
    m_qfswJsView->removePaths(m_qfswJsView->directories());
    m_qfswJsView->disconnect(m_qfswJsView,SIGNAL(directoryChanged(QString)),this,SLOT(on_viewDir_changed(QString)));
    m_qfswJsModel->disconnect(m_qfswJsModel,SIGNAL(directoryChanged(QString)),this,SLOT(on_modelDir_changed(QString)));
}

