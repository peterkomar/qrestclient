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
#ifndef RESTCLIENT_H
#define RESTCLIENT_H

#include <QObject>
#include <QNetworkReply>

class Request;
class QTime;

class RestClient : public QObject
{
    Q_OBJECT
public:
    explicit RestClient(QObject *parent = 0);
    ~RestClient();
    void abort();
    void sendRequest(Request *request);

signals:
    void finish();

public slots:
    void slotReplyResponse();
    void slotReplyError(QNetworkReply::NetworkError error);
    void slotFinishRequest();

private:
    QNetworkReply *m_reply;
    Request *m_request;
    QTime *m_time;

    void releaseReplyResources();
    void sendRawRequest(QNetworkAccessManager *manager,
                        QNetworkRequest& request,
                        const QUrlQuery& query);
    void parseResponseHeaders();
};

#endif // RESTCLIENT_H
