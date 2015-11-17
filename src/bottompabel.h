#ifndef BOTTOMPABEL_H
#define BOTTOMPABEL_H

#include "paramslist.h"

class QLineEdit;
class RestHistoryWidget;
class RestClientMainWindow;

class BottomPabel
{
public:
    BottomPabel(RestClientMainWindow* app);

    QLineEdit *m_filterEdit;
    RestHistoryWidget *m_historyWidget;
};

#endif // BOTTOMPABEL_H
