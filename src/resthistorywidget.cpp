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
    setRootIsDecorated(true);
    setUniformRowHeights(true);
    setAllColumnsShowFocus(true);
    header()->setCascadingSectionResizes(true);
    header()->setHighlightSections(false);
    header()->setStretchLastSection(true);

    setSelectionMode(QAbstractItemView::ExtendedSelection);
    menu = new QMenu;
}

void RestHistoryWidget::addContextMenuItem(QAction *action, const QString& name)
{
    menu->addAction(action);
    m_actions.insert(name, action);
}

void RestHistoryWidget::addContextMenuSeparator()
{
    menu->addSeparator();
}

void RestHistoryWidget::mousePressEvent(QMouseEvent *event)
{
    if( topLevelItemCount() != 0 ) {
      if(event->button() == Qt::RightButton) {
          QTreeWidgetItem *item = currentItem();
          if (item) {
              //Hide items for group
              if (item->type() == RestHistoryWidget::TYPE_GROUP) {
                  m_actions.value("info")->setVisible(false);
                  m_actions.value("remove")->setVisible(false);
                  m_actions.value("clear")->setVisible(false);
                  m_actions.value("group")->setVisible(false);
                  m_actions.value("ungroup")->setVisible(false);
              } else {
                  m_actions.value("info")->setVisible(true);
                  m_actions.value("remove")->setVisible(true);
                  m_actions.value("clear")->setVisible(true);
                  m_actions.value("group")->setVisible(true);
                  m_actions.value("ungroup")->setVisible(true);

                  //Disable/Enable for groupped and not grouped items
                  if (item->parent()) {
                      m_actions.value("group")->setVisible(false);
                      m_actions.value("ungroup")->setVisible(true);
                  } else {
                      m_actions.value("group")->setVisible(true);
                      m_actions.value("ungroup")->setVisible(false);
                  }
              }
              menu->exec(event->globalPos());
          }
      }
    }

    QTreeWidget::mousePressEvent(event);
}
