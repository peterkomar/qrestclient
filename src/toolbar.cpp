#include "toolbar.h"
#include "restclientmainwindow.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QShortcut>
#include <QToolBar>

ToolBar::ToolBar(RestClientMainWindow* app)
{
    QWidget *tool = new QWidget;
    QHBoxLayout *l = new QHBoxLayout;

    m_url = new QLineEdit("http://");
    m_method = new QComboBox;

    QStringList items;
    items << "GET" << "POST" << "PUT" << "DELETE";
    m_method->addItems(items);

    QPushButton *sendBtn = new QPushButton(QObject::tr("Send"));
    sendBtn->setToolTip(QObject::tr("To run request Ctrl+R"));
    sendBtn->setDefault(true);
    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_R), app);


    QObject::connect(sendBtn, SIGNAL(clicked()), app, SLOT(slotSendRequest()));
    QObject::connect(m_url, SIGNAL(returnPressed()), app, SLOT(slotSendRequest()));
    QObject::connect(shortcut, SIGNAL(activated()), app, SLOT(slotSendRequest()));


    l->addWidget(new QLabel(QObject::tr("URL:")));
    l->addWidget(m_url);
    l->addWidget(new QLabel(QObject::tr("Method:")));
    l->addWidget(m_method);
    l->addWidget(sendBtn);
    tool->setLayout(l);

    QToolBar *toolBar = new QToolBar("restbar", app);
    toolBar->setObjectName("RestToolBar");
    toolBar->addWidget(tool);

    app->addToolBar(toolBar);
}

