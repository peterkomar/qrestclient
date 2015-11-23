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
#include "request.h"
#include "gist.h"

#include <QtWidgets/QTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QClipboard>
#include <QApplication>
#include <QPushButton>
#include <QDesktopServices>
#include <QMessageBox>
#include <QLineEdit>
#include <QRadioButton>
#include <QGroupBox>

RequestDetailsDlg::RequestDetailsDlg(Request* request, QWidget *parent)
    : QDialog(parent)
    ,m_request(request)
    ,m_gist(new Gist(parent))
{
    connect(m_gist, SIGNAL(published(QString)), this, SLOT(slotGistUrl(QString)));
    QGridLayout *gridLayout = new QGridLayout(this);

    QHBoxLayout *phLayout = new QHBoxLayout;

    QPushButton *cliboard = new QPushButton(QIcon(":/clipboard_32.png"), "", this);
    cliboard->setAutoDefault(false);
    cliboard->setToolTip(tr("Copy details of request/response to clipboard"));
    cliboard->setWhatsThis(tr("Send request/response to cliboard"));
    connect(cliboard, SIGNAL(clicked()), this, SLOT(slotSendToBuffer()));

    QPushButton *share = new QPushButton(QIcon(":/cloud302_32.png"), "", this);
    share->setAutoDefault(false);
    share->setToolTip(tr("Share request at gist"));
    share->setWhatsThis(tr("Share request at gist"));
    connect(share, SIGNAL(clicked()), this, SLOT(slotSendToGist()));

    phLayout->addWidget(cliboard);
    phLayout->addSpacing(2);
    phLayout->addWidget(share);

    m_viewRequest = new QTextEdit(this);
    m_viewRequest->setFont(QFont("Monospace"));
    m_viewRequest->setReadOnly(true);
    m_viewRequest->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    m_viewRequest->setHtml(m_request->requestToHtml());

    m_viewResponse = new QTextEdit(this);
    m_viewResponse->setFont(QFont("Monospace"));
    m_viewResponse->setReadOnly(true);
    m_viewResponse->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    m_viewResponse->setHtml(m_request->responseToHtml());

    gridLayout->addItem(phLayout, 0, 0, 1, 2, Qt::AlignLeft);
    gridLayout->addWidget(m_viewRequest, 1, 0, 1, 1);
    gridLayout->addWidget(m_viewResponse, 1, 1, 1, 1);
    gridLayout->setContentsMargins(0,0,0,0);
    gridLayout->setMargin(0);
    resize(900, 500);
}

RequestDetailsDlg::~RequestDetailsDlg()
{
    delete m_gist;
}

void RequestDetailsDlg::slotSendToBuffer()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(m_request->toString().join("/n/n/n"));
}

void RequestDetailsDlg::slotSendToGist()
{
    if (!m_request->getGistId().isEmpty()) {
        QDesktopServices::openUrl(QUrl(m_request->getGistId(), QUrl::TolerantMode));
    } else {
        m_gist->sendRequest(m_request);
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    }
}

void RequestDetailsDlg::slotGistUrl(const QString& url)
{
    QApplication::restoreOverrideCursor();
    if (url == "Error") {
        QMessageBox::critical(this, "Gist Error", "Can't create Gist");
    } else {
        m_request->setGistId(url);
        QDesktopServices::openUrl(url);
    }
}

