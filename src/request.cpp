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
#include "request.h"

#include <QUrlQuery>
#include <QDebug>
#include <QJsonDocument>
#include <QStringList>

Request::Request(const QString& url, const QString& method)
    :m_url(url)
    ,m_method(method)
    ,i_group_id(0)
{}

QStringList Request::toString()
{    
    QString request("HTTP Request\n\n");
    request.append(QString("%1 %2\n\n").arg(m_method).arg(m_url));
    //Build headers
    QHashIterator<QString, QString> i(m_requestHeaders);
    while (i.hasNext()) {
        i.next();
        request.append(QString("%1: %2\n").arg(i.key()).arg(i.value()));
    }

    if (!m_requestParams.isEmpty()) {
        QUrlQuery query;
        i = m_requestParams;
        while (i.hasNext()) {
            i.next();
            query.addQueryItem(i.key(), i.value());
        }
        request.append(QString("Encoded Uri: %1\nDecoded Uri: %2\n")
                       .arg(query.query(QUrl::FullyEncoded))
                       .arg(query.query(QUrl::FullyDecoded)));
    }

    QString raw = format(m_requestRaw, getRequestContentType());
    if (!raw.isEmpty()) {
        request.append(raw);
    }

    //Build response
    QString response("HTTP Response\n\n");
    response.append(statusMessage(false)).append("\n");
    if (!m_responseHeaders.isEmpty()) {
        i = m_responseHeaders;
        while (i.hasNext()) {
            i.next();
            response.append(QString("%1: %2\n").arg(i.key()).arg(i.value()));
        }
    }
    response.append("\n\n");
    QString text = format(m_response, getResponseContentType());
    if (!text.isEmpty()) {
        response.append(text);
    }

    return QStringList() << request << response;
}

QString Request::requestToHtml()
{
    QString res(style());
    res.append(QString("<b>%1</b><br /><br />").arg("HTTP Request"));
    res.append(QString("<span class='method'>%1</span> %2<br />").arg(m_method).arg(m_url));
    //Build headers
    QHashIterator<QString, QString> i(m_requestHeaders);
    while (i.hasNext()) {
        i.next();
        res.append(QString("<span class='tagHeader'>%1</span>: %2<br />").arg(i.key()).arg(i.value()));
    }

    //Build Uri
    if (!m_requestParams.isEmpty()) {
        res.append("<br />");
        QUrlQuery query;
        i = m_requestParams;
        while (i.hasNext()) {
            i.next();
            query.addQueryItem(i.key(), i.value());
        }

        res.append(QString("<span class='tagHeader'>%1</span>: %2<br />"
                           "<span class='tagHeader'>%3</span>: %4<br />")
                   .arg(QObject::tr("Encoded Uri"))
                   .arg(query.query(QUrl::FullyEncoded))
                   .arg(QObject::tr("Decoded Uri"))
                   .arg(query.query(QUrl::FullyDecoded)));
    }

    QString raw = format(m_requestRaw, getRequestContentType());
    if (!raw.isEmpty()) {
        raw.replace(" ", "&nbsp;").replace("\n", "<br />");
        res.append(QString("<br /><code>%1</code>").arg(raw));
    }
    return res;
}

QString Request::style()
{
    return QString("<style >"
            "  .tagHeader {"
            "    color: #138fd5;"
            "   }"
            "   .method {"
            "    color: #289f1b;"
            "   }"
           "   .status {"
           "    color: #ff2200;"
           "   }"
            "</style>");
}

QString Request::responseToHtml()
{
    QString res(style());
    res.append(QString("<b>%1</b><br /><br />%2<br />")
               .arg("HTTP Response")
               .arg(statusMessage()));

    QHashIterator<QString, QString> i(m_responseHeaders);
    while (i.hasNext()) {
        i.next();
        res.append(QString("<span class='tagHeader'>%1</span>: %2<br />").arg(i.key()).arg(i.value()));
    }

    QString text = format(m_response, getResponseContentType());
    if (!text.isEmpty()) {
        text.replace(" ", "&nbsp;").replace("\n", "<br />");
        res.append(QString("<br /><code>%1</code>").arg(text));
    }

    return res;
}

QString Request::statusMessage(bool asHtml)
{
    if (asHtml) {
        return QString("<span class='%1'>%2 %3</span>")
                .arg((i_responseCode >= 200 && i_responseCode < 300)? "method" : "status")
                .arg(i_responseCode)
                .arg(m_message);
    } else {
        return QString("%2 %3")
                .arg(i_responseCode)
                .arg(m_message);
    }
}

QString Request::responseHeadersAsString()
{
    QString res(style());
    res.append(statusMessage());
    res.append("<br />");
    QHashIterator<QString, QString> i(m_responseHeaders);
    while (i.hasNext()) {
        i.next();
        res.append(QString("<b>%1:</b> %2<br />").arg(i.key()).arg(i.value()));
    }

    if (!m_error.isEmpty()) {
        res.append(QString("<br /><b>Server Replied:</b> %1").arg(m_error));
    }

    return res;
}

QString Request::getGistId()
{
    if (m_responseHeaders.contains("Gist-ID")) {
        return m_responseHeaders["Gist-ID"];
    }
    return QString("");
}

void Request::setGistId(const QString& id)
{
    m_responseHeaders["Gist-ID"] = id;
}

QString Request::getRequestContentType()
{
    if (m_requestHeaders.contains("Content-Type")) {
        return m_requestHeaders["Content-Type"];
    }
    return QString("");
}

QString Request::getResponseContentType()
{
    QString type = "text";
    if (m_responseHeaders.contains("Content-Type")) {
        type = m_responseHeaders["Content-Type"];
    }
    return type;
}
void Request::addRequestHeader(const QString& key, const QString& value)
{
    m_requestHeaders.insert(key, value);
}

void Request::addResponseHeader(const QString& key, const QString& value)
{
    m_responseHeaders.insert(key, value);
}

void Request::addRequestParam(const QString& key, const QString& value)
{
    m_requestParams.insert(key, value);
}

QString Request::format(const QString& text, const QString& type)
{
    QString s = text.simplified();
    if( type.indexOf("json", 0, Qt::CaseInsensitive) != -1) {
        s = parseJson(s);
    }

    return s;
}

QString Request::parseJson(const QString &json)
{
    QString s = json;
    QJsonParseError *error = new QJsonParseError;
    QJsonDocument doc = QJsonDocument::fromJson(s.toUtf8(), error);
    if (error->error == QJsonParseError::NoError) {
        s = doc.toJson(QJsonDocument::Indented);
    }
    delete error;
    return s;
}

