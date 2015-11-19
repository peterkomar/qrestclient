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
#include "gist.h"
#include "request.h"
#include "restclient.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QMessageBox>

Gist::Gist(QObject *parent)
    : QObject(parent)
{
    m_rest = new RestClient(this);
}

Gist::~Gist()
{
    delete m_rest;
}

void Gist::sendRequest(Request *request)
{
    m_request = new Request("https://api.github.com/gists", "POST");
    m_request->setRaw(toJson(request));
    m_request->addRequestHeader("Content-Type", "application/json");
    connect(m_rest, SIGNAL(finish()), this, SLOT(slotFinish()));
    m_rest->sendRequest(m_request);
}

void Gist::slotFinish()
{    
    if (m_request->responseCode() == 201) {
        emit published(getGistUrl(m_request->response()));
    } else {
        emit published("Error");
    }
    delete m_request;
    m_request = 0;
}

QString Gist::toJson(Request *request)
{
   QString body;
   body = "{"
          "\"description\": \"QRequestClient\","
          "\"public\": false,"
          "\"files\": {"
          "\"request.txt\": {"
          "  \"content\": \"%1\""
          " },"
          "\"response.txt\": {"
          "  \"content\": \"%2\""
          " }"
          "}"
        "}";
   QStringList str = request->toString();

   return body.arg(escapeJson(str[0])).arg(escapeJson(str[1]));
}

QString Gist::escapeJson(const QString& text)
{
    QString res;
    for(QString::const_iterator itr(text.begin()); itr != text.end(); ++itr) {
        if (*itr == '\\') {
            res += "\\\\";
        } else if (*itr == '"') {
            res += "\\\"";
        } else if (*itr == '/') {
            res += "\\/";
        } else if (*itr == '\b') {
             res += "\\b";
        } else if (*itr == '\f') {
            res += "\\f";
        } else if (*itr == '\n') {
            res += "\\n";
        } else if (*itr == '\r') {
            res += "\\r";
        } else if (*itr == '\t') {
            res += "\\t";
        } else {
            res += *itr;
        }
    }

    return res;
}

QString Gist::getGistUrl(const QString& json)
{
    QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject obj = document.object();

    QJsonObject::const_iterator iterator = obj.begin();
    while( iterator != obj.end() ) {
        if (iterator.key() == "html_url") {
            return iterator.value().toString();
        }
        ++iterator;
    }

    return QString("Error");
}

