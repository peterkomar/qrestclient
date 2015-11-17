#ifndef LEFTPANEL_H
#define LEFTPANEL_H

#include "paramslist.h"

class RestClientMainWindow;
class QComboBox;
class QTextEdit;
class QTabWidget;

class LeftPanel
{
public:
    LeftPanel(RestClientMainWindow* app);

    ParamsList *m_params;
    ParamsList *m_headers;
    QComboBox  *m_requestContentType;
    QTextEdit  *m_rawContent;
    QTabWidget *m_tab;

private:
    QWidget* buildParamsWidget(ParamsList *);
};

#endif // LEFTPANEL_H
