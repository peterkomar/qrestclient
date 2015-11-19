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
#include "menu.h"
#include "restclientmainwindow.h"

#include <QMenuBar>
#include <QActionGroup>
#include <QAction>

Menu::Menu(RestClientMainWindow* app)
{
    QMenu *view = app->menuBar()->addMenu(QObject::tr("View"));

    m_jsonView = new QAction("Json", app);
    m_textView = new QAction("Text", app);
    m_csvView  = new QAction("CSV", app);

    QActionGroup *viewGroup = new QActionGroup(app);
    viewGroup->addAction(m_jsonView);
    viewGroup->addAction(m_textView);
    viewGroup->addAction(m_csvView);

    m_jsonView->setCheckable(true);
    m_textView->setCheckable(true);
    m_csvView->setCheckable(true);

    m_textView->setChecked(true);

    view->addAction(m_jsonView);
    view->addAction(m_textView);
    view->addAction(m_csvView);

    QObject::connect(m_jsonView, SIGNAL(triggered()), app, SLOT(slotViewJson()));
    QObject::connect(m_textView, SIGNAL(triggered()), app, SLOT(slotViewText()));
    QObject::connect(m_csvView,  SIGNAL(triggered()), app, SLOT(slotViewCsv()));

    QAction *a = new QAction(QObject::tr("About"), app);
    QMenu *m = app->menuBar()->addMenu(QObject::tr("Help"));
    m->addAction(a);
    QObject::connect(a, SIGNAL(triggered()), app, SLOT(slotAbout()));
}

