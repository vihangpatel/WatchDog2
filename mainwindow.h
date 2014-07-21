#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QFileSystemWatcher>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QFileSystemModel>
#include <QtWidgets/QTreeView>
#include <QtGui>
#include <QListWidgetItem>
#include "js.h"
#include "templates.h"
#include "newinteractivityform.h"
#include "configHandler.h"
#include "css.h"
#include "QSystemTrayIcon"
#include "locacc.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QFileDialog *fd_select;
    QString str_basePath;
    QString str_rootPath;
    QFileSystemModel *qfs_model;
    QFileSystemWatcher *qfsw;
    JS *js;
    CSS *css;
    Templates *tmplt;
    NewInterActivityForm *form;
    ConfigHandler *config;
    QSystemTrayIcon *trayIcon;
    LOCACC *locAcc;

private :
    void initialize();

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void resetAll();
    void scanChanges();
    void registerWatcher();
    void deregisterWatcher();
    void loadSavedSettings();
    void updateDirTree();
    void connectSignals();

    QJsonArray syncTmpltList(QJsonArray jTemplateArray);
    QJsonArray syncJSList(QJsonArray jJSArray);
    QJsonArray syncCSSList(QJsonArray cssArray);

private slots:
    void on_openDialog_clicked();
    void on_templateNextLoadCheckBox_clicked();
    void on_jsViewNextLoadCheckBox_clicked();
    void on_createNewInter_clicked();

    void on_cssNextLoadCheckBox_clicked();

    void on_addScreenBtn_clicked();

    void on_addEleBtn_clicked();

public slots:
    void updateTemplateList(QFileInfoList);
    void templateFileListClicked(QListWidgetItem *);

    void updateJSList(QFileInfoList);
    void jsFileListClicked(QListWidgetItem *);

    void updateCssList(QFileInfoList);
    void cssFileListClicked(QListWidgetItem *);

    void changeBasePath(QString);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
