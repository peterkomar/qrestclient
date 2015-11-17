#ifndef TOOLBAR_H
#define TOOLBAR_H

class RestClientMainWindow;
class QLineEdit;
class QComboBox;

class ToolBar
{
public:
    ToolBar(RestClientMainWindow* app);

    QLineEdit *m_url;
    QComboBox *m_method;
};

#endif // TOOLBAR_H
