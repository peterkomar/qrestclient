/***************************************************************************
 *   Copyright (C) 2015 by peter komar                                     *
 *   udldevel@gmail.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
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

    QAction *groupAction = new QAction(QObject::tr("Group"), app);
    groupAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));
    QObject::connect(groupAction, SIGNAL(triggered()), app, SLOT(slotGroup()));
    groupAction->setVisible(false);

    QAction *unGroupAction = new QAction(QObject::tr("Ungroup"), app);
    unGroupAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_G));
    QObject::connect(unGroupAction, SIGNAL(triggered()), app, SLOT(slotUnGroup()));
    unGroupAction->setVisible(false);

    m_historyWidget->addContextMenuItem(infoAction, "info");
    m_historyWidget->addContextMenuSeparator();
    m_historyWidget->addContextMenuItem(removeAction, "remove");
    m_historyWidget->addContextMenuItem(clearAction, "clear");
    m_historyWidget->addContextMenuSeparator();
    m_historyWidget->addContextMenuItem(filterAction, "filter");
    m_historyWidget->addContextMenuItem(groupAction, "group");
    m_historyWidget->addContextMenuItem(unGroupAction, "ungroup");

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

