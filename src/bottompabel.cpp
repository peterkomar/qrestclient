#include "bottompabel.h"
#include "resthistorywidget.h"
#include "restclientmainwindow.h"

#include <QLineEdit>
#include <QAction>
#include <QDockWidget>
#include <QGridLayout>
#include <QMenuBar>

BottomPabel::BottomPabel(RestClientMainWindow* app)
{
    m_filterEdit = new QLineEdit;
    m_filterEdit->setMaximumWidth(300);
    m_filterEdit->setVisible(false);
    QObject::connect(m_filterEdit, SIGNAL(textChanged(QString)), app, SLOT(slotFilterHistoryItems(QString)));

    m_historyWidget = new RestHistoryWidget;
    QObject::connect(m_historyWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)), app, SLOT(slotHistoryLoad(QTreeWidgetItem*)));
    QObject::connect(m_historyWidget, SIGNAL(itemSelectionChanged()), app, SLOT(slotSelectedHistory()));
    QObject::connect(m_historyWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), app, SLOT(slotRequestDetails()));

    QAction *infoAction = new QAction(QObject::tr("Info"), app);
    infoAction->setShortcut(QKeySequence::Open);
    QObject::connect(infoAction, SIGNAL(triggered()), app, SLOT(slotRequestDetails()));

    QAction *removeAction = new QAction(QObject::tr("Remove"), app);
    removeAction->setShortcut(QKeySequence::Delete);
    QObject::connect(removeAction, SIGNAL(triggered()), app, SLOT(slotHistoryRemoveSelected()));

    QAction *clearAction = new QAction(QObject::tr("Clear history"), app);
    QObject::connect(clearAction, SIGNAL(triggered()), app, SLOT(slotHistoryClear()));

    QAction *filterAction = new QAction(QObject::tr("Filter"), app);
    filterAction->setShortcut(QKeySequence::Find);
    QObject::connect(filterAction, SIGNAL(triggered()), app, SLOT(slotShowHistoryFilter()));

    m_historyWidget->addContextMenuItem(infoAction);
    m_historyWidget->addContextMenuSeparator();
    m_historyWidget->addContextMenuItem(removeAction);
    m_historyWidget->addContextMenuItem(clearAction);
    m_historyWidget->addContextMenuSeparator();
    m_historyWidget->addContextMenuItem(filterAction);

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(m_filterEdit, 0, 1);
    gridLayout->addWidget(m_historyWidget, 1, 0, 1, 2);

    QWidget *bottom = new QWidget;
    bottom->setLayout(gridLayout);

    QDockWidget *dock = new QDockWidget;
    dock->setObjectName("Bottom");
    dock->setWindowTitle(QObject::tr("Request History"));
    dock->setWidget(bottom);
    dock->setFeatures(QDockWidget::DockWidgetMovable
                      | QDockWidget::DockWidgetFloatable);
    app->addDockWidget(Qt::BottomDockWidgetArea, dock);

    QMenu *history = app->menuBar()->addMenu(QObject::tr("History"));

    history->addAction(infoAction);
    history->addSeparator();
    history->addAction(removeAction);
    history->addAction(clearAction);
    history->addSeparator();
    history->addAction(filterAction);
}

