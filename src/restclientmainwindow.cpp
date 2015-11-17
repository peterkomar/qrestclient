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

#include "restclientmainwindow.h"
#include "paramslist.h"
#include "resthistorywidget.h"
#include "requesthistory.h"
#include "responsewidget.h"
#include "requestdetailsdlg.h"
#include "toolbar.h"
#include "leftpanel.h"
#include "rightpanel.h"
#include "bottompabel.h"

#include <QTextEdit>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QToolBar>
#include <QTabWidget>
#include <QPushButton>
#include <QDockWidget>
#include <QMessageBox>
#include <QUrlQuery>
#include <QApplication>
#include <QStatusBar>
#include <QMenuBar>
#include <QSettings>
#include <QCloseEvent>
#include <QShortcut>

RestClientMainWindow::RestClientMainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setMinimumSize(1000, 600);
    _gui();

    m_reply = 0;
    m_waitDialog =0;

    m_history = new RequestHistory();
    m_history->init();
    loadHistory();

    QSettings setting("UDL", "qrestclient");
    QByteArray arr = setting.value("gui/state").toByteArray();
    if( !arr.isEmpty() ) {
        restoreState(arr);
    }
    int tab = setting.value("gui/tab_index").toInt();
    m_leftPanel->m_tab->setCurrentIndex(tab);
}

RestClientMainWindow::~RestClientMainWindow()
{
    delete m_history;
    delete m_toolBar;
    delete m_leftPanel;
    delete m_rightPanel;
    delete m_bottomPanel;
}

void RestClientMainWindow::closeEvent(QCloseEvent *event)
{
    QSettings setting("UDL", "qrestclient");
    setting.setValue("gui/state", saveState());
    setting.setValue("gui/tab_index", m_leftPanel->m_tab->currentIndex());

    event->accept();
}

void RestClientMainWindow::keyPressEvent(QKeyEvent * event)
{
    if (event->key() == Qt::Key_Escape) {
        slotHideHistoryFilter();
        event->accept();
    }
}

void RestClientMainWindow::_gui()
{
     setWindowTitle("QRestClient");

     QVBoxLayout *l = new QVBoxLayout;

     m_response = new ResponseWidget(this);
     m_response->setMinimumSize(500, 205);
     l->addWidget(m_response);

     m_errorResponse = new QTextEdit();
     m_errorResponse->setText("Error:");
     m_errorResponse->setReadOnly(true);
     m_errorResponse->setAcceptRichText(false);
     m_errorResponse->setMinimumSize(500, 100);
     m_errorResponse->setMaximumHeight(100);
     l->addWidget(m_errorResponse);

     QWidget *main = new QWidget(this);
     main->setLayout(l);
     setCentralWidget(main);

     m_toolBar = new ToolBar(this);
     m_leftPanel = new LeftPanel(this);
     m_rightPanel = new RightPanel(this);
     m_bottomPanel = new BottomPabel(this);
     setupMenu();
}

void RestClientMainWindow::setupMenu()
{
    QMenu *view = menuBar()->addMenu("View");

    m_jsonView = new QAction("Json", this);
    m_textView = new QAction("Text", this);
    m_csvView  = new QAction("CSV", this);

    QActionGroup *viewGroup = new QActionGroup(this);
    viewGroup->addAction(m_jsonView);
    viewGroup->addAction(m_textView);
    viewGroup->addAction(m_csvView);

    m_jsonView->setCheckable(true);
    m_textView->setCheckable(true);
    m_csvView->setCheckable(true);

    m_textView->setChecked(true);

    view->addAction(m_jsonView);
    view->addAction(m_textView);
    view->addAction(m_csvView);

    connect(m_jsonView, SIGNAL(triggered()), this, SLOT(slotViewJson()));
    connect(m_textView, SIGNAL(triggered()), this, SLOT(slotViewText()));
    connect(m_csvView,  SIGNAL(triggered()), this, SLOT(slotViewCsv()));

    QAction *a = new QAction("About", this);
    QMenu *m = menuBar()->addMenu("Help");
    m->addAction(a);
    connect(a, SIGNAL(triggered()), this, SLOT(slotAbout()));
}

void RestClientMainWindow::saveHistory(int code)
{
    m_request->setResponseCode(code);
    m_request->setResponse(m_response->toText());
    m_request->setError(m_errorResponse->toPlainText());

    m_history->addRequest(m_request);

    m_bottomPanel->m_filterEdit->clear();
    loadHistory();
}

void RestClientMainWindow::loadHistory(const QString& filter)
{    
    QSqlQuery *q = m_history->getHistory(filter);
    m_bottomPanel->m_historyWidget->clear();
    while (q->next()) {
        QTreeWidgetItem *item  = new QTreeWidgetItem(m_bottomPanel->m_historyWidget);
        item->setText(0, q->value(0).toString());
        item->setText(1, q->value(3).toString());
        item->setText(2, q->value(4).toString());
        item->setText(3, q->value(2).toString());
        item->setText(4, q->value(1).toString());
    }
    delete q;
}

void RestClientMainWindow::waitDialog()
{
    m_waitDialog = new QMessageBox(this);
    m_waitDialog->setStandardButtons(QMessageBox::Abort);
    m_waitDialog->setText("Please wait, sending request ...");
    int btn = m_waitDialog->exec();
    if (btn == QMessageBox::Abort && m_reply) {
        m_reply->abort();
    }
}

void RestClientMainWindow::releaseReplyResources()
{
    disconnect(m_reply, SIGNAL(readyRead()), this, SLOT(slotReplyResponse()));
    disconnect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), this,SLOT(slotReplyError(QNetworkReply::NetworkError)));
    disconnect(m_reply, SIGNAL(finished()), this, SLOT(slotFinishRequest()));

    QNetworkAccessManager *m = m_reply->manager();
    m_reply->deleteLater();
    m->deleteLater();

    m_waitDialog->close();
    delete m_waitDialog;
    m_waitDialog = 0;

    delete m_request;
    m_request = 0;
}

void RestClientMainWindow::clearItems(QList<QTreeWidgetItem *>& items)
{
    QVector<int> ids;
    for (int i = 0; i < items.size(); ++i) {
        ids << ((QTreeWidgetItem *)items.at(i))->text(0).toInt();
    }

    if (m_history->deleteHistory(ids)) {
        qDeleteAll(items);
    } else {
        QMessageBox::critical(this, tr("Error delete"), tr("Can't delete item(s) beacuses database"));
    }
}

void RestClientMainWindow::parseUrlParams()
{
    QString url = m_toolBar->m_url->text();
    int pos = url.indexOf("?");
    if( pos == -1 ) {
        return;
    }

    QString queryString = url.mid(pos+1);
    url.remove("?"+queryString);
    m_toolBar->m_url->setText(url);

   QUrlQuery query(queryString);

   QList<QPair<QString, QString> > items = query.queryItems();
   for( int i= 0; i < items.size(); i++ ) {
       QPair<QString, QString> pair = items.at(i);
       QList<QTreeWidgetItem *> params = m_leftPanel->m_params->findItems(pair.first, Qt::MatchFixedString, 0);
       if( params.isEmpty() ) {
           QTreeWidgetItem *item = new QTreeWidgetItem(m_leftPanel->m_params);
           item->setText(0, pair.first);
           item->setText(1, pair.second);
       } else {
           QTreeWidgetItem *w = params.takeFirst();
           w->setText(1, pair.second);
       }
   }
}

void RestClientMainWindow::setTitle(const QString& method, const QString& url)
{
    QString title = windowTitle();
    int index = title.indexOf(" - ");
    if (index != -1) {
        title.remove(index, title.length());
    }

    setWindowTitle(title + QString(" - [%1] %2").arg(method).arg(url));
}

//slots
void RestClientMainWindow::slotSendRequest()
{
    parseUrlParams();
    QString url = m_toolBar->m_url->text();
    if( url.isEmpty()
            || url.left(4) != "http"
            || url == "http://"
            || url == "https://" ) {
        QMessageBox::critical(this, "Error", "URL is incorrect");
        return;
    }

    setTitle(m_toolBar->m_method->currentText(), url);

    if (m_request) {
        delete m_request;
        m_request = 0;
    }
    m_request = new Request(url, m_toolBar->m_method->currentText());

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    QUrl urlObject(url);
    QUrlQuery query;
    QTreeWidgetItemIterator paramsIterator(m_leftPanel->m_params);
    while (*paramsIterator) {
        query.addQueryItem((*paramsIterator)->text(0), (*paramsIterator)->text(1));
        m_request->addRequestParam((*paramsIterator)->text(0), (*paramsIterator)->text(1));
        ++paramsIterator;
    }
    urlObject.setQuery(query);

    QNetworkRequest request(urlObject);

    //Ignore SSL errors
    QSslConfiguration conf = request.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(conf);

    QTreeWidgetItemIterator headerIterator(m_leftPanel->m_headers);
    while (*headerIterator) {
        request.setRawHeader((*headerIterator)->text(0).toUtf8(), (*headerIterator)->text(1).toUtf8());
        m_request->addRequestHeader((*headerIterator)->text(0).toUtf8(), (*headerIterator)->text(1).toUtf8());
        ++headerIterator;
    }

    QByteArray rawBody  = m_leftPanel->m_rawContent->toPlainText().toUtf8();
    QString contentType = m_leftPanel->m_requestContentType->currentText();
    m_request->setRaw(rawBody, contentType);

    if( m_toolBar->m_method->currentText() == "GET" ) {
        m_reply = manager->get(request);
    } else if( m_toolBar->m_method->currentText() == "POST" ) {
        sendRawRequest(true, manager, request, query, rawBody, contentType);
    } else if( m_toolBar->m_method->currentText() == "PUT" ) {
        sendRawRequest(false, manager, request, query, rawBody, contentType);
    } else if( m_toolBar->m_method->currentText() == "DELETE" ) {
        m_reply = manager->deleteResource(request);
    } else {
        QMessageBox::critical(this, "Error", "Unsupported method");
        return;
    }

    m_time = QTime::currentTime();

    connect(m_reply, SIGNAL(readyRead()), this, SLOT(slotReplyResponse()));
    connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), this,SLOT(slotReplyError(QNetworkReply::NetworkError)));
    connect(m_reply, SIGNAL(finished()), this, SLOT(slotFinishRequest()));

    m_response->clear();
    m_errorResponse->clear();
    waitDialog();
}

void RestClientMainWindow::sendRawRequest(bool isPost,
                                          QNetworkAccessManager *manager,
                                          QNetworkRequest& request,
                                          const QUrlQuery& query,
                                          const QByteArray& rawBody,
                                          const QString& contentType)
{
    QByteArray raw = rawBody;
    if( rawBody.isEmpty() ) {
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        raw = query.query(QUrl::FullyEncoded).toUtf8();
    } else {
        QByteArray postDataSize = QByteArray::number(raw.size());
        request.setRawHeader("Content-Type", contentType.toLatin1());
        request.setRawHeader("Content-Length", postDataSize);
        m_request->addRequestHeader("Content-Length", postDataSize);
    }

    if (isPost) {
        m_reply = manager->post(request, raw);
    } else {
        m_reply = manager->put(request, raw);
    }
}

void RestClientMainWindow::renderResponseHeaders()
{
    QTime time = QTime::currentTime();
    QList<QByteArray> headers = m_reply->rawHeaderList();

    QString contetType;
    for (int i = 0; i < headers.size(); ++i) {
        if( headers.at(i).toLower() == "content-type") {
            contetType = m_reply->rawHeader(headers.at(i));
        }
        m_request->addResponseHeader(headers.at(i), m_reply->rawHeader(headers.at(i)));
    }
    m_request->addResponseHeader("Execution-Time", QString::number(m_time.msecsTo(time))+" ms");
    renderContentType(contetType);
    m_rightPanel->m_responseHeaders->setText(m_request->responseHeadersAsString());
}

void RestClientMainWindow::slotFinishRequest()
{
    renderResponseHeaders();
    m_reply->close();
    saveHistory(200);
    releaseReplyResources();
}

void RestClientMainWindow::slotReplyResponse()
{
    m_response->append(m_reply->readAll());
}

void RestClientMainWindow::slotReplyError(QNetworkReply::NetworkError error)
{
    renderResponseHeaders();

    QString error_string;
    switch( error) {
      case QNetworkReply::ConnectionRefusedError :
          error_string += tr("the remote server refused the connection (the server is not accepting requests)");
          break;

       case QNetworkReply::HostNotFoundError :
          error_string += tr("the remote host name was not found (invalid hostname)");
          break;

      default :
          error_string +=  m_reply->errorString();
          break;
    }

    m_errorResponse->setText(error_string);
    saveHistory(error);
    releaseReplyResources();
}

void RestClientMainWindow::slotSelectedHistory()
{
    QList<QTreeWidgetItem*> list = m_bottomPanel->m_historyWidget->selectedItems();
    if (!list.isEmpty()) {
       QTreeWidgetItem *item = list.takeFirst();
       slotHistoryLoad(item);
    }
}

void RestClientMainWindow::loadPairs(const QHash<QString, QString>& pair, ParamsList* list)
{
    QTreeWidgetItem *i = 0;
    list->clear();
    QHashIterator<QString, QString> iterator(pair);
    while (iterator.hasNext()) {
        iterator.next();
        i  = new QTreeWidgetItem(list);
        i->setText(0, iterator.key());
        i->setText(1, iterator.value());
    }
}

void RestClientMainWindow::slotHistoryLoad(QTreeWidgetItem *item)
{
    try {
      m_request = m_history->getRequest(item->text(0).toInt());
    } catch(bool ) {
        return;//Not found
    }

    m_toolBar->m_url->setText(m_request->url());
    m_toolBar->m_method->setCurrentText(m_request->method());
    m_response->setText(m_request->response());
    m_errorResponse->setText(m_request->error());
    m_rightPanel->m_responseHeaders->setText(m_request->responseHeadersAsString());
    renderContentType(m_request->getContetnType());

    //load params
    loadPairs(m_request->requestParams(), m_leftPanel->m_params);
    //Load headers
    loadPairs(m_request->requestHeaders(), m_leftPanel->m_headers);

    m_leftPanel->m_rawContent->setText(m_request->raw());

    QString s = m_request->rawType();
    if(m_leftPanel->m_requestContentType->findText(s) != -1) {
        m_leftPanel->m_requestContentType->setCurrentText(s);
    } else {
        m_leftPanel->m_requestContentType->addItem(s);
    }
}

void RestClientMainWindow::slotRequestDetails()
{
    QList<QTreeWidgetItem *> list = m_bottomPanel->m_historyWidget->selectedItems();
    if( list.isEmpty() ) {
        return;
    }

    QTreeWidgetItem *item = list.first();

    RequestDetailsDlg *dlg = new RequestDetailsDlg(this);
    Request* request = m_history->getRequest(item->text(0).toInt());
    dlg->setRequest(request->toString());
    dlg->setWindowTitle(tr("Details: %1").arg(item->text(2).toHtmlEscaped()));
    dlg->exec();
    delete request;
    delete dlg;    
}

void RestClientMainWindow::slotHistoryRemoveSelected()
{
    QList<QTreeWidgetItem *> list = m_bottomPanel->m_historyWidget->selectedItems();
    if( list.isEmpty() ) {
        return;
    }

    int res = QMessageBox::question(this, "Confirm Remove", "Are you shure to want to remove selected requests?");
    if( res == QMessageBox::Yes ) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        clearItems(list);
        QApplication::restoreOverrideCursor();
    }
}

void RestClientMainWindow::slotHistoryClear()
{
    int res = QMessageBox::question(this, "Confirm Remove", "Are you shure to want to clear request history?");
    if( res != QMessageBox::Yes ) {
        return;
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    int countItems = m_bottomPanel->m_historyWidget->topLevelItemCount();
    QList<QTreeWidgetItem *> list;
    for( int i=0; i < countItems; i++ ) {
        list.append(m_bottomPanel->m_historyWidget->topLevelItem(i));
    }
    clearItems(list);
    loadHistory();
    m_bottomPanel->m_filterEdit->clear();

    QApplication::restoreOverrideCursor();
}

void RestClientMainWindow::slotFilterHistoryItems(const QString& text)
{
    loadHistory(text);
}

void RestClientMainWindow::slotShowHistoryFilter()
{
    m_bottomPanel->m_filterEdit->setVisible(true);
    m_bottomPanel->m_filterEdit->setFocus();
}

void RestClientMainWindow::slotHideHistoryFilter()
{
    m_bottomPanel->m_filterEdit->setVisible(false);
}

void RestClientMainWindow::slotViewJson()
{
    ResponseWidget::type type = m_response->render(ResponseWidget::TYPE_JSON);

    if( type != ResponseWidget::TYPE_JSON ) {
        QMessageBox::critical(this, tr("Error parese"), tr("Error parsing JSON"));
        slotNotifyMenuView(type);
    }
}

void RestClientMainWindow::slotViewText()
{
    ResponseWidget::type type = m_response->render(ResponseWidget::TYPE_TEXT);
    slotNotifyMenuView(type);
}

void RestClientMainWindow::slotViewCsv()
{
    ResponseWidget::type type = m_response->render(ResponseWidget::TYPE_CSV);
    slotNotifyMenuView(type);
}

void RestClientMainWindow::slotNotifyMenuView(int pos)
{
    switch (pos) {
    case ResponseWidget::TYPE_TEXT: m_textView->setChecked(true);
        break;
    case ResponseWidget::TYPE_JSON: m_jsonView->setChecked(true);
    default:
        break;
    }
}

void RestClientMainWindow::renderContentType(const QString &contentType)
{
    int type = 0;
    if( contentType.indexOf("json", 0, Qt::CaseInsensitive) != -1) {
        type = m_response->render(ResponseWidget::TYPE_JSON);
    } else {
        type = m_response->render(ResponseWidget::TYPE_TEXT);
    }

    slotNotifyMenuView(type);
}

void RestClientMainWindow::slotAbout()
{
    QMessageBox::about(this, "About", "<b>QRestClient</b> - REST client for WEB services developers.<br/>"
                       "<br/> Supports sending GET, POST, PUT, DELETE requests to URL, managing sending params, heders, contets and logging sended requests.<br/><br/>"
                       "Author <a href=\"http://peter_komar.byethost17.com/\">Peter Komar</a>"
                       "<br/><br/><b>License:</b> GPL v2"
                       "<br/> 2007 - " + QDateTime::currentDateTime().toString("yyyy") +
                       "<br/><br/><b>Build </b>: 2.0." + QString::number(QDateTime::currentMSecsSinceEpoch()));
}
