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
#ifndef GIST_H
#define GIST_H

#include <QObject>

class Request;
class RestClient;

class Gist : public QObject
{
    Q_OBJECT
public:
    explicit Gist(QObject *parent = 0);
    ~Gist();
    void sendRequest(Request *request);

signals:
    void published(const QString& url);

public slots:
    void slotFinish();

private:
    RestClient *m_rest;
    Request *m_request;

    QString toJson(Request *request);
    QString getGistUrl(const QString& );
};

#endif // GIST_H
