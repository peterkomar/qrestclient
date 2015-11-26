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
#ifndef REQUEST_H
#define REQUEST_H

#include <QString>
#include <QHash>

class Request
{
public:
    Request(const QString& url, const QString& method);

    //Format functions
    QStringList toString();
    QString requestToHtml();
    QString responseToHtml();

    QString getResponseContentType();
    QString responseHeadersAsString();
    QString getRequestContentType();
    QString statusMessage();

    QString getGistId();
    void setGistId(const QString& id);

    QString url() const { return m_url; }
    QString method() const { return m_method; }


    QString response() const { return m_response; }
    void setResponse(const QString& response) { m_response = response; }

    QString message() const { return m_message; }
    void setMessage(const QString& message) { m_message = message; }

    QString error() const { return m_error; }
    void setError(const QString& error) { m_error = error; }

    int responseCode() { return i_responseCode; }
    void setResponseCode(int code) { i_responseCode = code; }

    void setRaw(const QString& raw) { m_requestRaw = raw; }
    QString raw() const { return m_requestRaw; }

    void addRequestHeader(const QString& key, const QString& value);
    QHash<QString, QString> requestHeaders() const { return m_requestHeaders; }

    void addResponseHeader(const QString& key, const QString& value);
    QHash<QString, QString> responseHeaders() const { return m_responseHeaders; }

    void addRequestParam(const QString& key, const QString& value);
    QHash<QString, QString> requestParams() const { return m_requestParams; }

    //Support old version
    void setResponseHeadersString(const QString& headers) { m_responseHeadersString = headers; }

private:
    QString m_url;
    QString m_method;
    QString m_response;
    QString m_message;
    QString m_error;
    QHash<QString, QString> m_requestHeaders;
    QHash<QString, QString> m_requestParams;
    QString m_requestRaw;
    QHash<QString, QString> m_responseHeaders;
    int i_responseCode;

    //For support old versions
    QString m_responseHeadersString;

    QString style();
};

#endif // REQUEST_H
