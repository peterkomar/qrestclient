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
#include "request.h"

#include <QUrlQuery>
#include <QMessageBox>

RequestHistory::RequestHistory()
    :version("2.2.1")
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
            throw QObject::tr("Can't create directory. Home directory is not writable");
        }
    }

    home.cd(".qrestclient");
    QString file = home.absolutePath() + "/history.db";
    bool bNeedToCreate = !QFile::exists(file);
    connect(file);

    if( bNeedToCreate ) {
        createDataBase();
    } else {
        migrateTo2();
        migrateTo3();
    }
}

void RequestHistory::connect(const QString &dbName)
{
    m_database = QSqlDatabase::addDatabase("QSQLITE", dbName);
    m_database.setDatabaseName(dbName);
    m_database.setUserName("");
    m_database.setPassword("");

    if (!m_database.open()) {
        QSqlDatabase::removeDatabase(dbName);
        throw QObject::tr("Can't init SQLite driver. Plese install SQLite driver or Qt slqlite extension.");
    }
}

/**
* Migration function will be removed later
*/
void RequestHistory::migrateTo2()
{
    QStringList tables = m_database.tables();
    if (tables.contains("settings")) {
        return;
    }

    QSqlQuery *q = new QSqlQuery(m_database);
    if (!tables.contains("response_headers")) {
        query(q, "CREATE TABLE response_headers ("
                   "request_id int,"
                   "name varchar,"
                   "value varchar)");
    }
    query(q, "CREATE TABLE settings ("
                   "name varchar,"
                   "value varchar)");
    query(q, "INSERT INTO settings (name, value)"
                   " VALUES ('ver', '2.0.1')");

    //Migrate requests
    query(q, "ALTER TABLE requests RENAME TO requests_old");
    query(q, "CREATE TABLE requests ("
                    "id int,"
                    "date datetime,"
                    "code int,"//response code
                    "type varchar,"//method of request GET POST and etc
                    "url varchar,"
                    "response mediumtext,"
                    "error varchar,"
                    "message varchar,"
                    "gist_id varchar"
        ")");

    query(q, "SELECT * FROM requests_old");
    QSqlQuery *q2 = new QSqlQuery(m_database);
    while (q->next()) {
        q2->prepare(""
         "INSERT INTO requests (id, date, code, type, url, response, message, gist_id)"
         " VALUES (:id, :date, :code, :type, :url, :response, :message, :gist_id)"
        );
        q2->bindValue(":id", q->value("id").toInt());
        q2->bindValue(":date", q->value("date").toString());
        q2->bindValue(":code", q->value("code").toInt());
        q2->bindValue(":type", q->value("type").toString());
        q2->bindValue(":url", q->value("url").toString());
        q2->bindValue(":response", q->value("response").toString());
        q2->bindValue(":message", q->value("error").toString());
        q2->bindValue(":gist_id", q->value("error").toString());
        query(q2);
        q2->clear();
    }
    query(q, "DROP TABLE requests_old");

    //Migrate request_raw
    query(q, "ALTER TABLE request_raw RENAME TO request_raw_old");
    query(q, "CREATE TABLE request_raw ("
             "request_id int,"
             "body varchar"
    ")");

    query(q, "SELECT * FROM request_raw_old");
    while (q->next()) {
        q2->prepare(""
                    "INSERT INTO request_raw (request_id, body)"
                    " VALUES (:request_id, :body)"
                   );
        q2->bindValue(":request_id", q->value("request_id").toInt());
        q2->bindValue(":body", q->value("body").toString());
        query(q2);
        q2->clear();
    }
    query(q, "DROP TABLE request_raw_old");

    delete q2;
    delete q;
}

/**
* Migration function will be removed later
*/
void RequestHistory::migrateTo3()
{
    QStringList tables = m_database.tables();
    if (tables.contains("groups")) {
        return;
    }

    QSqlQuery *q = new QSqlQuery(m_database);
    q->prepare("UPDATE settings SET value = :value WHERE name = :name");
    q->bindValue(":name", "ver");
    q->bindValue(":value", version);
    query(q);

    //Migrate requests
    query(q, "ALTER TABLE requests RENAME TO requests_old");
    query(q, "CREATE TABLE requests ("
                    "id int,"
                    "group_id int,"
                    "date datetime,"
                    "code int,"//response code
                    "type varchar,"//method of request GET POST and etc
                    "url varchar,"
                    "response mediumtext,"
                    "error varchar,"
                    "message varchar,"
                    "gist_id varchar"
        ")");

    query(q, "SELECT * FROM requests_old");
    QSqlQuery *q2 = new QSqlQuery(m_database);
    while (q->next()) {
        q2->prepare(""
         "INSERT INTO requests (id, group_id, date, code, type, url, response, message, gist_id)"
         " VALUES (:id, :group_id, :date, :code, :type, :url, :response, :message, :gist_id)"
        );
        q2->bindValue(":id", q->value("id").toInt());
        q2->bindValue(":group_id", 0);
        q2->bindValue(":date", q->value("date").toString());
        q2->bindValue(":code", q->value("code").toInt());
        q2->bindValue(":type", q->value("type").toString());
        q2->bindValue(":url", q->value("url").toString());
        q2->bindValue(":response", q->value("response").toString());
        q2->bindValue(":message", q->value("error").toString());
        q2->bindValue(":gist_id", q->value("error").toString());
        query(q2);
        q2->clear();
    }

    query(q, "CREATE TABLE groups ("
                    "id int,"
                    "name varchar"
             ")"
    );

    delete q2;
    delete q;
}

void RequestHistory::createDataBase()
{
    QSqlQuery *q = new QSqlQuery(m_database);

    QStringList queries;
    queries
    << "CREATE TABLE requests ("
                "id int,"
                "date datetime,"
                "code int,"//response code
                "type varchar,"//method of request GET POST and etc
                "url varchar,"
                "response mediumtext,"
                "error varchar,"
                "message varchar,"
                "gist_id varchar"
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
                "body varchar"
    ")"
    << "CREATE TABLE settings ("
                "name varchar,"
                "value varchar"
    ")"
     << "CREATE TABLE groups ("
                "id int,"
                "name varchar"
    ")";

    QStringList::const_iterator constIterator;
    for (constIterator = queries.constBegin(); constIterator != queries.constEnd(); ++constIterator) {
        query(q, *constIterator);
    }

    q->prepare(""
     "INSERT INTO settings (name, value)"
     " VALUES (:name, :value)"
    );
    q->bindValue(":name", "ver");
    q->bindValue(":value", version);
    query(q);
}

void RequestHistory::addRequest(Request *request)
{    
    int index = lastIncrementId("requests");

    m_database.transaction();
    QSqlQuery *query = new QSqlQuery(m_database);

    bool result = true;
    query->prepare(""
     "INSERT INTO requests (id, group_id, date, code, type, url, response, error, message)"
     " VALUES (:id, :group_id, :date, :code, :type, :url, :response, :error, :message)"
    );
    query->bindValue(":id", index);
    query->bindValue(":group_id", request->groupId());
    query->bindValue(":date", QDateTime::currentDateTime().toString());
    query->bindValue(":code", request->responseCode());
    query->bindValue(":type", request->method());
    query->bindValue(":url", request->url());
    query->bindValue(":response", request->response());
    query->bindValue(":error", request->error());
    query->bindValue(":message", request->message());
    query->exec();

    addRequestPairs(index, query, "requests_params", request->requestParams());
    addRequestPairs(index, query, "request_headers", request->requestHeaders());
    addRequestPairs(index, query, "response_headers", request->responseHeaders());

    //Add raw request
    query->prepare(""
     "INSERT INTO request_raw (request_id, body)"
     " VALUES (:request_id, :body)"
    );
    query->bindValue(":request_id", index);
    query->bindValue(":body", request->raw());
    query->exec();

    result = m_database.commit();
    if (!result) {
        qDebug() << "Failed to execute: " << query->lastError().text();
        m_database.rollback();
    }

    delete query;
}

void RequestHistory::setGistId(int requestId, const QString& gistId)
{
     QSqlQuery *q = new QSqlQuery(m_database);
     q->prepare("UPDATE requests SET gist_id = :gist_id WHERE id = :id");
     q->bindValue(":id", requestId);
     q->bindValue(":gist_id", gistId);
     query(q);
     delete q;
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

int RequestHistory::lastIncrementId(const QString& table)
{
    QSqlQuery *q = new QSqlQuery(m_database);
    if( !q->exec("SELECT max(id) FROM " + table) ) {
        qDebug() << "Error execution query: " << q->lastQuery() << " Error: " << q->lastError();
        throw "Error execution query";
    }

    int index = 0;
    if( q->first() ) {
      index = q->value(0).toInt();
    }
    return ++index;
}

void RequestHistory::groupHistory(const QString& name, const QVector<int> requestIds)
{
    QSqlQuery *q = new QSqlQuery(m_database);
    q->prepare("SELECT id FROM groups WHERE name = :name");
    q->bindValue(":name", name);
    query(q);
    int index = 0;
    if( q->first() ) {
        index = q->value(0).toInt();
    } else {
        index = lastIncrementId("groups");
        q->clear();
        q->prepare(""
         "INSERT INTO groups (id, name)"
         " VALUES (:id, :name)"
        );
        q->bindValue(":id", index);
        q->bindValue(":name", name);
        query(q);
    }

    QString ids;
    for (int i = 0; i < requestIds.size(); ++i) {
        ids += QString::number(requestIds.at(i)) + ", ";
    }
    ids = ids.remove(ids.length()-2, 2);

    q->clear();
    q->prepare(QString("UPDATE requests SET group_id = :group_id WHERE id IN (%1)").arg(ids));
    q->bindValue(":group_id", index);
    query(q);
    delete q;
}

void RequestHistory::unGroupHistory(const QVector<int> requestIds)
{
    QSqlQuery *q = new QSqlQuery(m_database);

    QString ids;
    for (int i = 0; i < requestIds.size(); ++i) {
        ids += QString::number(requestIds.at(i)) + ", ";
    }
    ids = ids.remove(ids.length()-2, 2);

    //Get all group ids for all selected requests ids
    q->prepare(QString("SELECT group_id FROM requests WHERE id IN (%1) GROUP BY group_id").arg(ids));
    query(q);
    QVector<int> groupIds;
    while (q->next()) {
        groupIds.append(q->value("group_id").toInt());
    }
    q->clear();

    //Ungroup all selected requests
    q->prepare(QString("UPDATE requests SET group_id = 0 WHERE id IN (%1)").arg(ids));
    query(q);
    q->clear();

    //Delete all groups which don't have requests
    for (int i = 0; i < groupIds.size(); ++i) {
        q->prepare("SELECT count(*) as cnt FROM requests WHERE group_id = :group_id");
        q->bindValue(":group_id", groupIds.at(i));
        query(q);
        if (q->first() && q->value("cnt").toInt() == 0) {
            q->clear();
            q->prepare("DELETE FROM groups WHERE id = :id");
            q->bindValue(":id", groupIds.at(i));
            query(q);
        }
        q->clear();
    }
    delete q;
}

//TODO:  Add checking if group of deleted item doesn't have any other requests and remove it
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
    QString order = "ORDER By r.id DESC";
    QString where = "";
    if (!filter.isEmpty()) {
        where = QString("WHERE r.id ='%1' "
                        "OR r.date LIKE '%%1%' "
                        "OR r.code = '%1' "
                        "OR r.type LIKE '%1' "
                        "OR r.url LIKE '%%1%'").arg(filter);
    }

    QSqlQuery *q = new QSqlQuery(m_database);
    query(q, QString("SELECT r.*, g.name as group_name FROM requests AS r LEFT JOIN groups AS g ON g.id = r.group_id %1 %2").arg(where).arg(order));
    return q;
}

Request* RequestHistory::getRequest(int requestId)
{
    QSqlQuery *q = new QSqlQuery(m_database);
    q->prepare("SELECT * FROM requests WHERE id = :id");
    q->bindValue(":id", requestId);
    query(q);
    if( !q->first() ) {
        delete q;
        throw false;//Request not found
    }

    Request *request = new Request(q->value("url").toString(), q->value("type").toString());
    request->setResponse(q->value("response").toString());
    request->setResponseCode(q->value("code").toInt());
    request->setError(q->value("error").toString());
    request->setMessage(q->value("message").toString());
    QString gistId = q->value("gist_id").toString();
    if (!gistId.isEmpty()) {
        request->setGistId(q->value("gist_id").toString());
    }
    q->clear();

    //load params
    q->prepare("SELECT * FROM requests_params WHERE request_id = :id");
    q->bindValue(":id", requestId);
    query(q);
    while (q->next()) {
        request->addRequestParam(q->value("name").toString(), q->value("value").toString());
    }
    q->clear();

    //load headers
    q->prepare("SELECT * FROM request_headers WHERE request_id = :id");
    q->bindValue(":id", requestId);
    query(q);
    while (q->next()) {
        request->addRequestHeader(q->value("name").toString(), q->value("value").toString());
    }
    q->clear();

    //load response headers
    q->prepare("SELECT * FROM response_headers WHERE request_id = :id");
    q->bindValue(":id", requestId);
    query(q);
    while (q->next()) {
        request->addResponseHeader(q->value("name").toString(), q->value("value").toString());
    }
    q->clear();

    q->prepare("SELECT * FROM request_raw WHERE request_id = :id");
    q->bindValue(":id", requestId);
    query(q);
    if( q->first() ) {
        request->setRaw(q->value("body").toString());
    }
    delete q;
    return request;
}

void RequestHistory::query(QSqlQuery* query, const QString& string)
{
    bool res = true;
    if(string.isEmpty()) {
        res = query->exec();
    } else {
        res = query->exec(string);
    }

    if( !res ) {
        QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Disk Error. Check logs for more details."));
        qDebug() << QString("Error execute query: %1").arg(query->lastQuery());
        delete query;
        throw -1;
    }
}
