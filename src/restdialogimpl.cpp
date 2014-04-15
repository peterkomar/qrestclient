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
#include "restdialogimpl.h"
#include "editdlgimpl.h"
#include "requesthistory.h"

#include <QStringList>
#include <QMessageBox>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QUrlQuery>

RestDialogImpl::RestDialogImpl(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    QStringList params;
    params << "Name" << "Value";
    m_paramsList->setHeaderLabels(params);
    m_headerList->setHeaderLabels(params);

    QStringList historyHeader;
    historyHeader << "ID" << "Type" << "URL" << "Response Code" << "Date";
    m_historyTree->setHeaderLabels(historyHeader);

    initConnections();

    m_history = new RequestHistory();
    m_history->init();

    loadHistory();

    m_headerList->setColumnWidth(0,200);
    m_paramsList->setColumnWidth(0,200);
    m_historyTree->setColumnWidth(0,90);
    m_historyTree->setColumnWidth(2,500);

    m_reply = 0;
    m_waitDialog = 0;

    QStringList types;
    types << "application/json" << "text/csv" << "text/plain" << "text/xml";
    m_typesMime->addItems(types);

}

RestDialogImpl::~RestDialogImpl()
{
    delete m_history;
}

void RestDialogImpl::initConnections()
{
    btnAddHeader->setProperty("add", true);
    connect(btnAddHeader, SIGNAL(clicked()), this, SLOT(slotEditHeader()));
    connect(btnEditHeader, SIGNAL(clicked()), this, SLOT(slotEditHeader()));
    connect(btnDeleteHeader, SIGNAL(clicked()), this, SLOT(slotDeleteHeader()));

    btnAddParam->setProperty("add", true);
    connect(btnAddParam, SIGNAL(clicked()), this, SLOT(slotEditParam()));
    connect(btnEditParam, SIGNAL(clicked()), this, SLOT(slotEditParam()));
    connect(btnRemParam, SIGNAL(clicked()), this, SLOT(slotDeleteParam()));

    connect(btnSend, SIGNAL(clicked()), this, SLOT(slotSendRequest()));

    connect(m_historyTree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(slotHistoryLoad(QTreeWidgetItem*,int)));
    connect(m_historyTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(slotRemoveHistoryItem(QTreeWidgetItem*)));
}

void RestDialogImpl::slotEditHeader()
{
    QPushButton *btn = static_cast<QPushButton *>(sender());
    QString name, value;
    QTreeWidgetItem *item = NULL;
    if( btn->property("add").isNull() ) {
        item = m_headerList->currentItem();
        if( item != NULL ) {
            name = item->text(0);
            value = item->text(1);
        }
    }

    EditDlgImpl *dlg = new EditDlgImpl(this);
    dlg->setName(name);
    dlg->setValue(value);

    dlg->exec();

    name = dlg->getName();
    value = dlg->getValue();
    int res = dlg->result();

    delete dlg;

    if( res == QDialog::Rejected ) {
        return;
    }

    if( item == NULL ) {
        item = new QTreeWidgetItem(m_headerList);
    }

    item->setText(0, name);
    item->setText(1, value);
}

void RestDialogImpl::slotDeleteHeader()
{
    QTreeWidgetItem *item = m_headerList->currentItem();
    if(item != NULL) {
        delete item;
        item = NULL;
    }
}

void RestDialogImpl::slotEditParam()
{
    QPushButton *btn = static_cast<QPushButton *>(sender());
    QString name, value;
    QTreeWidgetItem *item = NULL;
    if( btn->property("add").isNull() ) {
        item = m_paramsList->currentItem();
        if( item != NULL ) {
            name = item->text(0);
            value = item->text(1);
        }
    }

    EditDlgImpl *dlg = new EditDlgImpl(this);
    dlg->setName(name);
    dlg->setValue(value);

    dlg->exec();

    name = dlg->getName();
    value = dlg->getValue();
    int res = dlg->result();

    delete dlg;

    if( res == QDialog::Rejected ) {
        return;
    }

    if( item == NULL ) {
        item = new QTreeWidgetItem(m_paramsList);
    }

    item->setText(0, name);
    item->setText(1, value);
}

void RestDialogImpl::slotDeleteParam()
{
    QTreeWidgetItem *item = m_paramsList->currentItem();
    if(item != NULL) {
        delete item;
        item = NULL;
    }
}

void RestDialogImpl::slotSendRequest()
{
    QString url = m_url->text();
    if( url.isEmpty() || url.left(4) != "http" ) {
        QMessageBox::critical(this, "Error", "URL is incorrect");
        return;
    }

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    QUrl urlObject(url);
    QUrlQuery query;
    QTreeWidgetItemIterator paramsIterator(m_paramsList);
    while (*paramsIterator) {
        query.addQueryItem((*paramsIterator)->text(0), (*paramsIterator)->text(1));
        ++paramsIterator;
    }
    urlObject.setQuery(query);

    QNetworkRequest request(urlObject);
    QTreeWidgetItemIterator headerIterator(m_headerList);
    while (*headerIterator) {
        request.setRawHeader((*headerIterator)->text(0).toUtf8(), (*headerIterator)->text(1).toUtf8());
        ++headerIterator;
    }

    QByteArray rawBody  = m_contentBody->toPlainText().toUtf8();
    QString contentType = m_typesMime->currentText();

    if( m_cMethod->currentText() == "GET" ) {
        m_reply = manager->get(request);
    } else if( m_cMethod->currentText() == "POST" ) {

        if( rawBody.isEmpty() ) {
            request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
            m_reply = manager->post(request, query.query(QUrl::FullyEncoded).toUtf8());
        } else {
            QByteArray postDataSize = QByteArray::number(rawBody.size());
            request.setRawHeader("Content-Type", contentType.toLatin1());
            request.setRawHeader("Content-Length", postDataSize);
            m_reply = manager->post(request, rawBody);
        }

    } else if( m_cMethod->currentText() == "PUT" ) {
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        m_reply = manager->put(request, query.query(QUrl::FullyEncoded).toUtf8());
    } else if( m_cMethod->currentText() == "DELETE" ) {
        m_reply = manager->deleteResource(request);
    } else {
        QMessageBox::critical(this, "Error", "Unsupported method");
        return;
    }

    connect(m_reply, SIGNAL(readyRead()), this, SLOT(slotReplyResponse()));
    connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), this,SLOT(slotReplyError(QNetworkReply::NetworkError)));
    connect(m_reply, SIGNAL(finished()), this, SLOT(slotReplyFinish()));

    m_response->clear();
    waitDialog();
}

void RestDialogImpl::saveHistory(int code)
{
    QString url = m_url->text();
    if( url.isEmpty() || url.left(4) != "http" ) {
        QMessageBox::critical(this, "Error", "URL is incorrect");
        return;
    }

    int requestId = m_history->addRequest(url, m_cMethod->currentText(), code, m_response->toPlainText());
    QTreeWidgetItemIterator it(m_paramsList);
    while (*it) {
        m_history->addParam(requestId, (*it)->text(0), (*it)->text(1));
        ++it;
    }

    QTreeWidgetItemIterator it2(m_headerList);
    while (*it2) {
        m_history->addHeader(requestId, (*it2)->text(0), (*it2)->text(1));
        ++it2;
    }

    m_history->addRaw(requestId, m_typesMime->currentText(), m_contentBody->toPlainText());
    loadHistory();
}

void RestDialogImpl::loadHistory()
{
    QSqlQuery q(m_history->database());
    q.prepare("SELECT * FROM requests ORDER By id DESC");
    if( !q.exec() ) {
        throw "Error execute query";
    }

    m_historyTree->clear();
    while (q.next()) {
        QTreeWidgetItem *item  = new QTreeWidgetItem(m_historyTree);
        item->setText(0, q.value(0).toString());
        item->setText(1, q.value(3).toString());
        item->setText(2, q.value(4).toString());
        item->setText(3, q.value(2).toString());
        item->setText(4, q.value(1).toString());
    }
}

void RestDialogImpl::slotHistoryLoad(QTreeWidgetItem *item, int )
{
    QSqlQuery q(m_history->database());
    q.prepare("SELECT * FROM requests WHERE id = :id");
    q.bindValue(":id", item->text(0));
    if( !q.exec() ) {
        throw "Error execute query";
    }

    if( !q.first() ) {
        return;
    }

    m_url->setText(q.value(4).toString());
    m_cMethod->setCurrentText(q.value(3).toString());
    m_response->setText(q.value(5).toString());

    QTreeWidgetItem *i = 0;
    //load params
    q.prepare("SELECT * FROM requests_params WHERE request_id = :id");
    q.bindValue(":id", item->text(0));
    if( !q.exec() ) {
        throw "Error execute query";
    }
    m_paramsList->clear();
    while (q.next()) {
        i  = new QTreeWidgetItem(m_paramsList);
        i->setText(0, q.value(1).toString());
        i->setText(1, q.value(2).toString());
    }

    //load headers
    q.prepare("SELECT * FROM request_headers WHERE request_id = :id");
    q.bindValue(":id", item->text(0));
    if( !q.exec() ) {
        throw "Error execute query";
    }
    m_headerList->clear();
    while (q.next()) {
        i  = new QTreeWidgetItem(m_headerList);
        i->setText(0, q.value(1).toString());
        i->setText(1, q.value(2).toString());
    }

    q.prepare("SELECT * FROM request_raw WHERE request_id = :id");
    q.bindValue(":id", item->text(0));
    if( !q.exec() ) {
        throw "Error execute query";
    }

    m_contentBody->clear();
    if( !q.first() ) {
        return;
    }

    QString s = q.value(2).toString();
    if(m_typesMime->findText(s) != -1) {
        m_typesMime->setCurrentText(s);
    } else {
        //m_typesMime->addItem(s);
    }

    m_contentBody->setText(q.value(1).toString());
}

void RestDialogImpl::slotRemoveHistoryItem(QTreeWidgetItem *item)
{
    int res = QMessageBox::question(this, "Confirm Remove", "Are you shure to want to remove request?");
    if( res == QMessageBox::Yes ) {
        m_history->deleteHistory(item->text(0).toInt());
        delete item;
    }
}

void RestDialogImpl::waitDialog()
{
    m_waitDialog = new QDialog(this);
    m_waitDialog->setModal(true);
    Qt::WindowFlags flags = Qt::ToolTip;
    m_waitDialog->setWindowFlags(flags);

    QVBoxLayout *l = new QVBoxLayout;
    l->addWidget(new QLabel("Please wait, sending request ..."));

    m_waitDialog->setLayout(l);
    m_waitDialog->exec();
}

void RestDialogImpl::releaseReplyResources()
{
    disconnect(m_reply, SIGNAL(readyRead()), this, SLOT(slotReplyResponse()));
    disconnect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), this,SLOT(slotReplyError(QNetworkReply::NetworkError)));
    disconnect(m_reply, SIGNAL(finished()), this, SLOT(slotReplyFinish()));

    QNetworkAccessManager *m = m_reply->manager();
    m_reply->deleteLater();
    m->deleteLater();
    m_waitDialog->accept();

    delete m_waitDialog;
}

void RestDialogImpl::slotReplyResponse()
{
    QApplication::processEvents();
    m_response->append(m_reply->readAll());
}

void RestDialogImpl::slotReplyError(QNetworkReply::NetworkError error)
{
    QString error_string = "<br/><hr/>Error:<br/>";
    switch( error) {
      case QNetworkReply::ConnectionRefusedError :
          error_string += tr("the remote server refused the connection (the server is not accepting requests)");
          break;

      case QNetworkReply::HostNotFoundError :
          error_string += tr("the remote host name was not found (invalid hostname)");
          break;

      default :
          error_string +=  m_reply->errorString();
          break;
    }

    m_response->append(error_string);
    releaseReplyResources();

    saveHistory(error);
}

void RestDialogImpl::slotReplyFinish()
{
    QList<QByteArray> headers = m_reply->rawHeaderList();
    m_reply->close();
    releaseReplyResources();
    saveHistory(200);

    m_headersText->clear();
    for (int i = 0; i < headers.size(); ++i) {
        m_headersText->append("<b>"+headers.at(i) + "</b>: " + m_reply->rawHeader(headers.at(i)));
    }
}
