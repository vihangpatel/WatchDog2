#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QFileSystemWatcher>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QFileSystemModel>
#include <QtWidgets/QTreeView>
#include <QtGui>
#include <QSignalMapper>
#include <QListWidgetItem>
#include "js.h"
#include "templates.h"
#include "newinteractivityform.h"
#include "configHandler.h"
#include "css.h"
#include "QSystemTrayIcon"
#include "locacc.h"
#include "mediaimages.h"
#include "appconfig.h"
#include <QBrush>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QFileDialog *m_fileDialogSelect;
    QString m_strBasePath;
    QString m_strRootPath;
    QString m_strReplacementFilePath;
    QFileSystemModel *m_qfsModel;
    QFileSystemWatcher *m_qfswMain;
    JS *m_js;
    CSS *m_css;
    Templates *m_template;
    NewInterActivityForm *m_form;
    ConfigHandler *m_config;
    QSystemTrayIcon *m_trayIcon;
    LOCACC *m_locAcc;
    MediaImages *m_images;
    QMenu *m_qmTrayMenu;
    QMenu *m_qmTreeMenu;
    AppConfig *m_appConfig;
    enum OperationType{ O_CUT , O_COPY  ,O_PASTE , O_DELETE} m_eOperation;
    QTreeWidgetItem *m_qtwiSource;
    QTreeWidgetItem *m_qtwiDestination;

private :
    void initialize();    
    void initTrayIcon();

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void resetAll();
    void registerWatcher();
    void deregisterWatcher();
    void loadSavedSettings();
    void storeSetting();
    void updateDirTree();
    void connectSignals();
    int getTreeItemIndentationLevel(QTreeWidgetItem *,int count=0);
    void manageLocAccItemsVisibility(int indentationLevel = 0);
    void setCheckBoxStatus(bool checked);
    void refreshTabStatus();
    QString getCommonFolderPath();
    QString getCommonLocAccFilePath();
    QString getMinificationFolderPath();
    QString getToolsPath();
    QString getCurrentInteractivityName();    

    QJsonArray syncTmpltList(QJsonArray jTemplateArray);
    QJsonArray syncJSList(QJsonArray jJSArray);
    QJsonArray syncCSSList(QJsonArray cssArray);
    QJsonArray syncImgsList(QJsonArray imgesArray);

    void updateLocDetails(int indentationLevel = -1);
    void fillScreenDetail(QStringList data);
    void fillElementDetail(QStringList data);
    void fillMessageDetail(QStringList data);
    void createLOCTreeContext();
    void contextMenuVisibility();

private slots:
    void on_openDialog_clicked();
    void on_templateNextLoadCheckBox_clicked();
    void on_jsViewNextLoadCheckBox_clicked();
    void on_createNewInter_clicked();

    void scanChanges();
    void on_cssNextLoadCheckBox_clicked();

    void on_addScreenBtn_clicked();

    void on_addEleBtn_clicked();

    void on_locTreeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_locTreeWidget_itemSelectionChanged();

    void on_cb_isAccTextSame_clicked();

    void on_locMsgText_textChanged(const QString &arg1);

    void on_jsViewList_itemSelectionChanged();

    void on_templateList_itemSelectionChanged();

    void on_cssList_itemSelectionChanged();

    void on_addMsgBtn_clicked();

    void on_updtScreenBtn_clicked();

    void on_updateMsgBtn_clicked();

    void on_updtEleBtn_clicked();

    void deleteScreen();

    void deleteMessage();

    void deleteElement();

    void on_searchLocBtn_clicked();

    void on_goPrevSearchBtn_clicked();

    void on_goNextSearchBtn_clicked();

    void on_cb_isAccTextSameUpdt_clicked();

    void on_updtLocMsgText_textEdited(const QString &arg1);

    void openLocAccTab();

    void on_comboBox_Languages_currentIndexChanged(int index);

    void on_addNewLangBtn_clicked();

    void on_browsePathBtn_clicked();

    void on_jsOneUpBtn_clicked();

    void on_jsOneDownBtn_clicked();

    void showLocTreeCustomMenu(QTreeWidgetItem* item, const QPoint& globalPos);

    void onCustomContextMenuRequested(const QPoint& pos);

    void on_cb_stopJSMonitor_clicked();

    void on_cb_stopTmpltMonitir_clicked();

    void on_cb_stopCSSMonitor_clicked();

    void on_cb_stopMediaMonitor_clicked();

    void on_miniFyCommonBtn_clicked();

    void on_minifyInterActBtn_clicked();

    void on_minifyPreLoadBtn_clicked();

    void on_deleteOrigFilesBtn_clicked();

    void on_deleteBranchesBtn_clicked();

    void on_openIntrFolderBtn_clicked();

    void on_screenUpBtn_clicked();

    void on_screenDownBtn_clicked();

    void on_cb_stopConfigModification_clicked();

    void on_saveLocAccBtn_clicked();

    void performLocOpertions(int operation);

    void on_replaceAllLocBtn_clicked();

    void on_validateLOCBtn_clicked();

    void on_cb_mediaIsNextStopLoad_clicked();

    void on_mediaList_itemSelectionChanged();

    void on_cb_mediaForceLoad_clicked();

    void on_updateImgBtn_clicked();

public slots:
    void updateTemplateList(QFileInfoList);
    void templateFileListClicked(QListWidgetItem *);

    void updateJSList(QFileInfoList);
    void jsFileListClicked(QListWidgetItem *);

    void updateCssList(QFileInfoList);
    void cssFileListClicked(QListWidgetItem *);

    void updateMediaList(QFileInfoList);
    void mediaFileListClicked(QListWidgetItem *);

    void changeBasePath(QString);
    void showApp();
    void hideApp();
    void stopMonitoring();
    void startMonitoring();
    void compileAllHandleBars();
    void newInterActivityCreated(QString path);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
