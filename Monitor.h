#ifndef MONITOR_H
#define MONITOR_H

#include <QObject>
#include <QFileSystemWatcher>

class Monitor : public QObject {
    Q_OBJECT
public:
    QFileSystemWatcher *qfsw_js;

    void scanChanges();
    void registerWatcher();
    void deRegisterWatcher();

public slots:

private slots:

};

#endif // MONITOR_H
