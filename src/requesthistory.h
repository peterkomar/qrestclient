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
#ifndef REQUESTHISTORY_H
#define REQUESTHISTORY_H

#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QCryptographicHash>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QCoreApplication>
#include <QVector>

class Request;

class RequestHistory
{
public:
    RequestHistory();
    ~RequestHistory();
    void init();

    void addRequest(Request *request);
    void setGistId(int requestid, const QString& gistId);
    void groupHistory(const QString& name, const QVector<int> requestIds);
    void unGroupHistory(const QVector<int> requestIds);
    bool deleteHistory(const QVector<int> requestIds);
    QSqlQuery* getHistory(const QString& filter);
    Request* getRequest(int requestId);

private:
    void connect(const QString& name);
    void createDataBase();
    void addRequestPairs(int requestId, QSqlQuery *query, const QString& name,  const QHash<QString, QString>& pair);

    //Migration functions block
    void migrateTo2();
    void migrateTo3();
    //End migration functions block

    void query(QSqlQuery* query, const QString& string = "");
    int lastIncrementId(const QString& table);

    QSqlDatabase m_database;
    const QString version;
};

#endif // REQUESTHISTORY_H
