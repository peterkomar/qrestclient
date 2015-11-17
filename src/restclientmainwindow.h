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

#ifndef RESTCLIENTMAINWINDOW_H
#define RESTCLIENTMAINWINDOW_H

#include <QMainWindow>
#include <QNetworkReply>
#include <QTime>

class QTextEdit;
class RequestHistory;
class QLineEdit;
class QComboBox;
class ParamsList;
class RestHistoryWidget;
class RequestHistory;
class QTreeWidgetItem;
class QMessageBox;
class QTabWidget;
class ResponseWidget;
class Request;
class ToolBar;
class LeftPanel;
class RightPanel;
class BottomPabel;

class RestClientMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit RestClientMainWindow(QWidget *parent = 0);
    ~RestClientMainWindow();

signals:

public slots:
     //Requests slots
    void slotFinishRequest();
    void slotSendRequest();
    void slotReplyResponse();
    void slotReplyError(QNetworkReply::NetworkError error);

    void slotHistoryLoad(QTreeWidgetItem*);
    void slotSelectedHistory();
    void slotHistoryRemoveSelected();
    void slotHistoryClear();
    void slotFilterHistoryItems(const QString& );
    void slotShowHistoryFilter();
    void slotHideHistoryFilter();
    void slotRequestDetails();

    void slotViewJson();
    void slotViewText();
    void slotViewCsv();
    void slotAbout();

    void slotNotifyMenuView(int pos);

protected:
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent * event);


private:
    QMessageBox *m_waitDialog;

    ToolBar *m_toolBar;
    LeftPanel *m_leftPanel;
    RightPanel *m_rightPanel;
    BottomPabel *m_bottomPanel;


    ResponseWidget *m_response;
    QTextEdit *m_errorResponse;

    RequestHistory *m_history;
    QNetworkReply *m_reply;

    QTime m_time;

    QAction *m_jsonView;
    QAction *m_textView;
    QAction *m_csvView;

    Request *m_request;


    void loadHistory(const QString& query = "");
    void loadPairs(const QHash<QString, QString>& pair, ParamsList* list);
    void saveHistory(int resposeCode);

    void releaseReplyResources();
    void waitDialog();

    void _gui();

    void setupMenu();
    void sendRawRequest(bool isPost,
                        QNetworkAccessManager *manager,
                        QNetworkRequest& request,
                        const QUrlQuery& query,
                        const QByteArray& rawBody,
                        const QString& contentType);

    void clearItems(QList<QTreeWidgetItem*>& items);
    void parseUrlParams();

    void renderContentType(const QString& contentType);

    void renderResponseHeaders();
    void setTitle(const QString& method, const QString& url);
};

#endif // RESTCLIENTMAINWINDOW_H
