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
#include "requestdetailsdlg.h"

#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QGridLayout>
#include <QClipboard>
#include <QApplication>

RequestDetailsDlg::RequestDetailsDlg(QWidget *parent) : QDialog(parent)
{
    QGridLayout *gridLayout = new QGridLayout(this);

    m_view = new QPlainTextEdit(this);
    m_view->setFont(QFont("Monospace"));
    gridLayout->addWidget(m_view, 0, 0, 1, 1);

    resize(800, 500);
}

void RequestDetailsDlg::setRequest(const QString &text)
{
    m_view->setPlainText(text);
}

void RequestDetailsDlg::slotSendToBuffer()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(m_view->toPlainText());
}

