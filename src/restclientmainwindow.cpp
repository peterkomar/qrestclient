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
#include "request.h"
#include "requesthistory.h"
#include "responsewidget.h"
#include "requestdetailsdlg.h"
#include "toolbar.h"
#include "leftpanel.h"
#include "rightpanel.h"
#include "bottompabel.h"
#include "menu.h"
#include "mainpanel.h"
#include "restclient.h"

#include <QMessageBox>
#include <QApplication>
#include <QStatusBar>
#include <QSettings>
#include <QCloseEvent>
#include <QLineEdit>
#include <QUrlQuery>
#include <QComboBox>
#include <QTextEdit>
#include <QAction>

RestClientMainWindow::RestClientMainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    m_request    = 0;
    m_waitDialog = 0;
    m_restClient = 0;

    setMinimumSize(1000, 600);
    setWindowTitle("QRestClient");

    m_mainPanel   = new MainPanel(this);
    m_toolBar     = new ToolBar(this);
    m_leftPanel   = new LeftPanel(this);
    m_rightPanel  = new RightPanel(this);
    m_bottomPanel = 0;

    try {
        m_history = new RequestHistory();
        m_history->init();
        m_bottomPanel = new BottomPabel(this);
        loadHistory();
    } catch(QString error) {
        delete m_history;
        m_history = 0;
        QMessageBox::warning(this, tr("Init"), error);
    }

    m_menu = new Menu(this);

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
    if (m_history) {
        delete m_history;
        delete m_bottomPanel;
    }
    delete m_toolBar;
    delete m_leftPanel;
    delete m_rightPanel;    
    delete m_menu;
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

void RestClientMainWindow::waitDialog()
{
    m_waitDialog = new QMessageBox(this);
    m_waitDialog->setStandardButtons(QMessageBox::Abort);
    m_waitDialog->setText(tr("Please wait, sending request ..."));
    int btn = m_waitDialog->exec();
    if (btn == QMessageBox::Abort && m_restClient) {
        m_restClient->abort();
    }
}

//slots
void RestClientMainWindow::slotSendRequest()
{
    parseUrlParams();
    QString url = m_toolBar->m_url->text().trimmed();
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

    QTreeWidgetItemIterator paramsIterator(m_leftPanel->m_params);
    while (*paramsIterator) {
        m_request->addRequestParam((*paramsIterator)->text(0), (*paramsIterator)->text(1));
        ++paramsIterator;
    }

    QTreeWidgetItemIterator headerIterator(m_leftPanel->m_headers);
    while (*headerIterator) {
        m_request->addRequestHeader((*headerIterator)->text(0).toUtf8(), (*headerIterator)->text(1).toUtf8());
        ++headerIterator;
    }

    QString raw = m_leftPanel->m_rawContent->toPlainText().trimmed();
    if (!raw.isEmpty()) {
        m_request->setRaw(raw.toUtf8());
    }

    QString contentType = m_leftPanel->m_requestContentType->currentText().trimmed();
    if (!contentType.isEmpty() && !raw.isEmpty()) {
        m_request->addRequestHeader("Content-Type", contentType);
    }

    m_restClient = new RestClient;
    connect(m_restClient, SIGNAL(finish()), this, SLOT(slotFinishRequest()));

    m_restClient->sendRequest(m_request);

    m_mainPanel->m_response->clear();
    waitDialog();
}

void RestClientMainWindow::slotFinishRequest()
{
    m_rightPanel->m_responseHeaders->setText(m_request->responseHeadersAsString());
    m_mainPanel->m_response->setText(m_request->response());
    renderContentType(m_request->getResponseContentType());
    saveHistory();

    m_waitDialog->accept();
    delete m_waitDialog;
    m_waitDialog = 0;
    delete m_request;
    m_request = 0;
    m_restClient->deleteLater();
}

void RestClientMainWindow::saveHistory()
{
    if (!m_history) {
        return;
    }

    m_history->addRequest(m_request);
    m_bottomPanel->m_filterEdit->clear();
    loadHistory();
}

void RestClientMainWindow::loadHistory(const QString& filter)
{
    if (!m_history) {
        return;
    }

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
    if (!m_history) {
        return;
    }

    try {
      m_request = m_history->getRequest(item->text(0).toInt());
    } catch(bool ) {
        return;//Not found
    }

    m_toolBar->m_url->setText(m_request->url());
    m_toolBar->m_method->setCurrentText(m_request->method());
    m_mainPanel->m_response->setText(m_request->response());
    m_rightPanel->m_responseHeaders->setText(m_request->responseHeadersAsString());
    renderContentType(m_request->getResponseContentType());

    //load params
    loadPairs(m_request->requestParams(), m_leftPanel->m_params);
    //Load headers
    loadPairs(m_request->requestHeaders(), m_leftPanel->m_headers);

    m_leftPanel->m_rawContent->setText(m_request->raw());

    QString s = m_request->getRequestContentType();
    if(m_leftPanel->m_requestContentType->findText(s) != -1) {
        m_leftPanel->m_requestContentType->setCurrentText(s);
    } else {
        m_leftPanel->m_requestContentType->addItem(s);
    }

    delete m_request;
    m_request = 0;
}

void RestClientMainWindow::slotRequestDetails()
{
    if (!m_history) {
        return;
    }

    QList<QTreeWidgetItem *> list = m_bottomPanel->m_historyWidget->selectedItems();
    if( list.isEmpty() ) {
        return;
    }

    QTreeWidgetItem *item = list.first();
    Request* request = m_history->getRequest(item->text(0).toInt());
    QString gist = request->getGistId();

    RequestDetailsDlg *dlg = new RequestDetailsDlg(request, this);
    dlg->setWindowTitle(tr("Details: %1").arg(item->text(2).toHtmlEscaped()));
    dlg->exec();
    delete dlg;

    /**
    * Save Gist id if it first time
    */
    if (gist.isEmpty() && !request->getGistId().isEmpty()) {
        m_history->setGistId(item->text(0).toInt(), request->getGistId());
    }

    delete request;
}

void RestClientMainWindow::slotHistoryRemoveSelected()
{
    QList<QTreeWidgetItem *> list = m_bottomPanel->m_historyWidget->selectedItems();
    if( list.isEmpty() ) {
        return;
    }

    int res = QMessageBox::question(this, tr("Confirm Remove"), tr("Are you shure to want to remove selected requests?"));
    if( res == QMessageBox::Yes ) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        clearItems(list);
        QApplication::restoreOverrideCursor();
    }
}

void RestClientMainWindow::clearItems(QList<QTreeWidgetItem *>& items)
{
    if (!m_history) {
        return;
    }

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

void RestClientMainWindow::slotHistoryClear()
{
    int res = QMessageBox::question(this, tr("Confirm Remove"), tr("Are you shure to want to clear request history?"));
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
    loadHistory();
}

void RestClientMainWindow::slotViewJson()
{
    ResponseWidget::type type = m_mainPanel->m_response->render(ResponseWidget::TYPE_JSON);

    if( type != ResponseWidget::TYPE_JSON ) {
        QMessageBox::critical(this, tr("Error parese"), tr("Error parsing JSON"));
        slotNotifyMenuView(type);
    }
}

void RestClientMainWindow::slotViewText()
{
    ResponseWidget::type type = m_mainPanel->m_response->render(ResponseWidget::TYPE_TEXT);
    slotNotifyMenuView(type);
}

void RestClientMainWindow::slotViewCsv()
{
    ResponseWidget::type type = m_mainPanel->m_response->render(ResponseWidget::TYPE_CSV);
    slotNotifyMenuView(type);
}

void RestClientMainWindow::slotNotifyMenuView(int pos)
{
    switch (pos) {
    case ResponseWidget::TYPE_TEXT: m_menu->m_textView->setChecked(true);
        break;
    case ResponseWidget::TYPE_JSON: m_menu->m_jsonView->setChecked(true);
    default:
        break;
    }
}

void RestClientMainWindow::renderContentType(const QString &contentType)
{
    int type = 0;
    if( contentType.indexOf("json", 0, Qt::CaseInsensitive) != -1) {
        type = m_mainPanel->m_response->render(ResponseWidget::TYPE_JSON);
    } else {
        type = m_mainPanel->m_response->render(ResponseWidget::TYPE_TEXT);
    }

    slotNotifyMenuView(type);
}

void RestClientMainWindow::slotAbout()
{
    QMessageBox::about(this, "About", "<b>QRestClient</b> - REST client.<br/>"
                       "<br/> Supports sending GET, POST, PUT, DELETE requests to URL, managing sending params, heders, content and logging sent requests.<br/><br/>"
                       "Author <a href=\"http://peter_komar.byethost17.com/\">Peter Komar</a>"
                       "<br/><br/><b>License:</b> GPL v2"
                       "<br/> 2007 - " + QDateTime::currentDateTime().toString("yyyy") +
                       "<br/><br/><b>Build </b>: 2.0." + QString::number(QDateTime::currentMSecsSinceEpoch()));
}
