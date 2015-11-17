/***************************************************************************
 *   Copyright (C) 2014 by peter komar                                     *
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

#include "resthistorywidget.h"

#include <QMenu>
#include <QAction>
#include <QHeaderView>
#include <QMouseEvent>

RestHistoryWidget::RestHistoryWidget(QWidget *parent) :
    QTreeWidget(parent)
{
    QStringList params;
    params << "ID" << "Type" << "URL" << "Response Code" << "Date";
    setHeaderLabels(params);

    setColumnWidth(0,90);
    setColumnWidth(2,500);

    setAlternatingRowColors(true);
    setRootIsDecorated(false);
    setUniformRowHeights(true);
    setAllColumnsShowFocus(true);
    header()->setCascadingSectionResizes(true);
    header()->setHighlightSections(false);
    header()->setStretchLastSection(true);

    setSelectionMode(QAbstractItemView::ExtendedSelection);
    menu = new QMenu;
}

void RestHistoryWidget::addContextMenuItem(QAction *action)
{
    menu->addAction(action);
}

void RestHistoryWidget::addContextMenuSeparator()
{
    menu->addSeparator();
}

void RestHistoryWidget::mousePressEvent(QMouseEvent *event)
{
    if( topLevelItemCount() != 0 ) {
      if(event->button() == Qt::RightButton) {
        menu->exec(event->globalPos());
      }
    }

    QTreeWidget::mousePressEvent(event);
}
