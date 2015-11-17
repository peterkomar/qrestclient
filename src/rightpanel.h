#ifndef RIGHTPANEL_H
#define RIGHTPANEL_H

class RestClientMainWindow;
class QTextEdit;

class RightPanel
{
public:
    RightPanel(RestClientMainWindow* app);
    QTextEdit *m_responseHeaders;
};

#endif // RIGHTPANEL_H
