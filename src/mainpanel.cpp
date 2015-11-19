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
#include "mainpanel.h"
#include "restclientmainwindow.h"
#include "responsewidget.h"

#include <QPlainTextEdit>
#include <QVBoxLayout>

MainPanel::MainPanel(RestClientMainWindow* app)
{
    m_response = new ResponseWidget(app);
    m_response->setMinimumSize(500, 205);

    m_errorResponse = new QPlainTextEdit;
    m_errorResponse->setPlainText(QObject::tr("Error:"));
    m_errorResponse->setReadOnly(true);
    m_errorResponse->setMinimumSize(500, 40);
    m_errorResponse->setMaximumHeight(40);

    QWidget *main = new QWidget;
    QVBoxLayout *pvLayout = new QVBoxLayout(main);
    pvLayout->addWidget(m_response);
    pvLayout->addWidget(m_errorResponse);
    app->setCentralWidget(main);
}

