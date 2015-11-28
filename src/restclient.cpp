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
#include "restclient.h"
#include "request.h"

#include <QNetworkAccessManager>
#include <QTime>
#include <QUrlQuery>
#include <QUrl>
#include <QNetworkRequest>

RestClient::RestClient(QObject *parent)
    : QObject(parent)
    ,m_reply(0)
{
}

RestClient::~RestClient()
{
    m_request = 0;
    m_reply = 0;
    m_time = 0;
}

void RestClient::abort()
{
    if (m_reply) {
        m_reply->abort();
    }
}

void RestClient::sendRequest(Request *request)
{
    m_request = request;

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    QUrl urlObject(m_request->url());
    QUrlQuery query;

    QHashIterator<QString, QString>i(m_request->requestParams());
    while (i.hasNext()) {
        i.next();
        query.addQueryItem(i.key(), i.value());
    }
    urlObject.setQuery(query);
    QNetworkRequest networkRequest(urlObject);

    //Ignore SSL errors
    QSslConfiguration conf = networkRequest.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    networkRequest.setSslConfiguration(conf);

    i = m_request->requestHeaders();
    while (i.hasNext()) {
        i.next();
        networkRequest.setRawHeader(i.key().toUtf8(), i.value().toUtf8());
    }

    if( m_request->method() == "GET" ) {
        m_reply = manager->get(networkRequest);
    } else if( m_request->method() == "POST" || m_request->method() == "PUT" ) {
        sendRawRequest(manager, networkRequest, query);
    } else if( m_request->method() == "DELETE" ) {
        m_reply = manager->deleteResource(networkRequest);
    } else {
        throw "Method not supported";
    }

    QTime c = QTime::currentTime();
    m_time  = new QTime(c.hour(), c.minute(), c.second(), c.msec());
    connect(m_reply, SIGNAL(readyRead()), this, SLOT(slotReplyResponse()));
    connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), this,SLOT(slotReplyError(QNetworkReply::NetworkError)));
    connect(m_reply, SIGNAL(finished()), this, SLOT(slotFinishRequest()));
}

void RestClient::sendRawRequest(QNetworkAccessManager *manager,
                                          QNetworkRequest& request,
                                          const QUrlQuery& query)
{
    QByteArray raw = m_request->raw().toUtf8();
    if( m_request->raw().isEmpty() ) {
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        m_request->addRequestHeader("Content-Type", "application/x-www-form-urlencoded");
        raw = query.query(QUrl::FullyEncoded).toUtf8();
    } else {
        QByteArray postDataSize = QByteArray::number(raw.size());
        request.setRawHeader("Content-Length", postDataSize);
        m_request->addRequestHeader("Content-Length", postDataSize);
    }

    if (m_request->method() == "POST") {
        m_reply = manager->post(request, raw);
    } else {
        m_reply = manager->put(request, raw);
    }
}

void RestClient::parseResponseHeaders()
{
    QTime time = QTime::currentTime();
    QList<QByteArray> headers = m_reply->rawHeaderList();
    for (int i = 0; i < headers.size(); ++i) {
        m_request->addResponseHeader(headers.at(i), m_reply->rawHeader(headers.at(i)));
    }
    m_request->addResponseHeader("Execution-Time", QString::number(m_time->msecsTo(time))+" ms");
}

void RestClient::slotFinishRequest()
{
    parseResponseHeaders();
    m_reply->close();

    QVariant statusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    QVariant reason = m_reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute );

    m_request->setResponseCode(statusCode.toInt());
    m_request->setMessage(reason.toString());

    releaseReplyResources();
    emit finish();
}

void RestClient::slotReplyResponse()
{
    QString response = m_request->response();
    response += m_reply->readAll();
    m_request->setResponse(response);
}

void RestClient::slotReplyError(QNetworkReply::NetworkError error)
{
    QString error_string;
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

    QVariant statusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    QVariant reason = m_reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute);

    m_request->setError(error_string);
    m_request->setMessage(reason.toString());
    m_request->setResponseCode(statusCode.toInt());

    parseResponseHeaders();
    releaseReplyResources();
    emit finish();
}

void RestClient::releaseReplyResources()
{
    disconnect(m_reply, SIGNAL(readyRead()), this, SLOT(slotReplyResponse()));
    disconnect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), this,SLOT(slotReplyError(QNetworkReply::NetworkError)));
    disconnect(m_reply, SIGNAL(finished()), this, SLOT(slotFinishRequest()));

    QNetworkAccessManager *manager = m_reply->manager();
    m_reply->deleteLater();
    manager->deleteLater();
    delete m_time;
}

