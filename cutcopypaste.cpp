#include "cutcopypaste.h"

cutCopyPaste::cutCopyPaste()
{
}


bool cutCopyPaste::copyMsg()
{

}

bool cutCopyPaste::cutMsg()
{

}

bool cutCopyPaste::pasteMsg()
{

}

bool cutCopyPaste::copyElement()
{

}

bool cutCopyPaste::cutElement()
{

}

bool cutCopyPaste::pasteElement()
{

}

void cutCopyPaste::setCurrentItem(QListWidgetItem *currentItm)
{
        currentItem = currentItm;
}

void cutCopyPaste::setCopyOperation(bool allow)
{
    _isCopyOperation = allow;
    _isCutOperation = false;
    _isPasterOperation = false;
}

void cutCopyPaste::setPasteOperation(bool allow)
{
    _isCopyOperation = true;
    _isCutOperation = false;
    _isPasterOperation = allow;
}

void cutCopyPaste::setCutOperation(bool allow)
{
    _isCopyOperation = true;
    _isCutOperation = allow;
    _isPasterOperation = false;
}
