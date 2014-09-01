#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initialize();
}

void MainWindow::loadSavedSettings(){
        str_rootPath= appConfig->getRootPath();
        str_basePath = appConfig->getCurrentInteractivity();
        ui->cb_stopCSSMonitor->setChecked(appConfig->monitorCSS());
        ui->cb_stopJSMonitor->setChecked(appConfig->monitorJS());
        ui->cb_stopMediaMonitor->setChecked(appConfig->monitorMedia());
        ui->cb_stopTmpltMonitir->setChecked(appConfig->monitorTemplates());
        QDir dir(str_basePath);
        ui->statusBar->showMessage("Current interactivity : " + dir.dirName());
        ui->label_interActiveName->setText(dir.dirName());
}

void MainWindow::storeSetting()
{
    appConfig->resetFlag();
    appConfig->setRootPath(str_rootPath);
    appConfig->setCurrentInteractivity(str_basePath);
    appConfig->setJSFlag(ui->cb_stopJSMonitor->isChecked());
    appConfig->setCSSFlag(ui->cb_stopCSSMonitor->isChecked());
    appConfig->setTemplateFlag(ui->cb_stopTmpltMonitir->isChecked());
    appConfig->setMediaFlag(ui->cb_stopMediaMonitor->isChecked());
    appConfig->writeSettings();
}

void MainWindow::initialize(){

    appConfig = new AppConfig(this);
    initTrayIcon();
    loadSavedSettings();

    qfs_model = new QFileSystemModel;
    qfs_model->setRootPath(str_basePath);
    ui->treeView->setModel(qfs_model);
    ui->treeView->setRootIndex(qfs_model->index(str_rootPath));
    ui->treeView->setIndentation(20);
    ui->treeView->setSortingEnabled(true);
    ui->locSearchText->setAutoFillBackground(true);
    qfsw = new QFileSystemWatcher(this);
    form = new NewInterActivityForm(this);
    form->changeBasePath(str_rootPath);
    setWindowTitle(str_basePath);
    js = new JS(str_basePath);
    tmplt = new Templates(str_basePath);
    config = new ConfigHandler(str_basePath);
    css = new CSS(str_basePath);
    locAcc = new LOCACC(str_basePath);

    ui->locTreeWidget->addTopLevelItem(locAcc->getLocAccTree());
    ui->DEpathText->setText(str_rootPath);

    connectSignals();
    manageLocAccItemsVisibility(-1);
    changeBasePath(str_basePath);

    createLOCTreeContext();
    refreshTabStatus();
}

/*****************************************************************
 ******************** M E N U   H A N D L I N G **********************
 *****************************************************************/

void MainWindow::initTrayIcon()
{
    trayIcon = new QSystemTrayIcon(this);
    QIcon icon(":/images/tray_icon.ico");
    trayIcon->setIcon(icon);
    trayMenu = new QMenu(this);
    trayMenu->addAction("Show",this,SLOT(showApp()));
    trayMenu->addAction("Hide",this,SLOT(hideApp()));
    QMenu *minifyMenu = new QMenu("Minify",this);
    minifyMenu->setIcon(QIcon(":/images/sleeping_mat-48.png"));
    minifyMenu->addAction("Common");
    minifyMenu->addAction("Interactive");
    minifyMenu->addAction("Preloader");
    trayMenu->addMenu(minifyMenu);
    trayMenu->addAction("Compile Handlebars");
    trayMenu->addAction(QIcon(":/images/refresh-48.png"),"Refresh",this,SLOT(scanChanges()));
    trayMenu->addAction(QIcon(":/images/lock-48.png"),"Stop monitoring",this,SLOT(stopMonitoring()));
    trayMenu->addAction(QIcon(":/images/unlock-48.png"),"Start monitoring",this,SLOT(startMonitoring()));
    trayMenu->addAction("Open Loc-acc Tab",this,SLOT(openLocAccTab()));
    trayMenu->addAction("Exit",this,SLOT(close()));
    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();
    showApp();
    stopMonitoring();
}

void MainWindow::openLocAccTab()
{
    this->showApp();
    ui->tabWidget->setCurrentIndex(5);
}

void MainWindow::showApp()
{
    QList<QAction *> actions = trayMenu->actions();
    actions.at(0)->setEnabled(false);
    actions.at(1)->setEnabled(true);
    this->show();
}

void MainWindow::hideApp()
{
    QList<QAction *> actions = trayMenu->actions();
    actions.at(0)->setEnabled(true);
    actions.at(1)->setEnabled(false);
    this->hide();
}

void MainWindow::stopMonitoring()
{
    QList<QAction *> actions = trayMenu->actions();
    actions.at(5)->setEnabled(false);
    actions.at(6)->setEnabled(true);
    setCheckBoxStatus(true);
}

void MainWindow::startMonitoring()
{
    QList<QAction *> actions = trayMenu->actions();
    actions.at(5)->setEnabled(true);
    actions.at(6)->setEnabled(false);
    setCheckBoxStatus(false);
    scanChanges();
}

void MainWindow::setCheckBoxStatus(bool checked)
{
    ui->cb_stopCSSMonitor->setChecked(checked);
    ui->cb_stopJSMonitor->setChecked(checked);
    ui->cb_stopTmpltMonitir->setChecked(checked);
    ui->cb_stopMediaMonitor->setChecked(checked);  
    refreshTabStatus();
}

void MainWindow::refreshTabStatus()
{
    on_cb_stopCSSMonitor_clicked();
    on_cb_stopJSMonitor_clicked();
    on_cb_stopTmpltMonitir_clicked();
    on_cb_stopMediaMonitor_clicked();
}

void MainWindow::createLOCTreeContext()
{
    treeMenu = new QMenu(this);
    treeMenu->addAction("Show",this,SLOT(showApp()));
    treeMenu->addAction("Hide",this,SLOT(hideApp()));
    ui->locTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
 }

// ////////////////////////////////////////////////////////////////////////////////////

void MainWindow::connectSignals(){
    connect(tmplt,SIGNAL(filesChanged(QFileInfoList)),this,SLOT(updateTemplateList(QFileInfoList)));
    connect(ui->templateList,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(templateFileListClicked(QListWidgetItem*)));

    connect(js,SIGNAL(jsfilesChanged(QFileInfoList)),this,SLOT(updateJSList(QFileInfoList)));
    connect(ui->jsViewList,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(jsFileListClicked(QListWidgetItem*)));

    connect(css,SIGNAL(filesChanged(QFileInfoList)),this,SLOT(updateCssList(QFileInfoList)));
    connect(ui->cssList,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(cssFileListClicked(QListWidgetItem*)));

    connect(form,SIGNAL(newInterActivityCreated(QString)),this,SLOT(changeBasePath(QString)));
    connect(form,SIGNAL(newJSONPrepared(QJsonObject)),config,SLOT(newInteractivityCreated(QJsonObject)));

    connect(ui->locTreeWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(onCustomContextMenuRequested(QPoint)));
}

void MainWindow::onCustomContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem* item = ui->locTreeWidget->itemAt(pos);

            if (item) {
                showLocTreeCustomMenu(item,ui->locTreeWidget->viewport()->mapToGlobal(pos));
            }
}

void MainWindow::showLocTreeCustomMenu(QTreeWidgetItem *item, const QPoint &globalPos)
{
    qDebug() << "show request sent";
    treeMenu->show();
    treeMenu->popup(globalPos);
}

MainWindow::~MainWindow()
{
    storeSetting();

    delete locAcc;
    delete appConfig;
    delete js;
    delete css;
    delete tmplt;
    delete config;
    delete ui;
}

void MainWindow::scanChanges(){
    changeBasePath(str_basePath);
}

void MainWindow::registerWatcher(){
    js->registerWatcher();
    tmplt->registerWatcher();
}

void MainWindow::deregisterWatcher(){
    js->deRegisterWatcher();
    tmplt->deRegisterWatcher();
}

/**************************************************************
 *
 *  O V E R V I E W    T A B
 *
 **************************************************************/

void MainWindow::changeBasePath(QString strBasePath)
{
    deregisterWatcher();
    str_basePath = strBasePath;
    config->changeBasePath(str_basePath);
    tmplt->changeBasePath(str_basePath);
    js->changeBasePath(str_basePath);
    css->changeBasePath(str_basePath);
    locAcc->changeBasePath(str_basePath);    
    updateDirTree();
    setWindowTitle("DE-Interactives " + str_basePath);
    on_searchLocBtn_clicked();
    ui->comboBox_Languages->clear();
    ui->comboBox_Languages->addItems(locAcc->getAvailableLangugaes());
    ui->languageLabel->setText(ui->comboBox_Languages->currentText());
    on_comboBox_Languages_currentIndexChanged(0);
}

void MainWindow::on_openDialog_clicked()
{
    deregisterWatcher();
    QString folderName =  qfs_model->itemData(ui->treeView->currentIndex())[Qt::DisplayRole].toString();
    str_basePath = str_rootPath + "/" + folderName;

    ui->statusBar->showMessage("Current interactivity : " + folderName);
    ui->label_interActiveName->setText(folderName);
    changeBasePath(str_basePath);
    form->changeBasePath(str_rootPath);
}

void MainWindow::updateDirTree(){   
    return;
    ui->treeView->setRootIndex(qfs_model->index(str_basePath));
}

void MainWindow::resetAll(){
    qfsw->removePaths(qfsw->files());
    qfsw->removePaths(qfsw->directories());
}

/****************************************************************
             T E M P L A T E   H A N D  L I N G
******************************************************************/

void MainWindow::updateTemplateList(QFileInfoList fileList){
    if(ui->cb_stopTmpltMonitir->isChecked())
    {
        return;
    }
    ui->templateList->clear();
    QJsonObject obj;
    QJsonArray jArray ;
    QString currentFileName ;
    for(int i = 0 ; i < fileList.length() ; i++){
        currentFileName = fileList.at(i).fileName().replace(QString(".handlebars"),QString(".js"));
        obj["url"] = currentFileName;
        obj["isNextStepLoad"] = true;
        jArray.insert(i,obj);
        ui->templateList->addItem(currentFileName);
        // qDebug() << fileList.at(i).fileName();
    }
    ui->templateList->setCurrentRow(0);
    config->setTemplateJArray(syncTmpltList(jArray));
    templateFileListClicked(ui->templateList->currentItem());
    config->writeConfigJson();
}

QJsonArray MainWindow::syncTmpltList(QJsonArray newArray)
{
    QJsonObject tempNewObj,tempOrigObj;
    QJsonArray templateJArray = config->getTemplateJArray();
    for(int i = 0 ; i < newArray.count() ; i++)
    {
        tempNewObj =  newArray.at(i).toObject();
        for(int j = 0 ; j < templateJArray.count() ; j ++)
        {
            tempOrigObj = templateJArray.at(j).toObject();
            if(tempNewObj["url"] == tempOrigObj["url"]){
                tempNewObj["isNextStepLoad"] = tempOrigObj["isNextStepLoad"];
                newArray.replace(i,tempNewObj);
                // qDebug() <<"replced";
            }
        }
    }
    return newArray;
}

void MainWindow::templateFileListClicked(QListWidgetItem *item){
    if(item == NULL)
    {
        return;
    }
    if(ui->cb_stopTmpltMonitir->isChecked())
    {
        return;
    }
    QJsonArray tmpltJArray = config->getTemplateJArray();
    QString itemText = item->text();
    for(int i = 0; i < tmpltJArray.count() ; i++){
        QJsonObject obj = tmpltJArray.at(i).toObject();
        if(obj["url"] == itemText){
            ui->templateNextLoadCheckBox->setChecked(obj["isNextStepLoad"].toBool());
            ui->label_TemplateFileName->setText(itemText);
            break;
        }
    }
}

void MainWindow::on_templateNextLoadCheckBox_clicked()
{
    if(ui->cb_stopTmpltMonitir->isChecked())
    {
        return;
    }
    QJsonArray tmpltJArray = config->getTemplateJArray();
    QListWidgetItem *currentItem  = ui->templateList->currentItem();
    for(int i = 0; i < tmpltJArray.count() ; i++){

        QJsonObject obj = tmpltJArray.at(i).toObject();
        if(obj["url"] == currentItem->text())
        {
            obj["isNextStepLoad"] = ui->templateNextLoadCheckBox->isChecked();
            tmpltJArray.replace(i,obj);
        }
    }
    config->setTemplateJArray(tmpltJArray);
    config->writeConfigJson();
    // qDebug() << "ON CHECK BOX CLICKED : " << tmpltJArray;
}

void MainWindow::on_templateList_itemSelectionChanged()
{
    templateFileListClicked(ui->templateList->currentItem());

}

/****************************************************************
             J S     H A N D L I N G
******************************************************************/

void MainWindow::jsFileListClicked(QListWidgetItem *item){
    if(item == NULL || ui->cb_stopJSMonitor->isChecked())
    {
        return;
    }
    QJsonArray jsViewJArray = config->getJSJArray();
    QString itemText = item->text();
    bool containsBase = false;
    for(int i = 0; i < jsViewJArray.count() ; i++){
        QJsonObject obj = jsViewJArray.at(i).toObject();
        containsBase = obj.contains("basePath");
        ui->jsViewNextLoadCheckBox->setDisabled(containsBase);
        if(obj["url"] == itemText)
        {
            ui->jsViewNextLoadCheckBox->setChecked(obj["isNextStepLoad"].toBool());
            ui->label_JSFileName->setText(itemText);
            break;
        }
    }
}

void MainWindow::on_jsViewList_itemSelectionChanged()
{
    jsFileListClicked(ui->jsViewList->currentItem());
}

void MainWindow::updateJSList(QFileInfoList fileList){
    if(ui->cb_stopJSMonitor->isChecked())
    {
        return;
    }
    ui->jsViewList->clear();
    QJsonArray jsViewJArray;
    QString currentFileName ;
    QJsonObject obj;
    QFileInfoList jsFolderFiles = js->getJSFolderInfoList();
    QFileInfoList modelFolderFiles = js->getModelFileInfoList();
    QFileInfoList viewFolderFiles = js->getViewFileInfoList();

    for(int i = 0 ; i < jsFolderFiles.count() ; i++ ){
        currentFileName = jsFolderFiles.at(i).fileName();
        obj["url"] = currentFileName;
        obj["isNextStepLoad"] = false;
        jsViewJArray.insert(i,obj);
    }

    int addCnt = jsFolderFiles.length();
    for(int i = 0 ; i < modelFolderFiles.count() ; i++ ){
        currentFileName = "models/" + modelFolderFiles.at(i).fileName();
        obj["url"] = currentFileName;
        obj["isNextStepLoad"] = true;
        jsViewJArray.insert(addCnt + i,obj);
    }

    addCnt = jsFolderFiles.length() + modelFolderFiles.length();
    for(int i = 0 ; i < viewFolderFiles.count() ; i++ ){
        currentFileName = "views/" + viewFolderFiles.at(i).fileName();
        obj["url"] = currentFileName;
        obj["isNextStepLoad"] = true;
        jsViewJArray.insert(addCnt +  i,obj);
    }    
    ui->jsViewList->setCurrentRow(0);
    config->setJSJArray(syncJSList(jsViewJArray));
    config->writeConfigJson();
    jsFileListClicked(ui->jsViewList->currentItem());
}


void MainWindow::on_jsViewNextLoadCheckBox_clicked()
{
    if(ui->cb_stopJSMonitor->isChecked())
    {
        return;
    }

    QListWidgetItem *currentItem  = ui->jsViewList->currentItem();
    QJsonArray jsViewJArray = config->getJSJArray();
    for(int i = 0; i < jsViewJArray.count() ; i++){

        QJsonObject obj = jsViewJArray.at(i).toObject();
        if( obj["url"] == currentItem->text())
        {
            obj["isNextStepLoad"] = ui->jsViewNextLoadCheckBox->isChecked();
            jsViewJArray.replace(i,obj);
        }
    }
    config->setJSJArray(jsViewJArray);
    config->writeConfigJson();
    // qDebug() << "ON CHECK BOX CLICKED : JS LIST \n" << jsViewJArray;
}

QJsonArray MainWindow::syncJSList(QJsonArray newArray)
{
    bool isEntryFound = false;
    QList<int> removeIndexList ;
    QJsonObject tempNewObj,tempOrigObj;
    QJsonArray jsOrigArray = config->getJSJArray();
    // SCAN ORIGINAL LIST TO REMOVE UNWANTED ENTRIES
    for(int i = 0 ; i < jsOrigArray.count() ; i++)
    {
        tempOrigObj =   jsOrigArray.at(i).toObject();
        isEntryFound = false;
        for(int j = 0 ; j < newArray.count() ; j ++)
        {
            tempNewObj = newArray.at(j).toObject();
            if(tempNewObj["url"] == tempOrigObj["url"]){
                tempNewObj["isNextStepLoad"] = tempOrigObj["isNextStepLoad"];
                jsOrigArray.replace(i,tempNewObj);
                // qDebug() <<"replced";
                isEntryFound = true;
            }
        }
        if(!isEntryFound)
        {
            removeIndexList.append(i);
        }
    }
    for(int i = 0 ; i < removeIndexList.length() ; i++)
    {
        if(jsOrigArray.at(removeIndexList.at(i)).toObject().contains("basePath"))
        {
            continue;
        }
        jsOrigArray.removeAt(removeIndexList.at(i));
    }

    // SCAN NEW LIST FOR NEW ENTRIES
    removeIndexList.clear();
    for(int i = 0 ; i < newArray.count() ; i++)
    {
        tempNewObj = newArray.at(i).toObject();
        isEntryFound = false;
        for(int j = 0 ; j < jsOrigArray.count() ; j++)
        {
            tempOrigObj = jsOrigArray.at(j).toObject();
            if(tempNewObj["url"] == tempOrigObj["url"])
            {
                isEntryFound = true;
                break;
            }
        }
        if(!isEntryFound)
        {
            removeIndexList.append(i);
        }
    }
    for(int i = 0 ; i < removeIndexList.length() ; i++)
    {
        jsOrigArray.append(newArray.at(removeIndexList.at(i)));
    }

    // Fill the view list of the JS tab
    for(int i = 0 ; i < jsOrigArray.count() ; i++ )
    {
        QString fileName = jsOrigArray.at(i).toObject()["url"].toString();
        ui->jsViewList->addItem(fileName);
    }
    return jsOrigArray;
}

/** J S UP DOWN Buttons
 **/
void MainWindow::on_jsOneUpBtn_clicked()
{
        int currentIndex = ui->jsViewList->currentRow();
        if(currentIndex < 0)
        {
            return;
        }
        QJsonArray jsJArray = config->getJSJArray();
        QJsonObject removedJObj = jsJArray.at(currentIndex).toObject();
        jsJArray.removeAt(currentIndex);

        QListWidgetItem *currentItem = ui->jsViewList->takeItem(currentIndex);
        int newIndex = currentIndex < 1 ? 0 : currentIndex - 1;
        ui->jsViewList->insertItem(newIndex,currentItem);
        ui->jsViewList->setCurrentRow(newIndex);
        jsJArray.insert(newIndex,removedJObj);

        config->setJSJArray(jsJArray);
        config->writeConfigJson();
}

void MainWindow::on_jsOneDownBtn_clicked()
{
    int currentIndex = ui->jsViewList->currentRow();
    if(currentIndex < 0)
    {
        return;
    }
    QJsonArray jsJArray = config->getJSJArray();
    QJsonObject removedJObj = jsJArray.at(currentIndex).toObject();
    jsJArray.removeAt(currentIndex);

    QListWidgetItem *currentItem = ui->jsViewList->takeItem(currentIndex);
    int newIndex = currentIndex > ui->jsViewList->count() - 1 ? ui->jsViewList->count() : currentIndex + 1;
    ui->jsViewList->insertItem(newIndex,currentItem);
    ui->jsViewList->setCurrentRow(newIndex);
    jsJArray.insert(newIndex,removedJObj);

    config->setJSJArray(jsJArray);
    config->writeConfigJson();
}


/****************************************************************
               C S S        H A N D  L I N G
******************************************************************/

void MainWindow::updateCssList(QFileInfoList fileList){
    if(ui->cb_stopCSSMonitor->isChecked())
    {
        return;
    }
    ui->cssList->clear();
    QJsonObject obj;
    QJsonArray cssArray ;
    QString currentFileName ;
    for(int i = 0 ; i < fileList.length() ; i++){
        currentFileName = fileList.at(i).fileName();
        obj["url"] = currentFileName;
        obj["isNextStepLoad"] = true;
        cssArray.insert(i,obj);
        ui->cssList->addItem(currentFileName);
        // qDebug() << fileList.at(i).fileName();
    }
    ui->cssList->setCurrentRow(0);
    config->setCssJArray(syncCSSList(cssArray));
    cssFileListClicked(ui->cssList->currentItem());
    config->writeConfigJson();
}

QJsonArray MainWindow::syncCSSList(QJsonArray newArray)
{
    QJsonObject tempNewObj,tempOrigObj;
    QJsonArray cssJArray = config->getCssJArray();
    for(int i = 0 ; i < newArray.count() ; i++)
    {
        tempNewObj =  newArray.at(i).toObject();
        for(int j = 0 ; j < cssJArray.count() ; j ++)
        {
            tempOrigObj = cssJArray.at(j).toObject();
            if(tempNewObj["url"] == tempOrigObj["url"]){
                tempNewObj["isNextStepLoad"] = tempOrigObj["isNextStepLoad"];
                newArray.replace(i,tempNewObj);
                // qDebug() <<"replced";
            }
        }
    }
    return newArray;
}

void MainWindow::cssFileListClicked(QListWidgetItem *item){
    if(item == NULL || ui->cb_stopCSSMonitor->isChecked())
    {
        return;
    }
    QJsonArray cssJArray = config->getCssJArray();
    QString itemText = item->text();
    for(int i = 0; i < cssJArray.count() ; i++){
        QJsonObject obj = cssJArray.at(i).toObject();
        if(obj["url"] == itemText){
            ui->cssNextLoadCheckBox->setChecked(obj["isNextStepLoad"].toBool());
            ui->label_CSSFileName->setText(itemText);
            break;
        }
    }
}

void MainWindow::on_cssNextLoadCheckBox_clicked()
{
    if(ui->cb_stopCSSMonitor->isChecked())
    {
        return;
    }
    QJsonArray cssJArray = config->getCssJArray();
    QListWidgetItem *currentItem  = ui->cssList->currentItem();
    for(int i = 0; i < cssJArray.count() ; i++){

        QJsonObject obj = cssJArray.at(i).toObject();
        if(obj["url"] == currentItem->text())
        {
            obj["isNextStepLoad"] = ui->cssNextLoadCheckBox->isChecked();
            cssJArray.replace(i,obj);
        }
    }
    config->setCssJArray(cssJArray);
    config->writeConfigJson();
    // qDebug() << "ON CHECK BOX CLICKED CSS: " << cssJArray;
}

void MainWindow::on_cssList_itemSelectionChanged()
{
    cssFileListClicked(ui->cssList->currentItem());
}



/***************************************************************
 *                         L O C - A C C     J S O N     H A N D L I N G
 ***************************************************************/

/*
#################    ADD  MESSAGE   #######################
*/

void MainWindow::on_addScreenBtn_clicked()
{
    QString screenId = ui->screenNameText->text();
    QString screenName = ui->screenNameText_2->text();
    QStringList screenData;
    screenData << screenId << screenName;
    QTreeWidgetItem *addedItem = locAcc->addScreen(screenData);
    if(addedItem == NULL)
    {
        QMessageBox::critical(this,"Same screen Id exists","Same screen Id exists .Please use another Id.",QMessageBox::Cancel);
        return;
    }
    ui->locTreeWidget->setCurrentItem(addedItem,0);
    ui->screenNameText->clear();
    ui->screenNameText_2->clear();
    ui->statusBar->showMessage("New screen successfully addded.",1000);
    ui->screenNameText->setFocus();
}

void MainWindow::on_addEleBtn_clicked()
{
    QString eleName = ui->eleNameText->text();
    QString eleAccId = ui->eleAccIdText->text();
    QString eleRole = ui->eleRoleText->text();
    QString eleType = ui->eleTypeText->text();
    QString eleTabIndex = ui->eleTabIndexText->text();
    QStringList eleData;
    eleData << eleName << eleAccId << eleType << eleRole << eleTabIndex;
    QTreeWidgetItem* addedItem = locAcc->addElement(eleData,ui->locTreeWidget->currentItem());
    if(addedItem == NULL)
    {
        QMessageBox::critical(this,"Same element Id exists in screen.","Same element Id exists .Please use another Id.",QMessageBox::Cancel);
        return;
    }
    ui->locTreeWidget->setCurrentItem(addedItem,0);
    ui->eleAccIdText->clear();
    ui->eleNameText->clear();
    ui->eleRoleText->clear();
    ui->eleTabIndexText->clear();
    ui->eleTypeText->clear();
    ui->statusBar->showMessage("New element successfully addded.",1000);
    ui->eleNameText->setFocus();
}

void MainWindow::on_addMsgBtn_clicked()
{
    QStringList msgData;
    msgData << ui->msgIdText->text() << ui->locMsgText->text() << ui->accMsgText->text();
    QTreeWidgetItem* addedItem = locAcc->addMessage(msgData,ui->cb_isAccTextSame->isChecked(),ui->locTreeWidget->currentItem());
    if(addedItem == NULL)
    {
        QMessageBox::critical(this,"Same message Id exists in element.","Same message Id exists .Please use another Id.",QMessageBox::Cancel);
        return;
    }
    ui->locTreeWidget->expandItem(ui->locTreeWidget->currentItem());
    //ui->locTreeWidget->setCurrentItem(addedItem,0);
    ui->msgIdText->clear();
    ui->locMsgText->clear();
    ui->accMsgText->clear();
    ui->statusBar->showMessage("New message successfully addded.",1000);
    ui->msgIdText->setFocus();
}

/*
#################    UPDATE MESSAGE   #######################
*/

void MainWindow::on_updtScreenBtn_clicked()
{
    QString screenId = ui->updtScreenIdText->text();
    QString screenName = ui->updtScreenNameText->text();
    QStringList screenData;
    screenData << screenId << screenName;
    if(!locAcc->updateScreen(screenData,ui->locTreeWidget->currentItem()))
    {
        QMessageBox::critical(this,"Same screen Id exists","Same screen Id exists .Please use another Id.",QMessageBox::Cancel);
        return;
    }
    ui->statusBar->showMessage("Screen entry successfully updated.",1000);
}

void MainWindow::on_updateMsgBtn_clicked()
{
    QStringList msgData;
    msgData << ui->updtMsgidText->text() << ui->updtLocMsgText->text() << ui->updtAccMsgText->text();
    if(!locAcc->updateMessage(msgData,ui->cb_isAccTextSameUpdt->isChecked(),ui->locTreeWidget->currentItem()))
    {
        QMessageBox::critical(this,"Same message Id exists in element.","Same message Id exists .Please use another Id.",QMessageBox::Cancel);
        return;
    }
    ui->statusBar->showMessage("Message entry successfully updated.",1000);
}

void MainWindow::on_updtEleBtn_clicked()
{
    QString eleName = ui->updtEleIdText->text();
    QString eleAccId = ui->updtEleAccIdText->text();
    QString eleRole = ui->updtEleRoleText->text();
    QString eleType = ui->updtEleTypeText->text();
    QString eleTabIndex = ui->updTabIndexText->text();
    QStringList eleData;
    eleData << eleName << eleAccId << eleType << eleRole << eleTabIndex;
    if(!locAcc->updateElement(eleData,ui->locTreeWidget->currentItem()))
    {
        QMessageBox::critical(this,"Same element Id exists in screen.","Same element Id exists .Please use another Id.",QMessageBox::Cancel);
        return;
    }
    ui->statusBar->showMessage("Element entry successfully updated.",1000);
}

void MainWindow::on_locTreeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    int indentationCount =  getTreeItemIndentationLevel(item);
    // qDebug() << "LOC TREE CLICKED : " << item->text(column) << " col : " << column << " Heirarchy : "
             //<<  indentationCount;
    manageLocAccItemsVisibility(indentationCount);
}

void MainWindow::manageLocAccItemsVisibility(int indentationLevel)
{
    ui->groupBox_addScreen->setVisible(indentationLevel == 0 );
    ui->groupBox_addElement->setVisible(indentationLevel == 1 );
    ui->groupBox_addMessage->setVisible(indentationLevel == 2 );

    ui->groupBox_optScr->setVisible(indentationLevel == 1);
    ui->groupBox_optEle->setVisible(indentationLevel == 2);
    ui->groupBox_optMsg->setVisible(indentationLevel == 3);

    ui->groupBox_updateScreen->setVisible(indentationLevel == 1);
    ui->groupBox_updateElement->setVisible(indentationLevel == 2);
    ui->groupBox_updateMessage->setVisible(indentationLevel == 3);

    updateLocDetails(indentationLevel);
}

void MainWindow::updateLocDetails(int indentationLevel)
{
    QStringList dataList;
    QTreeWidgetItem *currentItem = ui->locTreeWidget->currentItem();
    switch (indentationLevel) {
    case 1:
        dataList =locAcc->getScreenTreeData(currentItem);
        fillScreenDetail(dataList);
        break;
    case 2:
        dataList =locAcc->getElementTreeData(currentItem);
        fillElementDetail(dataList);
        break;
    case 3:
        dataList =locAcc->getMessageTreeData(currentItem);
        fillMessageDetail(dataList);
        ui->locUtilityTabWidget->setCurrentIndex(1);  // Directly set tab to "Update" part for message.
        break;
    default:
        break;
    }
}

void MainWindow::fillScreenDetail(QStringList data)
{
    ui->updtScreenIdText->setText(data.at(0));
    ui->updtScreenNameText->setText(data.at(0));
}

void MainWindow::fillElementDetail(QStringList data)
{
    ui->updtEleIdText->setText(data.at(0));
    ui->updtEleAccIdText->setText(data.at(1));
    ui->updtEleTypeText->setText(data.at(2));
    ui->updtEleRoleText->setText(data.at(3));
    ui->updTabIndexText->setText(data.at(4));
}

void MainWindow::fillMessageDetail(QStringList data)
{
    ui->updtMsgidText->setText(data.at(0));
    ui->cb_isAccTextSameUpdt->setChecked(data.at(1) == "true" ? true : false);
    ui->updtLocMsgText->setText(data.at(2));
    ui->updtAccMsgText->setText(data.at(3));
    // qDebug() << "Message detail updated : " << data.at(1);
    on_cb_isAccTextSameUpdt_clicked();
}

int MainWindow::getTreeItemIndentationLevel(QTreeWidgetItem *currentItem,int count)
{
    QTreeWidgetItem *parent = currentItem->parent();
    if(parent != NULL)
    {
        count = getTreeItemIndentationLevel(parent,count + 1);
    }
    return count;
}

void MainWindow::on_cb_isAccTextSame_clicked()
{
    bool cb_status = ui->cb_isAccTextSame->isChecked();
    ui->accMsgText->setDisabled(cb_status);
    if(cb_status)
    {
        ui->accMsgText->setText(ui->locMsgText->text());
    }
}

void MainWindow::on_locTreeWidget_itemSelectionChanged()
{
    on_locTreeWidget_itemClicked(ui->locTreeWidget->currentItem(),0);
}

void MainWindow::on_locMsgText_textChanged(const QString &arg1)
{
    if(ui->cb_isAccTextSame->isChecked())
    {
        ui->accMsgText->setText(ui->locMsgText->text());
    }
}

void MainWindow::on_cb_isAccTextSameUpdt_clicked()
{
    bool cb_status = ui->cb_isAccTextSameUpdt->isChecked();
    ui->updtAccMsgText->setDisabled(cb_status);
    if(cb_status)
    {
        ui->updtAccMsgText->setText(ui->updtLocMsgText->text());
    }
}

void MainWindow::on_updtLocMsgText_textEdited(const QString &arg1)
{
    if(ui->cb_isAccTextSameUpdt->isChecked())
    {
        ui->updtAccMsgText->setText(ui->updtLocMsgText->text());
    }
}

/***********************************************
 * S E A R C H   R E S U L T   F U N C T I O N
 **********************************************/
void MainWindow::on_searchLocBtn_clicked()
{
    QList<QTreeWidgetItem *> searchedList = locAcc->getSearchResult(ui->locSearchText->text());
    ui->totalIndexText->setText(QString::number(searchedList.length()));
    QTreeWidgetItem *currentResult = locAcc->getCurrentSearchResult();
    ui->currentIndexText->setText(QString::number(locAcc->getCurrentSearchIndex() + 1));
    if(currentResult != NULL)
    {
        ui->locTreeWidget->setCurrentItem(currentResult);
    }
    on_goPrevSearchBtn_clicked();
    if(searchedList.length() == 0)
    {
        ui->locSearchText->setStyleSheet("background-color : pink ; opacity : 0.5;");
    }
    else
    {
        ui->locSearchText->setStyleSheet("background-color: white ; opacity : 1;");
    }
}

void MainWindow::on_goPrevSearchBtn_clicked()
{
    QTreeWidgetItem *itemToHighLight = locAcc->getPrevSeachResult();
    if(itemToHighLight == NULL)
    {
        return;
    }
    ui->currentIndexText->setText(QString::number(locAcc->getCurrentSearchIndex() + 1));
    ui->locTreeWidget->setCurrentItem(itemToHighLight);
}

void MainWindow::on_goNextSearchBtn_clicked()
{
    QTreeWidgetItem *itemToHighLight = locAcc->getNextSearchResult();
    if(itemToHighLight == NULL)
    {
        return;
    }
    ui->currentIndexText->setText(QString::number(locAcc->getCurrentSearchIndex() + 1));
    ui->locTreeWidget->setCurrentItem(itemToHighLight);
}

/*
  #############################################
            D E L E T I O N   O F   T H E   D A T A
*/

void MainWindow::on_dltScrBtn_clicked()
{
    int result = QMessageBox::warning(this,"Are you Sure ???" , "You are about to delete Screen !!! Allow Deletion?",
                                      QMessageBox::Ok, QMessageBox::Cancel);
    if(result == QMessageBox::Ok)
    {
        locAcc->deleteScreen(ui->locTreeWidget->currentItem());
    }
}

void MainWindow::on_dltMsgBtn_clicked()
{
    int result = QMessageBox::warning(this,"Are you Sure ???" , "You are about to delete Message !!! Allow Deletion?",
                                      QMessageBox::Ok, QMessageBox::Cancel);
    if(result == QMessageBox::Ok)
    {
        locAcc->deleteMessage(ui->locTreeWidget->currentItem());
    }

}

void MainWindow::on_dltEleBtn_clicked()
{
    int result = QMessageBox::warning(this,"Are you Sure ???" , "You are about to delete Element !!! Allow Deletion?",
                                      QMessageBox::Ok, QMessageBox::Cancel);
    if(result == QMessageBox::Ok)
    {
        locAcc->deleteElement(ui->locTreeWidget->currentItem());
    }
}

/*****************************************************************
******* M U L T I P L E   L A N G U A G E  S U P P O R T ******
*************************************************************/

void MainWindow::on_comboBox_Languages_currentIndexChanged(int index)
{
    if(index < 0){
        return;
    }
    locAcc->changeLanguage(index);
    ui->languageLabel->setText(ui->comboBox_Languages->currentText());
}

void MainWindow::on_addNewLangBtn_clicked()
{
    QString newLangName = ui->newLangText->text();
    if(!locAcc->addNewLanguage(newLangName))
    {
        QMessageBox::critical(this,"Same language exists","Same language exists. Please enter another language.",QMessageBox::Cancel);
        return;
    }
    ui->comboBox_Languages->addItem(newLangName);
}

/****************************************************************
 ****** C R E A T E S    N E W    I N T E R A C T I V I T Y *****
 ****************************************************************/
void MainWindow::on_createNewInter_clicked()
{
    form->clearAllFormData();
    form->show();
}

/****************************************************************
 ****************** S E T T I N G S    T A B ********************
 ****************************************************************/

void MainWindow::on_browsePathBtn_clicked()
{
    str_rootPath =  QFileDialog::getExistingDirectory(this, tr("Set DE-Interactives Path"),
                                                      "D:\\",QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    qfs_model->setRootPath(str_basePath);
    ui->treeView->setModel(qfs_model);
    ui->treeView->setRootIndex(qfs_model->index(str_rootPath));
    ui->DEpathText->setText(str_rootPath);
}

/****************************************************************
 *********** L O C - A C C    C U T    C O P Y   P A S T E   O P E R A T I O N ********************
 ****************************************************************/

void MainWindow::on_copyEleBtn_clicked()
{

}

void MainWindow::on_cutEleBtn_clicked()
{

}

void MainWindow::on_pasteEleBtn_clicked()
{

}

void MainWindow::on_copyMsgBtn_clicked()
{

}

void MainWindow::on_cutMsgBtn_clicked()
{

}

void MainWindow::on_pasteMsgBtn_clicked()
{

}

void MainWindow::on_copyScrBtn_clicked()
{

}

void MainWindow::on_pasteScrBtn_clicked()
{

}

/*****************************************************
 * MONITORING CHECK BOX HANDLING
 *****************************************************/

void MainWindow::on_cb_stopJSMonitor_clicked()
{
    bool flagStatus = !ui->cb_stopJSMonitor->isChecked();
    if(flagStatus)
    {
        js->scanChanges();
    }
    ui->tabWidget->setTabEnabled(1,flagStatus);
}

void MainWindow::on_cb_stopTmpltMonitir_clicked()
{
    bool flagStatus = !ui->cb_stopTmpltMonitir->isChecked();
    if(flagStatus)
    {
       tmplt->scanChanges();
    }
    ui->tabWidget->setTabEnabled(2,flagStatus);
}

void MainWindow::on_cb_stopCSSMonitor_clicked()
{
    bool flagStatus = !ui->cb_stopCSSMonitor->isChecked();
    if(flagStatus)
    {
       css->scanChanges();
    }
    ui->tabWidget->setTabEnabled(3,flagStatus);
}

void MainWindow::on_cb_stopMediaMonitor_clicked()
{
    bool flagStatus = !ui->cb_stopMediaMonitor->isChecked();
    if(flagStatus)
    {
       //->scanChanges();
    }
    ui->tabWidget->setTabEnabled(4,flagStatus);
}

/*****************************************************
 * UTILITIES OPERATION
 *****************************************************/

QString MainWindow::getCommonFolderPath()
{
    return str_rootPath + "/common";
}

QString MainWindow::getToolsPath()
{
    return str_rootPath + "/common/tool";
}

void MainWindow::on_miniFyCommonBtn_clicked()
{
    QString str_ToolPath = getToolsPath();
}

void MainWindow::on_minifyInterActBtn_clicked()
{
    QString str_ToolPath = getToolsPath();
}

void MainWindow::on_minifyPreLoadBtn_clicked()
{
    QString str_ToolPath = getToolsPath();
}

void MainWindow::on_deleteOrigFilesBtn_clicked()
{
    QString str_ToolPath = getToolsPath();
}

void MainWindow::on_deleteBranchesBtn_clicked()
{
    QString str_ToolPath = getToolsPath();
}
