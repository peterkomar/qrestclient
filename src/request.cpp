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

Request::Request(const QString& url, const QString& method)
    :m_url(url)
    ,m_method(method)
{}

QString Request::toString()
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
        QHashIterator<QString, QString> i(m_requestParams);
        while (i.hasNext()) {
            i.next();
            query.addQueryItem(i.key(), i.value());
        }

        res += "Encoded Uri: " + query.query(QUrl::FullyEncoded).toUtf8() + "\n";
        res += "Decoded Uri: " + query.query(QUrl::FullyDecoded).toUtf8();
    }

    res += "\n";
    QString raw = m_requestRaw.simplified();
    if (raw.isEmpty()) {
        res += raw + "\n";
    }

    return res;
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

QString Request::getContetnType()
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

