#include "js.h"
#include <QDebug>

QString JS_SUFFIX = "js";
QString JS_VIEW_SUFFIX = "views";
QString JS_MODEL_SUFFIX = "models";

JS::JS(QString basePath){
    qfsw_jsView = new QFileSystemWatcher();
    qfsw_jsModel = new QFileSystemWatcher();
    jsDir = new QDir();
    viewDir = new QDir();
    modelDir = new QDir();
    process = new QProcess();
    changeBasePath(basePath);
}

void JS::connectSignals(){
    this->connect(qfsw_jsView,SIGNAL(directoryChanged(QString)),SLOT(on_viewDir_changed(QString)));
    this->connect(qfsw_jsModel,SIGNAL(directoryChanged(QString)),SLOT(on_modelDir_changed(QString)));
}

QString JS::getViewDirPath(){
    return str_basePath + "/" + JS_VIEW_SUFFIX;
}

QString JS::getModelDirPath(){
    return str_basePath + "/" + JS_MODEL_SUFFIX;
}

void JS::scanChanges()
{
    emit jsfilesChanged(getAllFileInfoList());
}

void JS::setBasePath(QString basePath){
    str_basePath = basePath + "/" + JS_SUFFIX;
}

void JS::changeBasePath(QString strPath){
    viewfileCount = 0;
    modelfileCount = 0;
    setBasePath(strPath);
    jsDir->setPath(str_basePath);
    viewDir->setPath(getViewDirPath());
    modelDir->setPath(getModelDirPath());
    deRegisterWatcher();
    if(!jsDir->exists()){
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
    jsDir->setNameFilters(filters);
    return jsDir->entryInfoList();
}

QFileInfoList JS::getViewFileInfoList(){
    QStringList filters;
    filters << "*.js";
    viewDir->setNameFilters(filters);
    return viewDir->entryInfoList();
}

QFileInfoList JS::getModelFileInfoList(){
    QStringList filters;
    filters << "*.js";
    modelDir->setNameFilters(filters);
    return modelDir->entryInfoList();
}

bool JS::registerWatcher(){
    if(qfsw_jsView != NULL) {
        delete qfsw_jsView;
    }
    if( qfsw_jsModel != NULL){
        delete qfsw_jsModel;
    }
    qfsw_jsView = new QFileSystemWatcher();
    qfsw_jsView->addPath(getViewDirPath());
    qfsw_jsModel = new QFileSystemWatcher();
    qfsw_jsModel->addPath(getModelDirPath());
    disconnectSignals();
    connectSignals();
    return true;
}

void JS :: disconnectSignals(){
    disconnect(qfsw_jsView,SIGNAL(directoryChanged(QString)),this,SLOT(on_viewDir_changed(QString)));
    disconnect(qfsw_jsModel,SIGNAL(directoryChanged(QString)),this,SLOT(on_modelDir_changed(QString)));
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
    qfsw_jsModel->removePaths(qfsw_jsModel->directories());
    qfsw_jsView->removePaths(qfsw_jsView->directories());
    qfsw_jsView->disconnect(qfsw_jsView,SIGNAL(directoryChanged(QString)),this,SLOT(on_viewDir_changed(QString)));
    qfsw_jsModel->disconnect(qfsw_jsModel,SIGNAL(directoryChanged(QString)),this,SLOT(on_modelDir_changed(QString)));
}

