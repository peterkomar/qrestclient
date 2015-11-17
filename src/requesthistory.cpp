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
#include "requesthistory.h"

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

//////////////////////////////////////////////////////////////////////////////////////////

RequestHistory::RequestHistory()
{
}

RequestHistory::~RequestHistory()
{
    m_database.close();
}

void RequestHistory::init()
{
    QDir home = QDir::home();
    if( !home.exists(".qrestclient") ) {
        if( !home.mkdir(".qrestclient") ) {
            qDebug() << "Fatal create directory";
            return;
        }
    }

    home.cd(".qrestclient");
    QString file = home.absolutePath() + "/history.db";
    bool bNeedToCreate = !QFile::exists(file);
    connect(file);

    if( bNeedToCreate ) {
        createDataBase();
    }
}

bool RequestHistory::connect(const QString &dbName)
{
    m_database = QSqlDatabase::addDatabase("QSQLITE", dbName);
    m_database.setDatabaseName(dbName);
    m_database.setUserName("");
    m_database.setPassword("");

    if (!m_database.open()) {
        QSqlDatabase::removeDatabase(dbName);
        qDebug() << "Cant init QSQLITE";
        return false;
    }

    migrateTo2();

    return true;
}

void RequestHistory::migrateTo2()
{
    QStringList tables = m_database.tables();
    if (tables.contains("response_headers")) {
        return;
    }

    QSqlQuery query(m_database);
    query.exec("CREATE TABLE response_headers ("
               "request_id int,"
               "name varchar,"
               "value varchar)");
}

void RequestHistory::createDataBase()
{
    QSqlQuery query(m_database);

    QStringList queries;
    queries
    << "CREATE TABLE requests ("
                "id int,"
                "date datetime,"
                "code int,"
                "type varchar,"
                "url varchar,"
                "response mediumtext,"
                "error varchar,"
    ")"
    << "CREATE TABLE requests_params ("
                "request_id int,"
                "name varchar,"
                "value varchar"
    ")"
    << "CREATE TABLE request_headers ("
                "request_id int,"
                "name varchar,"
                "value varchar"
    ")"
    << "CREATE TABLE response_headers ("
                "request_id int,"
                "name varchar,"
                "value varchar"
    ")"
    << "CREATE TABLE request_raw ("
                "request_id int,"
                "body varchar,"
                "type_content varchar"
    ")";

    QStringList::const_iterator constIterator;
    for (constIterator = queries.constBegin(); constIterator != queries.constEnd(); ++constIterator) {

        if (!query.exec(*constIterator)) {
          qDebug() << "Failed to execute: " << (*constIterator).toLocal8Bit().constData();
        }
    }
}

void RequestHistory::addRequest(Request *request)
{    
    QSqlQuery q(m_database);

    q.prepare("SELECT max(id) FROM requests");
    if( !q.exec() ) {
        qDebug() << "Error execution query: " << q.lastQuery() << " Error: " << q.lastError();
        throw "Error execution query";
    }

    int index = -1;
    if( q.first() ) {
      index = q.value(0).toInt();
    }

    ++index;

    m_database.transaction();
    QSqlQuery *query = new QSqlQuery(m_database);

    bool result = true;
    query->prepare(""
     "INSERT INTO requests (id, date, code, type, url, response, error)"
     " VALUES (:id, :date, :code, :type, :url, :response, :error)"
    );
    query->bindValue(":id", index);
    query->bindValue(":date", QDateTime::currentDateTime().toString());
    query->bindValue(":code", request->responseCode());
    query->bindValue(":type", request->method());
    query->bindValue(":url", request->url());
    query->bindValue(":response", request->response());
    query->bindValue(":error", request->error());
    query->exec();

    addRequestPairs(index, query, "requests_params", request->requestParams());
    addRequestPairs(index, query, "request_headers", request->requestHeaders());
    addRequestPairs(index, query, "response_headers", request->responseHeaders());

    //Add raw request
    query->prepare(""
     "INSERT INTO request_raw (request_id, body, type_content)"
     " VALUES (:request_id, :body, :type_content)"
    );
    query->bindValue(":request_id", index);
    query->bindValue(":body", request->raw());
    query->bindValue(":type_content", request->rawType());
    query->exec();

    result = m_database.commit();
    if (!result) {
        qDebug() << "Failed to execute: " << query->lastError().text();
        m_database.rollback();
    }

    delete query;
}

void RequestHistory::addRequestPairs(int requestId, QSqlQuery *query, const QString& name,  const QHash<QString, QString>& pair)
{
    QHashIterator<QString, QString> i(pair);
    while (i.hasNext()) {
        i.next();
        query->prepare(""
         "INSERT INTO "+name+" (request_id, name, value)"
         " VALUES (:request_id, :name, :value)"
        );

        query->bindValue(":request_id", requestId);
        query->bindValue(":name", i.key());
        query->bindValue(":value", i.value());
        query->exec();
    }
}

bool RequestHistory::deleteHistory(const QVector<int> requestIds)
{
    m_database.transaction();
    QSqlQuery query(m_database);

    QString ids;
    for (int i = 0; i < requestIds.size(); ++i) {
        ids += QString::number(requestIds.at(i)) + ", ";
    }
    ids = ids.remove(ids.length()-2, 2);

    QStringList queries;
    queries << QString("DELETE FROM requests WHERE id IN (%1)").arg(ids)
            << QString("DELETE FROM requests_params WHERE request_id IN (%1)").arg(ids)
            << QString("DELETE FROM request_headers WHERE request_id IN (%1)").arg(ids)
            << QString("DELETE FROM request_raw WHERE request_id IN (%1)").arg(ids)
            << QString("DELETE FROM response_headers WHERE request_id IN (%1)").arg(ids);

    QStringList::const_iterator constIterator;
    bool sqlOk = true;
    for (constIterator = queries.constBegin(); constIterator != queries.constEnd(); ++constIterator) {
        if (!query.exec(*constIterator) && sqlOk) {
            sqlOk = false;
        }
    }

    query.clear();
    if (sqlOk) {
        sqlOk = m_database.commit();
    }

    if(!sqlOk) {
        qDebug() << "Failed to execute: " << query.lastError().text();
        m_database.rollback();
    }
    query.exec("vacuum");

    return true;
}

QSqlQuery* RequestHistory::getHistory(const QString& filter)
{
    QString order = "ORDER By id DESC";
    QString where = "";
    if (!filter.isEmpty()) {
        where = QString("WHERE id ='%1' "
                        "OR date LIKE '%%1%' "
                        "OR code = '%1' "
                        "OR type LIKE '%1' "
                        "OR url LIKE '%%1%'").arg(filter);
    }

    QSqlQuery *query = new QSqlQuery(m_database);
    if( !query->exec(QString("SELECT * FROM requests %1 %2").arg(where).arg(order)) ) {
        throw "Error execute query";
    }
    return query;
}

Request* RequestHistory::getRequest(int requestId)
{
    QSqlQuery q(m_database);
    q.prepare("SELECT * FROM requests WHERE id = :id");
    q.bindValue(":id", requestId);
    if( !q.exec() ) {
        throw "Error execute query";
    }

    if( !q.first() ) {
        throw false;//Request not found
    }

    Request *request = new Request(q.value(4).toString(), q.value(3).toString());
    request->setResponse(q.value(5).toString());
    request->setResponseCode(q.value(2).toInt());
    request->setError(q.value(6).toString());
    request->setResponseHeadersString(q.value(7).toString());

    //load params
    q.prepare("SELECT * FROM requests_params WHERE request_id = :id");
    q.bindValue(":id", requestId);
    if( !q.exec() ) {
        throw "Error execute query";
    }
    while (q.next()) {
        request->addRequestParam(q.value(1).toString(), q.value(2).toString());
    }

    //load headers
    q.prepare("SELECT * FROM request_headers WHERE request_id = :id");
    q.bindValue(":id", requestId);
    if( !q.exec() ) {
        throw "Error execute query";
    }
    while (q.next()) {
        request->addRequestHeader(q.value(1).toString(), q.value(2).toString());
    }

    //load response headers
    q.prepare("SELECT * FROM response_headers WHERE request_id = :id");
    q.bindValue(":id", requestId);
    if( !q.exec() ) {
        throw "Error execute query";
    }
    while (q.next()) {
        request->addResponseHeader(q.value(1).toString(), q.value(2).toString());
    }

    q.prepare("SELECT * FROM request_raw WHERE request_id = :id");
    q.bindValue(":id", requestId);
    if( !q.exec() ) {
        throw "Error execute query";
    }
    if( q.first() ) {
        request->setRaw(q.value(1).toString(), q.value(2).toString());
    }
    return request;
}
