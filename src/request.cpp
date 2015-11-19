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

Request::Request(const QString& url, const QString& method)
    :m_url(url)
    ,m_method(method)
{}

QStringList Request::toString()
{    
    QString res;

    res = "HTTP Request\n\n";
    res += m_method + " " + m_url + "\n\n";
    //Build headers
    QHashIterator<QString, QString> i(m_requestHeaders);
    while (i.hasNext()) {
        i.next();
        res += i.key() + ": " + i.value() +"\n";
    }

    if (!m_requestParams.isEmpty()) {
        QUrlQuery query;
        i = m_requestParams;
        while (i.hasNext()) {
            i.next();
            query.addQueryItem(i.key(), i.value());
        }

        res += "Encoded Uri: " + query.query(QUrl::FullyEncoded).toUtf8() + "\n";
        res += "Decoded Uri: " + query.query(QUrl::FullyDecoded).toUtf8();
    }

    res += "\n";
    QString raw = m_requestRaw.simplified();
    if (!raw.isEmpty()) {
        res += raw + "\n";
    }

    //Build response
    QString response = "HTTP Response\n\n";
    response += statusMessage() + "\n";
    if (!m_responseHeaders.isEmpty()) {
        i = m_responseHeaders;
        while (i.hasNext()) {
            i.next();
            response += i.key() + ": " + i.value() +"\n";
        }
    }
    response += "\n\n";
    QString text = m_response.simplified();
    if (!text.isEmpty()) {
        response += text + "\n";
    }

    return QStringList() << res << response;
}

QString Request::requestToHtml()
{
    QString res(style());
    res += QString("<b>%1</b><br /><br />").arg("HTTP Request");
    res += QString("<span class='method'>%1</span> %2<br />").arg(m_method).arg(m_url);
    //Build headers
    QHashIterator<QString, QString> i(m_requestHeaders);
    while (i.hasNext()) {
        i.next();
        res += QString("<span class='tagHeader'>%1</span>: %2<br />").arg(i.key()).arg(i.value());
    }



    if (!m_requestParams.isEmpty()) {
        res += "<br />";
        QUrlQuery query;
        i = m_requestParams;
        while (i.hasNext()) {
            i.next();
            query.addQueryItem(i.key(), i.value());
        }

        res += QString("<span class='tagHeader'>%1</span>: %2<br />")
                .arg(QObject::tr("Encoded Uri"))
                .arg(query.query(QUrl::FullyEncoded));
        res += QString("<span class='tagHeader'>%1</span>: %2<br />")
                .arg(QObject::tr("Decoded Uri"))
                .arg(query.query(QUrl::FullyDecoded));
    }

    QString raw = m_requestRaw.simplified();
    if (!raw.isEmpty()) {
        res += "<br />";
        res += QString("<code>%1</code>").arg(raw);
    }
    return res;
}

QString Request::style()
{
    return "<style >"
            "  .tagHeader {"
            "    color: #138fd5;"
            "   }"
            "   .method {"
            "    color: #289f1b;"
            "   }"
           "   .status {"
           "    color: #ff2200;"
           "   }"
            "</style>";
}

QString Request::responseToHtml()
{
    QString res(style());
    res += QString("<b>%1</b><br /><br />").arg("HTTP Response");

    res += QString("<span class='%1'>%2</span><br />")
            .arg((i_responseCode == 200)? "method" : "status")
            .arg(statusMessage());

    QHashIterator<QString, QString> i(m_responseHeaders);
    while (i.hasNext()) {
        i.next();
        res += QString("<span class='tagHeader'>%1</span>: %2<br />").arg(i.key()).arg(i.value());
    }

    QString text = m_response.simplified();
    if (!text.isEmpty()) {
        res += "<br />";
        res += QString("<code>%1</code>").arg(text);
    }

    return res;
}

QString Request::statusMessage()
{
    return  QString("%1 %2").arg(i_responseCode).arg(m_message);
}

QString Request::responseHeadersAsString()
{
    //Supports old version
    if (!m_responseHeadersString.isEmpty()) {
        return m_responseHeadersString;
    } else {
        QString res;
        QHashIterator<QString, QString> i(m_responseHeaders);
        while (i.hasNext()) {
            i.next();
            res += "<b>"+i.key() + ":</b> " + i.value() +"<br />";
        }
        return res;
    }
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
    //Support old version
    if (!m_responseHeadersString.isEmpty()) {
        int pos = 0;

        if((pos = m_responseHeadersString.indexOf("content-type:", 0, Qt::CaseInsensitive)) != -1) {
            int end = m_responseHeadersString.indexOf("\n", pos+1);
            type = m_responseHeadersString.mid(pos, end-pos).trimmed();
        }
    } else {
        if (m_responseHeaders.contains("Content-Type")) {
            type = m_responseHeaders["Content-Type"];
        }
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

