#include "rightpanel.h"
#include "restclientmainwindow.h"

#include <QTextEdit>
#include <QDockWidget>

RightPanel::RightPanel(RestClientMainWindow* app)
{
    m_responseHeaders = new QTextEdit;
    m_responseHeaders->setAcceptRichText(false);
    m_responseHeaders->setReadOnly(true);

    QDockWidget *dock = new QDockWidget(app);
    dock->setObjectName("Right");
    dock->setWindowTitle(QObject::tr("Response Headers"));
    dock->setWidget(m_responseHeaders);
    dock->setFeatures(QDockWidget::DockWidgetMovable
                      | QDockWidget::DockWidgetFloatable);
    app->addDockWidget(Qt::LeftDockWidgetArea, dock);
}

