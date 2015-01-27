#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

QString MINIFY_COMMON = "minify_common.bat";
QString MINIFY_INTERACTIVITY = "minify_interactive.bat";
QString MINIFY_PRELOADER = "minify_preloader.bat";
QString DELETE_ORIG_FILES = "delete-orig-files.bat";
QString DELETE_UNUSED_BRANCH = "delete-unused-branches.bat";
QString LOC_ACC_PLACE_HOLDER = "%@$%";
QString APP_NAME = "WatchDog2";

QString IS_NEXT_STEP_LOAD = "isNextStepLoad";
QString URL = "url";
QString ID = "id";
QString FORCE_LOAD = "forceLoad";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{    
    ui->setupUi(this);
    instanceCheck();
    checkForUpdates();
    initialize();
    showTipDialog();
}

void MainWindow::loadSavedSettings()
{
    m_strRootPath= m_appConfig->getRootPath();
    m_strBasePath = m_appConfig->getCurrentInteractivity();
    ui->cb_stopCSSMonitor->setChecked(m_appConfig->monitorCSS());
    ui->cb_stopJSMonitor->setChecked(m_appConfig->monitorJS());
    ui->cb_stopMediaMonitor->setChecked(m_appConfig->monitorMedia());
    ui->cb_stopTmpltMonitir->setChecked(m_appConfig->monitorTemplates());
    ui->cb_showTips->setChecked(m_appConfig->showTipsOnStartup());
    ui->statusBar->showMessage("Current interactivity : " + getCurrentInteractivityName());
    ui->cb_stopConfigModification->setChecked(m_appConfig->monitorConfig());
    ui->cb_launchOnStartup->setChecked(m_appConfig->startUpLaunch());
    ui->label_interActiveName->setText(getCurrentInteractivityName());
    ui->cb_stopAutoCompile->setChecked(m_appConfig->getAutoCompile());
}

QString MainWindow::getCurrentInteractivityName()
{
   QDir dir(m_strBasePath);
   return dir.dirName();
}

void MainWindow::storeSetting()
{
    m_appConfig->resetFlag();
    m_appConfig->setRootPath(m_strRootPath);
    m_appConfig->setCurrentInteractivity(m_strBasePath);
    m_appConfig->setJSFlag(ui->cb_stopJSMonitor->isChecked());
    m_appConfig->setCSSFlag(ui->cb_stopCSSMonitor->isChecked());
    m_appConfig->setTemplateFlag(ui->cb_stopTmpltMonitir->isChecked());
    m_appConfig->setMediaFlag(ui->cb_stopMediaMonitor->isChecked());
    m_appConfig->setConfigModificationFlag(ui->cb_stopConfigModification->isChecked());
    m_appConfig->setShowTipsOnStartup(ui->cb_showTips->isChecked());
    m_appConfig->setStartUpLaunch(ui->cb_launchOnStartup->isChecked());
    m_appConfig->setAutoCompile(ui->cb_stopAutoCompile->isChecked());
    m_appConfig->writeSettings();
}

void MainWindow::showTipDialog()
{
    if(!ui->cb_showTips->isChecked())
    {
        return;
    }
    int index = m_appConfig->getTipIndex();
    QFile tipsJsonFile("startupTips.json");
    if(!tipsJsonFile.exists())
    {
        return;
    }
    tipsJsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray rawData = tipsJsonFile.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(rawData));
    QJsonObject tipsObj = doc.object();
    index = index >= tipsObj.keys().length() ? 0 : index;
    QMessageBox::information(this,"Did you know ?"
                             ,tipsObj.value(QString::number(index)).toString()
                             + "\n\n Note : You can disable startup tips by unchecking 'Show Tips on start up' checkbox in settings tab."
                             ,QMessageBox::Ok);
    index++;
    m_appConfig->setTipIndex(index);
    tipsJsonFile.close();
}

void MainWindow::initialize(){

    m_qtwiSource = NULL;
    m_appConfig = new AppConfig(this);
    initTrayIcon();
    loadSavedSettings();

    m_qfsModel = new QFileSystemModel;
    m_qfsModel->setRootPath(m_strBasePath);
    ui->treeView->setModel(m_qfsModel);
    ui->treeView->setRootIndex(m_qfsModel->index(m_strRootPath));
    ui->treeView->setIndentation(20);
    ui->treeView->setSortingEnabled(true);
    ui->locSearchText->setAutoFillBackground(true);
    ui->treeView->setColumnHidden(1,true);
    ui->treeView->setColumnHidden(2,true);
    ui->treeView->setColumnHidden(3,true);

    m_qfswMain = new QFileSystemWatcher(this);
    m_form = new NewInterActivityForm(this);
    m_form->changeBasePath(m_strRootPath);
    setWindowTitle(m_strBasePath);
    m_js = new JS(m_strBasePath);
    m_template = new Templates(m_strBasePath);
    m_config = new ConfigHandler(m_strBasePath);
    m_css = new CSS(m_strBasePath);
    m_locAcc = new LOCACC(m_strBasePath);
    m_images = new MediaImages(m_strBasePath);
    m_components = new components(m_strBasePath);

    ui->locTreeWidget->addTopLevelItem(m_locAcc->m_qtwiRoot);
    ui->DEpathText->setText(m_strRootPath);
    createLOCTreeContext();

    connectSignals();
    manageLocAccItemsVisibility(-1);
    changeBasePath(m_strBasePath);

    refreshTabStatus();

    m_exportHelpDialog = new ExportHelp(this);
    m_exportHelpDialog->hide();

}

/*****************************************************************
 ******************** M E N U   H A N D L I N G **********************
 *****************************************************************/

void MainWindow::initTrayIcon()
{
    m_trayIcon = new QSystemTrayIcon(this);
    QIcon icon("tray_icon.png");
    m_trayIcon->setIcon(icon);
    m_qmTrayMenu = new QMenu(this);
    m_qmTrayMenu->addAction("Show",this,SLOT(showApp()));
    m_qmTrayMenu->addAction("Hide",this,SLOT(hideApp()));
    QMenu *minifyMenu = new QMenu("Minify",this);
    minifyMenu->setIcon(QIcon(":/images/sleeping_mat-48.png"));
    minifyMenu->addAction("Common",this,SLOT(on_miniFyCommonBtn_clicked()));
    minifyMenu->addAction("Interactive",this,SLOT(on_minifyInterActBtn_clicked()));
    minifyMenu->addAction("Preloader",this,SLOT(on_minifyPreLoadBtn_clicked()));
    m_qmTrayMenu->addMenu(minifyMenu);
    m_qmTrayMenu->addAction("Compile Handlebars",this,SLOT(compileAllHandleBars()));
    m_qmTrayMenu->addAction(QIcon(":/images/refresh-48.png"),"Refresh",this,SLOT(scanChanges()));
    // TODO : Remove setEnabled from the following two lines
    m_qmTrayMenu->addAction(QIcon(":/images/lock-48.png"),"Stop monitoring",this,SLOT(stopMonitoring()))->setEnabled(true);
    m_qmTrayMenu->addAction(QIcon(":/images/unlock-48.png"),"Start monitoring",this,SLOT(startMonitoring()))->setEnabled(false);
    m_qmTrayMenu->addAction("Open Loc-acc Tab",this,SLOT(openLocAccTab()));
    m_qmTrayMenu->addAction("Open Folder Location",this,SLOT(on_openIntrFolderBtn_clicked()));
    m_qmTrayMenu->addAction("Exit",qApp,SLOT(quit()));
    m_trayIcon->setContextMenu(m_qmTrayMenu);    
    m_trayIcon->show();
    showApp();    
}

void MainWindow::closeEvent(QCloseEvent *closeEvent)
{    
    closeEvent->ignore();
    hideApp();
}

void MainWindow::compileAllHandleBars()
{
    m_template->compileAllHandleBars();
}

void MainWindow::openLocAccTab()
{
    this->showApp();
    ui->tabWidget->setCurrentIndex(5);
}

void MainWindow::showApp()
{
    QList<QAction *> actions = m_qmTrayMenu->actions();
    actions.at(0)->setEnabled(false);
    actions.at(1)->setEnabled(true);
    this->show();
    qApp->setActiveWindow(this);
}

void MainWindow::hideApp()
{
    QList<QAction *> actions = m_qmTrayMenu->actions();
    actions.at(0)->setEnabled(true);
    actions.at(1)->setEnabled(false);
    this->hide();
}

void MainWindow::stopMonitoring()
{
    QList<QAction *> actions = m_qmTrayMenu->actions();
    actions.at(5)->setEnabled(false);
    actions.at(6)->setEnabled(true);
    setCheckBoxStatus(true);
}

void MainWindow::startMonitoring()
{
    QList<QAction *> actions = m_qmTrayMenu->actions();
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
    ui->cb_stopConfigModification->setChecked(checked);
    ui->cb_stopAutoCompile->setChecked(checked);
    refreshTabStatus();
}

void MainWindow::refreshTabStatus()
{
    on_cb_stopCSSMonitor_clicked();
    on_cb_stopJSMonitor_clicked();
    on_cb_stopTmpltMonitir_clicked();
    on_cb_stopMediaMonitor_clicked();
    on_cb_stopConfigModification_clicked();
    on_cb_stopAutoCompile_clicked();
}

// ////////////////////////////////////////////////////////////////////////////////////

void MainWindow::connectSignals(){
    connect(m_template,SIGNAL(filesChanged(QFileInfoList)),this,SLOT(updateTemplateList(QFileInfoList)));
    connect(ui->templateList,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(templateFileListClicked(QListWidgetItem*)));

    connect(m_js,SIGNAL(jsfilesChanged(QFileInfoList)),this,SLOT(updateJSList(QFileInfoList)));
    connect(ui->jsViewList,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(jsFileListClicked(QListWidgetItem*)));

    connect(m_css,SIGNAL(filesChanged(QFileInfoList)),this,SLOT(updateCssList(QFileInfoList)));
    connect(ui->cssList,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(cssFileListClicked(QListWidgetItem*)));

    connect(m_images,SIGNAL(filesChanged(QFileInfoList)),this,SLOT(updateMediaList(QFileInfoList)));
    connect(ui->mediaList,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(mediaFileListClicked(QListWidgetItem*)));

    connect(m_form,SIGNAL(newInterActivityCreated(QString)),this,SLOT(newInterActivityCreated(QString)));
    connect(m_form,SIGNAL(newJSONPrepared(QJsonObject)),m_config,SLOT(newInteractivityCreated(QJsonObject)));

    connect(ui->locTreeWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(onCustomContextMenuRequested(QPoint)));
    connect(m_components,SIGNAL(componentsChanged()),this,SLOT(componentsChanged()));

    connect(ui->actionHelp_File,SIGNAL(triggered()),this,SLOT(on_helpActionTriggered()));
    connect(m_trayIcon,SIGNAL(activated(QSystemTrayIcon::DoubleClick)),this,SLOT(showApp()));

    connect(m_config,SIGNAL(configManuallyModified(QString)),this,SLOT(on_configManuallyModified(QString)));
    connect(m_template,SIGNAL(batchDoNotExists()),this,SLOT(createTemplateBatchFiles()));
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
    m_qmTreeMenu->show();
    m_qmTreeMenu->popup(globalPos);
}

MainWindow::~MainWindow()
{
    storeSetting();    

    delete m_locAcc;
    delete m_appConfig;
    delete m_js;
    delete m_css;
    delete m_template;
    delete m_config;
    delete m_form;
    delete ui;
}

void MainWindow::scanChanges(){
    changeBasePath(m_strBasePath);
}

void MainWindow::registerWatcher(){
    m_js->registerWatcher();
    m_template->registerWatcher();
}

void MainWindow::deregisterWatcher(){
    m_js->deRegisterWatcher();
    m_template->deRegisterWatcher();
}

/**************************************************************
 *
 *  O V E R V I E W    T A B
 *
 **************************************************************/

void MainWindow::newInterActivityCreated(QString path)
{
    changeBasePath(path);    
    m_config->setConfigUpdateFlag(false);
    m_config->changeLoadStepOfFiles();
    m_config->writeConfigJson();    
    stopMonitoring();
    startMonitoring();
    compileAllHandleBars();
}

void MainWindow::changeBasePath(QString strBasePath)
{
    ui->imageIdText->setText("");
    deregisterWatcher();
    m_strBasePath = strBasePath;
    m_config->changeBasePath(m_strBasePath);
    m_template->changeBasePath(m_strBasePath);
    m_js->changeBasePath(m_strBasePath);
    m_css->changeBasePath(m_strBasePath);
    m_locAcc->changeBasePath(m_strBasePath);
    m_images->changeBasePath(m_strBasePath);
    m_components->setCommonPath(getCommonFolderPath());
    m_components->setUsedComponentList(m_config->getComponentList());
    updateDirTree();
    setWindowTitle("DE-Interactives " + m_strBasePath);
    on_searchLocBtn_clicked();
    ui->comboBox_Languages->clear();
    ui->comboBox_Languages->addItems(m_locAcc->getAvailableLangugaes());
    ui->languageLabel->setText(ui->comboBox_Languages->currentText());
    on_comboBox_Languages_currentIndexChanged(0);
    ui->label_interActiveName->setText(getCurrentInteractivityName());
    ui->statusBar->showMessage("Current interactivity : " + getCurrentInteractivityName());
}

void MainWindow::on_openDialog_clicked()
{
    deregisterWatcher();
    if(QMessageBox::warning(this,"Are you sure ?","You are about to change interactivity folder. "
                             "If this is not you wanted then click cancel."
                            "\nAll the monitoring will be stopped on clicking Ok."
                            " \n\nNote : You need to uncheck check boxes to start monitoring."
                            ,QMessageBox::Ok,QMessageBox::Cancel)
            == QMessageBox::Ok)
    {
        stopMonitoring();
        QString folderName =  m_qfsModel->itemData(ui->treeView->currentIndex())[Qt::DisplayRole].toString();
        m_strBasePath = m_strRootPath + "/" + folderName;

        changeBasePath(m_strBasePath);
        m_form->changeBasePath(m_strRootPath);        
    }
    storeSetting();
}

void MainWindow::updateDirTree(){   
    return;
    ui->treeView->setRootIndex(m_qfsModel->index(m_strBasePath));
}

void MainWindow::resetAll(){
    m_qfswMain->removePaths(m_qfswMain->files());
    m_qfswMain->removePaths(m_qfswMain->directories());
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
        obj[URL] = currentFileName;
        obj[IS_NEXT_STEP_LOAD] = true;
        jArray.insert(i,obj);
        ui->templateList->addItem(currentFileName);
        // qDebug() << fileList.at(i).fileName();
    }
    ui->templateList->setCurrentRow(0);
    m_config->setTemplateJArray(syncTmpltList(jArray));
    templateFileListClicked(ui->templateList->currentItem());
    m_config->writeConfigJson();
}

QJsonArray MainWindow::syncTmpltList(QJsonArray newArray)
{
    QJsonObject tempNewObj,tempOrigObj;
    QJsonArray templateJArray = m_config->getTemplateJArray();
    for(int i = 0 ; i < newArray.count() ; i++)
    {
        tempNewObj =  newArray.at(i).toObject();
        for(int j = 0 ; j < templateJArray.count() ; j ++)
        {
            tempOrigObj = templateJArray.at(j).toObject();
            if(tempNewObj[URL] == tempOrigObj[URL]){
                tempNewObj[IS_NEXT_STEP_LOAD] = tempOrigObj[IS_NEXT_STEP_LOAD];
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
    QJsonArray tmpltJArray = m_config->getTemplateJArray();
    QString itemText = item->text();
    for(int i = 0; i < tmpltJArray.count() ; i++){
        QJsonObject obj = tmpltJArray.at(i).toObject();
        if(obj[URL] == itemText){
            ui->templateNextLoadCheckBox->setChecked(obj[IS_NEXT_STEP_LOAD].toBool());
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
    QJsonArray tmpltJArray = m_config->getTemplateJArray();
    QListWidgetItem *currentItem  = ui->templateList->currentItem();
    for(int i = 0; i < tmpltJArray.count() ; i++){

        QJsonObject obj = tmpltJArray.at(i).toObject();
        if(obj[URL] == currentItem->text())
        {
            obj[IS_NEXT_STEP_LOAD] = ui->templateNextLoadCheckBox->isChecked();
            tmpltJArray.replace(i,obj);
        }
    }
    m_config->setTemplateJArray(tmpltJArray);
    m_config->writeConfigJson();
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
    QJsonArray jsViewJArray = m_config->getJSJArray();
    QString itemText = item->text();
    bool containsBase = false;
    for(int i = 0; i < jsViewJArray.count() ; i++){
        QJsonObject obj = jsViewJArray.at(i).toObject();
        containsBase = obj.contains("basePath");
        ui->jsViewNextLoadCheckBox->setDisabled(containsBase);
        if(obj[URL] == itemText)
        {
            ui->jsViewNextLoadCheckBox->setChecked(obj[IS_NEXT_STEP_LOAD].toBool());
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
    QFileInfoList jsFolderFiles = m_js->getJSFolderInfoList();
    QFileInfoList modelFolderFiles = m_js->getModelFileInfoList();
    QFileInfoList viewFolderFiles = m_js->getViewFileInfoList();

    for(int i = 0 ; i < jsFolderFiles.count() ; i++ ){
        currentFileName = jsFolderFiles.at(i).fileName();
        obj[URL] = currentFileName;
        obj[IS_NEXT_STEP_LOAD] = false;
        jsViewJArray.insert(i,obj);
    }

    int addCnt = jsFolderFiles.length();
    for(int i = 0 ; i < modelFolderFiles.count() ; i++ ){
        currentFileName = "models/" + modelFolderFiles.at(i).fileName();
        obj[URL] = currentFileName;
        obj[IS_NEXT_STEP_LOAD] = true;
        jsViewJArray.insert(addCnt + i,obj);
    }

    addCnt = jsFolderFiles.length() + modelFolderFiles.length();
    for(int i = 0 ; i < viewFolderFiles.count() ; i++ ){
        currentFileName = "views/" + viewFolderFiles.at(i).fileName();
        obj[URL] = currentFileName;
        obj[IS_NEXT_STEP_LOAD] = true;
        jsViewJArray.insert(addCnt +  i,obj);
    }    
    ui->jsViewList->setCurrentRow(0);
    m_config->setJSJArray(syncJSList(jsViewJArray));
    m_config->writeConfigJson();
    jsFileListClicked(ui->jsViewList->currentItem());
}


void MainWindow::on_jsViewNextLoadCheckBox_clicked()
{
    if(ui->cb_stopJSMonitor->isChecked())
    {
        return;
    }

    QListWidgetItem *currentItem  = ui->jsViewList->currentItem();
    QJsonArray jsViewJArray = m_config->getJSJArray();
    for(int i = 0; i < jsViewJArray.count() ; i++){

        QJsonObject obj = jsViewJArray.at(i).toObject();
        if( obj[URL] == currentItem->text())
        {
            obj[IS_NEXT_STEP_LOAD] = ui->jsViewNextLoadCheckBox->isChecked();
            jsViewJArray.replace(i,obj);
        }
    }
    m_config->setJSJArray(jsViewJArray);
    m_config->writeConfigJson();
    // qDebug() << "ON CHECK BOX CLICKED : JS LIST \n" << jsViewJArray;
}

QJsonArray MainWindow::syncJSList(QJsonArray newArray)
{
    bool isEntryFound = false;
    QList<int> removeIndexList ;
    QJsonObject tempNewObj,tempOrigObj;
    QJsonArray jsOrigArray = m_config->getJSJArray();
    // SCAN ORIGINAL LIST TO REMOVE UNWANTED ENTRIES
    for(int i = 0 ; i < jsOrigArray.count() ; i++)
    {
        tempOrigObj =   jsOrigArray.at(i).toObject();
        isEntryFound = false;
        for(int j = 0 ; j < newArray.count() ; j ++)
        {
            tempNewObj = newArray.at(j).toObject();
            if(tempNewObj[URL] == tempOrigObj[URL]){
                tempNewObj[IS_NEXT_STEP_LOAD] = tempOrigObj[IS_NEXT_STEP_LOAD];
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
            if(tempNewObj[URL] == tempOrigObj[URL])
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
        QString fileName = jsOrigArray.at(i).toObject()[URL].toString();
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
        QJsonArray jsJArray = m_config->getJSJArray();
        QJsonObject removedJObj = jsJArray.at(currentIndex).toObject();
        jsJArray.removeAt(currentIndex);

        QListWidgetItem *currentItem = ui->jsViewList->takeItem(currentIndex);
        int newIndex = currentIndex < 1 ? 0 : currentIndex - 1;
        ui->jsViewList->insertItem(newIndex,currentItem);
        ui->jsViewList->setCurrentRow(newIndex);
        jsJArray.insert(newIndex,removedJObj);

        m_config->setJSJArray(jsJArray);
        m_config->writeConfigJson();
}

void MainWindow::on_jsOneDownBtn_clicked()
{
    int currentIndex = ui->jsViewList->currentRow();
    if(currentIndex < 0)
    {
        return;
    }
    QJsonArray jsJArray = m_config->getJSJArray();
    QJsonObject removedJObj = jsJArray.at(currentIndex).toObject();
    jsJArray.removeAt(currentIndex);

    QListWidgetItem *currentItem = ui->jsViewList->takeItem(currentIndex);
    int newIndex = currentIndex > ui->jsViewList->count() - 1 ? ui->jsViewList->count() : currentIndex + 1;
    ui->jsViewList->insertItem(newIndex,currentItem);
    ui->jsViewList->setCurrentRow(newIndex);
    jsJArray.insert(newIndex,removedJObj);

    m_config->setJSJArray(jsJArray);
    m_config->writeConfigJson();
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
        obj[URL] = currentFileName;
        obj[IS_NEXT_STEP_LOAD] = true;
        cssArray.insert(i,obj);
        ui->cssList->addItem(currentFileName);
        // qDebug() << fileList.at(i).fileName();
    }
    ui->cssList->setCurrentRow(0);
    m_config->setCssJArray(syncCSSList(cssArray));
    cssFileListClicked(ui->cssList->currentItem());
    m_config->writeConfigJson();
}

QJsonArray MainWindow::syncCSSList(QJsonArray newArray)
{
    QJsonObject tempNewObj,tempOrigObj;
    QJsonArray cssJArray = m_config->getCssJArray();
    for(int i = 0 ; i < newArray.count() ; i++)
    {
        tempNewObj =  newArray.at(i).toObject();
        for(int j = 0 ; j < cssJArray.count() ; j ++)
        {
            tempOrigObj = cssJArray.at(j).toObject();
            if(tempNewObj[URL] == tempOrigObj[URL]){
                tempNewObj[IS_NEXT_STEP_LOAD] = tempOrigObj[IS_NEXT_STEP_LOAD];
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
    QJsonArray cssJArray = m_config->getCssJArray();
    QString itemText = item->text();
    for(int i = 0; i < cssJArray.count() ; i++){
        QJsonObject obj = cssJArray.at(i).toObject();
        if(obj[URL] == itemText){
            ui->cssNextLoadCheckBox->setChecked(obj[IS_NEXT_STEP_LOAD].toBool());
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
    QJsonArray cssJArray = m_config->getCssJArray();
    QListWidgetItem *currentItem  = ui->cssList->currentItem();
    for(int i = 0; i < cssJArray.count() ; i++){

        QJsonObject obj = cssJArray.at(i).toObject();
        if(obj[URL] == currentItem->text())
        {
            obj[IS_NEXT_STEP_LOAD] = ui->cssNextLoadCheckBox->isChecked();
            cssJArray.replace(i,obj);
        }
    }
    m_config->setCssJArray(cssJArray);
    m_config->writeConfigJson();
    // qDebug() << "ON CHECK BOX CLICKED CSS: " << cssJArray;
}

void MainWindow::on_cssList_itemSelectionChanged()
{
    cssFileListClicked(ui->cssList->currentItem());
}

/***************************************************************
 *                         M E D I A      I M A G E S      H A N D L I N G
 ***************************************************************/

void MainWindow::updateMediaList(QFileInfoList fileList){
    if(ui->cb_stopMediaMonitor->isChecked())
    {
        return;
    }
    ui->mediaList->clear();
    QJsonObject obj;
    QJsonArray imgesArray ;
    QString currentFileName ;
    for(int i = 0 ; i < fileList.length() ; i++){
        currentFileName = fileList.at(i).fileName();
        obj[ID] = currentFileName.split(".")[0];
        obj[URL] = currentFileName;
        obj[IS_NEXT_STEP_LOAD] = true;
        obj[FORCE_LOAD] = false;
        imgesArray.insert(i,obj);
        ui->mediaList->addItem(currentFileName);
        // qDebug() << fileList.at(i).fileName();
    }
    ui->mediaList->setCurrentRow(0);
    m_config->setImagesJArray(syncImgsList(imgesArray));
    mediaFileListClicked(ui->mediaList->currentItem());
    m_config->writeConfigJson();
}

QJsonArray MainWindow::syncImgsList(QJsonArray newArray)
{
    QJsonObject tempNewObj,tempOrigObj;
    QJsonArray mediaJArray = m_config->getImagesJArray();
    for(int i = 0 ; i < newArray.count() ; i++)
    {
        tempNewObj =  newArray.at(i).toObject();
        for(int j = 0 ; j < mediaJArray.count() ; j ++)
        {
            tempOrigObj = mediaJArray.at(j).toObject();
            if(tempNewObj[URL] == tempOrigObj[URL]){
                tempNewObj[IS_NEXT_STEP_LOAD] = tempOrigObj[IS_NEXT_STEP_LOAD];
                tempNewObj[FORCE_LOAD] = tempOrigObj[FORCE_LOAD];
                newArray.replace(i,tempNewObj);
                // qDebug() <<"replced";
            }
        }
    }
    return newArray;
}

void MainWindow::mediaFileListClicked(QListWidgetItem *item){
    if(item == NULL || ui->cb_stopMediaMonitor->isChecked())
    {
        return;
    }
    QJsonArray imagesJArray = m_config->getImagesJArray();
    QString itemText = item->text();
    for(int i = 0; i < imagesJArray.count() ; i++){
        QJsonObject obj = imagesJArray.at(i).toObject();
        if(obj[URL] == itemText){
            ui->cb_mediaIsNextStopLoad->setChecked(obj[IS_NEXT_STEP_LOAD].toBool());
            ui->cb_mediaForceLoad->setChecked(obj[FORCE_LOAD].toBool());
            ui->imageIdText->setText(obj[ID].toString());
            break;
        }
    }
}

void MainWindow::on_cb_mediaForceLoad_clicked()
{
    if(ui->cb_stopMediaMonitor->isChecked())
    {
        return;
    }
    QJsonArray mediaJArray = m_config->getImagesJArray();
    QListWidgetItem *currentItem  = ui->mediaList->currentItem();
    for(int i = 0; i < mediaJArray.count() ; i++){

        QJsonObject obj = mediaJArray.at(i).toObject();
        if(obj[URL] == currentItem->text())
        {
            obj[FORCE_LOAD] = ui->cb_mediaForceLoad->isChecked();
            mediaJArray.replace(i,obj);
        }
    }
    m_config->setImagesJArray(mediaJArray);
    m_config->writeConfigJson();
}

void MainWindow::on_cb_mediaIsNextStopLoad_clicked()
{
    if(ui->cb_stopMediaMonitor->isChecked())
    {
        return;
    }
    QJsonArray mediaJArray = m_config->getImagesJArray();
    QListWidgetItem *currentItem  = ui->mediaList->currentItem();
    for(int i = 0; i < mediaJArray.count() ; i++){

        QJsonObject obj = mediaJArray.at(i).toObject();
        if(obj[URL] == currentItem->text())
        {
            obj[IS_NEXT_STEP_LOAD] = ui->cb_mediaIsNextStopLoad->isChecked();
            mediaJArray.replace(i,obj);
        }
    }
    m_config->setImagesJArray(mediaJArray);
    m_config->writeConfigJson();
    // qDebug() << "ON CHECK BOX CLICKED CSS: " << cssJArray;
}

void MainWindow::on_mediaList_itemSelectionChanged()
{
    mediaFileListClicked(ui->mediaList->currentItem());
}

void MainWindow::on_updateImgBtn_clicked()
{
    if(ui->cb_stopMediaMonitor->isChecked())
    {
        return;
    }
    QJsonArray mediaJArray = m_config->getImagesJArray();
    QListWidgetItem *currentItem  = ui->mediaList->currentItem();
    for(int i = 0; i < mediaJArray.count() ; i++){

        QJsonObject obj = mediaJArray.at(i).toObject();
        if(obj[URL] == currentItem->text())
        {
            obj[ID] = ui->imageIdText->text();
            mediaJArray.replace(i,obj);
        }
    }
    m_config->setImagesJArray(mediaJArray);
    m_config->writeConfigJson();
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
    QTreeWidgetItem *addedItem = m_locAcc->addScreen(screenData);
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
    QTreeWidgetItem* addedItem = m_locAcc->addElement(eleData,ui->locTreeWidget->currentItem());
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
    QTreeWidgetItem* addedItem = m_locAcc->addMessage(msgData,ui->cb_isAccTextSame->isChecked(),ui->locTreeWidget->currentItem());
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
    if(!m_locAcc->updateScreen(screenData,ui->locTreeWidget->currentItem()))
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
    if(!m_locAcc->updateMessage(msgData,ui->cb_isAccTextSameUpdt->isChecked(),ui->locTreeWidget->currentItem()))
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
    QString eleOffsetTop = ui->updtOffTopText->text();
    QString eleOffsetLeft = ui->updtOffLeftText->text();
    QStringList eleData;
    eleData << eleName << eleAccId << eleType << eleRole << eleTabIndex << eleOffsetTop << eleOffsetLeft;
    if(!m_locAcc->updateElement(eleData,ui->locTreeWidget->currentItem()))
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

    ui->groupBox_updateScreen->setVisible(indentationLevel == 1);
    ui->groupBox_updateElement->setVisible(indentationLevel == 2);
    ui->groupBox_updateMessage->setVisible(indentationLevel == 3);

    ui->screenUpBtn->setVisible(indentationLevel == 1 );
    ui->screenDownBtn->setVisible(indentationLevel == 1 );
    updateLocDetails(indentationLevel);
    contextMenuVisibility();
}

void MainWindow::updateLocDetails(int indentationLevel)
{
    QStringList dataList;
    QTreeWidgetItem *currentItem = ui->locTreeWidget->currentItem();

    switch (indentationLevel) {
    case 1:
        dataList =m_locAcc->getScreenTreeData(currentItem);
        fillScreenDetail(dataList);
        break;
    case 2:
        dataList =m_locAcc->getElementTreeData(currentItem);
        fillElementDetail(dataList);
        break;
    case 3:
        dataList =m_locAcc->getMessageTreeData(currentItem);
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
    ui->updtScreenNameText->setText(data.at(1));
}

void MainWindow::fillElementDetail(QStringList data)
{
    ui->updtEleIdText->setText(data.at(0));
    ui->updtEleAccIdText->setText(data.at(1));
    ui->updtEleTypeText->setText(data.at(2));
    ui->updtEleRoleText->setText(data.at(3));
    ui->updTabIndexText->setText(data.at(4));
    ui->updtOffTopText->setText(data.at(5));
    ui->updtOffLeftText->setText(data.at(6));
}

void MainWindow::fillMessageDetail(QStringList data)
{
    ui->updtMsgidText->setText(data.at(0));
    ui->cb_isAccTextSameUpdt->setChecked(data.at(1) == "true" ? true : false);
    ui->updtLocMsgText->setText(data.at(2));
    ui->updtAccMsgText->setText(data.at(3));
    ui->label_commonId->setVisible(data.at(4).length() > 0);

    if(data.at(4).length() > 0)
    {
        ui->label_commonId->setText("CommonId : " + data.at(4));
    }
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
    QList<QTreeWidgetItem *> searchedList = m_locAcc->getSearchResult(ui->locSearchText->text());
    ui->totalIndexText->setText(QString::number(searchedList.length()));
    QTreeWidgetItem *currentResult = m_locAcc->getCurrentSearchResult();
    ui->currentIndexText->setText(QString::number(m_locAcc->getCurrentSearchIndex() + 1));
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
    QTreeWidgetItem *itemToHighLight = m_locAcc->getPrevSeachResult();
    if(itemToHighLight == NULL)
    {
        return;
    }
    ui->currentIndexText->setText(QString::number(m_locAcc->getCurrentSearchIndex() + 1));
    ui->locTreeWidget->setCurrentItem(itemToHighLight);
}

void MainWindow::on_goNextSearchBtn_clicked()
{
    QTreeWidgetItem *itemToHighLight = m_locAcc->getNextSearchResult();
    if(itemToHighLight == NULL)
    {
        return;
    }
    ui->currentIndexText->setText(QString::number(m_locAcc->getCurrentSearchIndex() + 1));
    ui->locTreeWidget->setCurrentItem(itemToHighLight);
}

/*
  #############################################
            D E L E T I O N   O F   T H E   D A T A
*/

void MainWindow::deleteScreen()
{
    int result = QMessageBox::warning(this,"Are you Sure ???" , "You are about to delete Screen !!! Allow Deletion?",
                                      QMessageBox::Ok, QMessageBox::Cancel);
    if(result == QMessageBox::Ok)
    {
        m_locAcc->deleteScreen(ui->locTreeWidget->currentItem());
    }
}

void MainWindow::deleteMessage()
{
    int result = QMessageBox::warning(this,"Are you Sure ???" , "You are about to delete Message !!! Allow Deletion?",
                                      QMessageBox::Ok, QMessageBox::Cancel);
    if(result == QMessageBox::Ok)
    {
        m_locAcc->deleteMessage(ui->locTreeWidget->currentItem());
    }

}

void MainWindow::deleteElement()
{
    int result = QMessageBox::warning(this,"Are you Sure ???" , "You are about to delete Element !!! Allow Deletion?",
                                      QMessageBox::Ok, QMessageBox::Cancel);
    if(result == QMessageBox::Ok)
    {
        m_locAcc->deleteElement(ui->locTreeWidget->currentItem());
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
    m_locAcc->changeLanguage(index);
    ui->languageLabel->setText(ui->comboBox_Languages->currentText());
}

void MainWindow::on_addNewLangBtn_clicked()
{
    QString newLangName = ui->newLangText->text();
    if(!m_locAcc->addNewLanguage(newLangName))
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
    m_form->clearAllFormData();
    m_form->show();
}

/****************************************************************
 ****************** S E T T I N G S    T A B ********************
 ****************************************************************/

void MainWindow::on_browsePathBtn_clicked()
{
    m_strRootPath =  QFileDialog::getExistingDirectory(this, tr("Set DE-Interactives Path"),
                                                      "D:\\",QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    m_qfsModel->setRootPath(m_strBasePath);
    ui->treeView->setModel(m_qfsModel);
    ui->treeView->setRootIndex(m_qfsModel->index(m_strRootPath));
    ui->DEpathText->setText(m_strRootPath);
    m_form->changeBasePath(m_strRootPath);
}

/****************************************************************
 * ************** L O C - A C C       C O N T E X T      M E N U  ******************
 *********** L O C - A C C    C U T    C O P Y   P A S T E   O P E R A T I O N ********************
 ****************************************************************/

void MainWindow::createLOCTreeContext()
{
    m_qmTreeMenu = new QMenu(this);
    QSignalMapper *mapper = new QSignalMapper(this);
    QAction *cutAction = m_qmTreeMenu->addAction("Cut",mapper,SLOT(map()));
    QAction *copyAction = m_qmTreeMenu->addAction("Copy",mapper,SLOT(map()));
    QAction *pasteAction = m_qmTreeMenu->addAction("Paste",mapper,SLOT(map()));
    QAction *deleteAction = m_qmTreeMenu->addAction("Delete",mapper,SLOT(map()));

    cutAction->setEnabled(false);
    copyAction->setEnabled(false);
    pasteAction->setEnabled(false);
    deleteAction->setEnabled(false);

    mapper->setMapping(cutAction,0);
    mapper->setMapping(copyAction,1);
    mapper->setMapping(pasteAction,2);
    mapper->setMapping(deleteAction,3);

    connect(mapper,SIGNAL(mapped(int)),this,SLOT(performLocOpertions(int)));

     ui->locTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
 }

void MainWindow::performLocOpertions(int operation)
{
    QTreeWidgetItem *currentItem = ui->locTreeWidget->currentItem();
    int indentationLevel = getTreeItemIndentationLevel(currentItem);    
    switch (operation) {
        case 0:
            m_qtwiSource = ui->locTreeWidget->currentItem();
            m_eOperation = O_CUT;           
            break;
        case 1:
            m_qtwiSource = ui->locTreeWidget->currentItem();            
            m_eOperation = O_COPY;
            break;
        case 2:
                if(m_qtwiSource)
                {
                    switch(indentationLevel)
                    {
                        case 0 :
                            ui->locTreeWidget->setCurrentItem( m_locAcc->cloneScreen(m_qtwiSource));
                            if(m_eOperation == O_CUT)
                            {
                                m_locAcc->deleteScreen(m_qtwiSource);
                            }
                            break;
                        case 1 :
                            ui->locTreeWidget->setCurrentItem( m_locAcc->cloneElement(m_qtwiSource,currentItem));
                            if(m_eOperation == O_CUT)
                            {
                                m_locAcc->deleteElement(m_qtwiSource);
                            }
                            break;
                        case 2 :
                            ui->locTreeWidget->setCurrentItem( m_locAcc->cloneMessage(m_qtwiSource,currentItem));
                            if(m_eOperation == O_CUT)
                            {
                                m_locAcc->deleteMessage(m_qtwiSource);
                            }
                            break;
                    }
                }                
                break;
        case 3:
                        switch(indentationLevel)
                        {
                            case 1 :
                                deleteScreen();
                                break;
                            case 2 :
                                deleteElement();
                                break;
                            case 3 :
                                deleteMessage();
                                break;
                        }
                    m_qmTreeMenu->actions()[2]->setEnabled(false);                    
                    break;
    }
}

void MainWindow::contextMenuVisibility()
{
    QTreeWidgetItem *currentItem = ui->locTreeWidget->currentItem();
    if(currentItem == NULL)
    {
        return;
    }
    int currentItemIndentation  = getTreeItemIndentationLevel(currentItem);

    QList<QAction *> locTreeMenu = m_qmTreeMenu->actions();
    for(int i=0; i < locTreeMenu.length(); i++)
    {
        m_qmTreeMenu->actions()[i]->setEnabled(currentItemIndentation != 0);
    }

    if(m_qtwiSource != NULL)
    {
        int sourceItemIndentation = -1;
        sourceItemIndentation = getTreeItemIndentationLevel(m_qtwiSource);
        m_qmTreeMenu->actions()[2]->setEnabled((sourceItemIndentation - currentItemIndentation) == 1);
    }
    else
    {
        m_qmTreeMenu->actions()[2]->setEnabled(false);
    }
}

/*****************************************************
 * MONITORING CHECK BOX HANDLING
 *****************************************************/

void MainWindow::on_cb_stopJSMonitor_clicked()
{
    bool flagStatus = !ui->cb_stopJSMonitor->isChecked();
    if(flagStatus)
    {
        m_js->scanChanges();
    }   
    ui->tabWidget->setTabEnabled(1,flagStatus);
    changeIcon(ui->cb_stopJSMonitor);
    storeSetting();
}

void MainWindow::on_cb_stopTmpltMonitir_clicked()
{
    bool flagStatus = !ui->cb_stopTmpltMonitir->isChecked();
    if(flagStatus)
    {
       m_template->scanChanges();
    }
    ui->tabWidget->setTabEnabled(2,flagStatus);
    changeIcon(ui->cb_stopTmpltMonitir);
    storeSetting();
}

void MainWindow::on_cb_stopCSSMonitor_clicked()
{
    bool flagStatus = !ui->cb_stopCSSMonitor->isChecked();
    if(flagStatus)
    {
       m_css->scanChanges();
    }
    ui->tabWidget->setTabEnabled(3,flagStatus);
    changeIcon(ui->cb_stopCSSMonitor);
    storeSetting();
}

void MainWindow::on_cb_stopMediaMonitor_clicked()
{
    bool flagStatus = !ui->cb_stopMediaMonitor->isChecked();
    if(flagStatus)
    {
       m_images->scanChanges();
    }
    ui->tabWidget->setTabEnabled(4,flagStatus);
    changeIcon(ui->cb_stopMediaMonitor);
    storeSetting();
}

/*****************************************************
 * UTILITIES OPERATION
 *****************************************************/

QString MainWindow::getCommonFolderPath()
{
    return m_strRootPath + "/common";
}

QString MainWindow::getCommonLocAccFilePath()
{
    return m_strRootPath + "/common/lang/en/data/loc-acc.json";
}

QString MainWindow::getToolsPath()
{
    return m_strRootPath + "/common/tool";
}

QString MainWindow::getMinificationFolderPath()
{
    return getToolsPath() + "/" + "minification-batch-files";
}

/***************************************************
 * M I N I F I C A T I O N    R E L A T E D
***************************************************/

void MainWindow::on_miniFyCommonBtn_clicked()
{
    QString str_MiniFyPath = getMinificationFolderPath();
    QProcess *process = new QProcess();    
    process->startDetached(str_MiniFyPath + "/" + MINIFY_COMMON,QStringList(),
                           str_MiniFyPath);
    process->deleteLater();
}

void MainWindow::on_minifyInterActBtn_clicked()
{
    QString str_MiniFyPath = getMinificationFolderPath();
    QProcess *process = new QProcess();
    QStringList args;
    args << getCurrentInteractivityName();
    process->startDetached(str_MiniFyPath + "/" + MINIFY_INTERACTIVITY,args,str_MiniFyPath);
    process->deleteLater();    
}

void MainWindow::on_minifyPreLoadBtn_clicked()
{
    QString str_MiniFyPath = getMinificationFolderPath();
    QProcess *process = new QProcess();
    process->startDetached(str_MiniFyPath + "/" + MINIFY_PRELOADER,QStringList(),
                           str_MiniFyPath);
    process->deleteLater();
}

void MainWindow::on_deleteOrigFilesBtn_clicked()
{
    QString str_ToolPath = getToolsPath();
    QProcess *process = new QProcess();
    process->startDetached(str_ToolPath + "/" + DELETE_ORIG_FILES,QStringList(),
                           str_ToolPath);
    process->deleteLater();
}

void MainWindow::on_deleteBranchesBtn_clicked()
{
    QString str_ToolPath = getToolsPath();
    QProcess *process = new QProcess();
    process->startDetached(str_ToolPath + "/" + DELETE_UNUSED_BRANCH,QStringList(),
                           str_ToolPath);
    process->deleteLater();
}

void MainWindow::on_openIntrFolderBtn_clicked()
{
    QUrl url(m_strBasePath.replace("\\","/"));
    QDesktopServices::openUrl(url);
}

/*******************************************************
 ***** CHANGE ORDER OF THE LOC ACC SCREENS ******
 ******************************************************/

void MainWindow::on_screenUpBtn_clicked()
{
    QTreeWidgetItem* item = ui->locTreeWidget->currentItem();
    int  row  = ui->locTreeWidget->currentIndex().row();

    m_locAcc->changeOrder(row,row-1,item);
    ui->locTreeWidget->setCurrentItem(item);
    return;

}

void MainWindow::on_screenDownBtn_clicked()
{
    QTreeWidgetItem* item = ui->locTreeWidget->currentItem();
    int  row  = ui->locTreeWidget->currentIndex().row();

    if(m_locAcc->changeOrder(row,row+1,item)){

    }
    ui->locTreeWidget->setCurrentItem(item);
    return;
}

void MainWindow::on_cb_stopConfigModification_clicked()
{
    m_config->setConfigUpdateFlag(ui->cb_stopConfigModification->isChecked());
    m_config->writeConfigJson();
    changeIcon(ui->cb_stopConfigModification);
    storeSetting();
}

void MainWindow::on_cb_stopAutoCompile_clicked()
{
    m_template->setAutoCompile(ui->cb_stopAutoCompile->isChecked());
    changeIcon(ui->cb_stopAutoCompile);
    storeSetting();
}

void MainWindow::on_saveLocAccBtn_clicked()
{
    m_locAcc->writeFile();
}

void MainWindow::on_replaceAllLocBtn_clicked()
{
    QString str_locAccFilePath = getCommonLocAccFilePath();
    m_locAcc->replaceAll(str_locAccFilePath);
    on_validateLOCBtn_clicked();
}


void MainWindow::on_validateLOCBtn_clicked()
{
    bool returnFlag = m_locAcc->validateLocAccJson();
    if(!returnFlag)
    {
        QMessageBox::critical(this,"Duplicate Entries are found.","Duplicate entries are found for message/element or screen missing.Refer log file.");
        return;
    }
}

void MainWindow::on_refreshBtn_clicked()
{
    scanChanges();
}

/********************************************************
 *  H A N D L E   C O M P O N E N T S
 * ****************************************************/

void MainWindow::componentsChanged()
{
    ui->origCompList->clear();
    ui->origCompList->addItems(m_components->getUnusedComponentList());

    ui->usedCompList->clear();
    ui->usedCompList->addItems(m_components->getUsedComponentList());

    ui->origCompList->setCurrentRow(0);
    ui->usedCompList->setCurrentRow(0);

    m_config->setComponentList(m_components->getUsedComponentList());
    m_config->writeConfigJson();
}

void MainWindow::on_useComponentBtn_clicked()
{
    int currentSelectedCompIndex = ui->origCompList->currentRow();
    if(ui->origCompList->count() > 0 )
    {
        m_components->useComponent(currentSelectedCompIndex);
    }
}

void MainWindow::on_removeComponentBtn_clicked()
{
    int currentSelectedCompIndex = ui->usedCompList->currentRow();
    if(ui->usedCompList->count() > 0)
    {
        m_components->removeComponent(currentSelectedCompIndex);
    }
}

void MainWindow::on_addPlaceHolderBtn_clicked()
{
    if(ui->locMsgText->hasFocus())
    {
        int cursorPosition = ui->locMsgText->cursorPosition();
        QString newText = ui->locMsgText->text();
        newText.insert(cursorPosition,LOC_ACC_PLACE_HOLDER);
        ui->locMsgText->setText(newText);
        if(ui->cb_isAccTextSame->isChecked())
        {
            ui->accMsgText->setText(newText);
        }

        ui->locMsgText->setCursorPosition(cursorPosition + LOC_ACC_PLACE_HOLDER.length());
    }
    else if(ui->accMsgText->hasFocus())
    {
        int cursorPosition = ui->accMsgText->cursorPosition();
        QString newText = ui->accMsgText->text();
        newText.insert(cursorPosition,LOC_ACC_PLACE_HOLDER);
        ui->accMsgText->setText(newText);

        ui->accMsgText->setCursorPosition(cursorPosition + LOC_ACC_PLACE_HOLDER.length());
    }
}

void MainWindow::on_updtPlaceHolderBtn_clicked()
{
    if(ui->updtLocMsgText->hasFocus())
    {
        int cursorPosition = ui->updtLocMsgText->cursorPosition();
        QString newText = ui->updtLocMsgText->text();
        newText.insert(cursorPosition,LOC_ACC_PLACE_HOLDER);
        ui->updtLocMsgText->setText(newText);
        if(ui->cb_isAccTextSameUpdt->isChecked())
        {
            ui->updtAccMsgText->setText(newText);
        }
        ui->updtLocMsgText->setCursorPosition(cursorPosition + LOC_ACC_PLACE_HOLDER.length());
     }
    else if(ui->updtAccMsgText->hasFocus())
    {
        int cursorPosition = ui->updtAccMsgText->cursorPosition();
        QString newText = ui->updtAccMsgText->text();
        newText.insert(cursorPosition,LOC_ACC_PLACE_HOLDER);
        ui->updtAccMsgText->setText(newText);

        ui->updtAccMsgText->setCursorPosition(cursorPosition + LOC_ACC_PLACE_HOLDER.length());
    }
}


void MainWindow::on_helpActionTriggered()
{
    QUrl url("de-oneclick-away.chm");
    QDesktopServices::openUrl(url);
}


void MainWindow::on_configManuallyModified(QString path)
{
    showApp();
    if(QMessageBox::information(this
                             ,"Do you want to reload ??"
                             ,"Config has been modified manually. Do you want to reload ?"
                             ,QMessageBox::Ok,QMessageBox::Cancel) == QMessageBox::Ok)
    {
        m_config->readConfigJson();
        m_locAcc->readFile();
    }
}

void MainWindow::on_cb_launchOnStartup_clicked()
{
       QSettings settings("HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run",QSettings::NativeFormat);
       bool startupEnabled = ui->cb_launchOnStartup->isChecked();
       if (startupEnabled) {
           settings.setValue(APP_NAME, QCoreApplication::applicationFilePath().replace("/","\\"));
           qDebug() << settings.value("Skype");
       } else {
           settings.remove(APP_NAME);
       }
       storeSetting();
}

void MainWindow::createTemplateBatchFiles()
{
    QString moduleName = m_config->getModuleName();
    m_form->createTemplateBatchFiles(m_strBasePath + "/templates", moduleName);
}

void MainWindow::changeIcon(QCheckBox *widget)
{
    bool checkedStatus = widget->isChecked();
    QString iconSource = QString(":/images/") + ( !checkedStatus ? "unlock-48.png" : "lock-48.png");
    widget->setIcon(QIcon(iconSource));
}

void MainWindow::on_le_folderSearch_textEdited(const QString &arg1)
{
    QModelIndex index = m_qfsModel->index(m_strRootPath);
    int rowCount = m_qfsModel->rowCount(index);
    QString folderName;
    m_listMatchedFolders.clear();
    for(int i = 0 ;i < rowCount; i++)
    {
        QModelIndex mi = m_qfsModel->index(i,0,index);
        folderName = m_qfsModel->itemData(mi)[Qt::DisplayRole].toString();
        if(folderName.contains(arg1)){
            m_listMatchedFolders.append(mi);
        }
    }
    QString style = QString("background-color : ") +
            ( m_listMatchedFolders.length() != 0 ? "white" : "pink") + QString(";opacity : 0.5;");
    ui->le_folderSearch->setStyleSheet(style);
    m_iMatchedId = 0;
    on_btn_searchUp_clicked();
}

void MainWindow::on_btn_searchUp_clicked()
{
    if(m_listMatchedFolders.length() == 0){
        return;
    }
    m_iMatchedId = m_iMatchedId <= 0 ? 0 : (m_iMatchedId - 1);
    ui->treeView->setCurrentIndex(m_listMatchedFolders.at(m_iMatchedId));
}

void MainWindow::on_btn_searchDown_clicked()
{
    if(m_listMatchedFolders.length() == 0){
        return;
    }
    m_iMatchedId = (m_iMatchedId > m_listMatchedFolders.length() - 2) ? (m_listMatchedFolders.length() - 1)
                                                                      : (m_iMatchedId + 1);
    ui->treeView->setCurrentIndex(m_listMatchedFolders.at(m_iMatchedId));
}

void MainWindow::on_locSearchText_textEdited(const QString &arg1)
{
    on_searchLocBtn_clicked();
}

void MainWindow::on_exportHelpBtn_clicked()
{
    QString fnText = m_locAcc->exportHelpFunction();
    m_exportHelpDialog->setText(fnText);
    m_exportHelpDialog->show();
}

void MainWindow::instanceCheck()
{
    QLocalSocket socket;
    socket.connectToServer("serverName");
    if (socket.waitForConnected(500)){
        QMessageBox::critical(this,"WatchDog2 is already running.",
                             "You are trying to open another instance of the WatchDog2.\n"
                              "Newely created instance will be terminated.\n"
                              "Note : Check your system notification area."
                                ,QMessageBox::Ok) ;
        close();
        qApp->quit();
        return;
    }

    QLocalServer *m_localServer = new QLocalServer(this);
    m_localServer->listen("serverName");
}

void MainWindow::checkForUpdates()
{
    QProcess *updater = new QProcess();
    qDebug() << updater->startDetached("WatchDog2Updater.exe");
}

void MainWindow::on_btn_checkUpdates_clicked()
{
    checkForUpdates();
}

void MainWindow::on_yuiDoc_btn_clicked()
{
    QProcess *lazyYUI = new QProcess();
    qDebug() << lazyYUI->startDetached("LazyYUIDesk.exe");
}
