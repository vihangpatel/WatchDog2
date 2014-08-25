#ifndef CUTCOPYPASTE_H
#define CUTCOPYPASTE_H

#include <QObject>
#include <QListWidgetItem>

class cutCopyPaste : public QObject
{
    Q_OBJECT

    bool _isCopyOperation;
    bool _isCutOperation;
    bool _isPasterOperation;

    QListWidgetItem *currentItem;

public:
    explicit cutCopyPaste();
    bool cutMsg();
    bool copyMsg();
    bool pasteMsg();

    bool cutElement();
    bool copyElement();
    bool pasteElement();

    void setCurrentItem(QListWidgetItem *currentItem);

    void setCutOperation(bool allow);
    void setPasteOperation(bool allow);
    void setCopyOperation(bool allow);

signals:
    
public slots:
    
};

#endif // CUTCOPYPASTE_H
