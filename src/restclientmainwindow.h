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

class RequestHistory;
class ParamsList;
class QTreeWidgetItem;
class QMessageBox;
class Request;
class ToolBar;
class LeftPanel;
class RightPanel;
class BottomPabel;
class Menu;
class MainPanel;
class RestClient;

class RestClientMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit RestClientMainWindow(QWidget *parent = 0);
    ~RestClientMainWindow();

signals:

public slots:
    //Requests slots
    void slotSendRequest();
    void slotFinishRequest();

    void slotHistoryLoad(QTreeWidgetItem*);
    void slotSelectedHistory();
    void slotHistoryRemoveSelected();
    void slotHistoryClear();
    void slotFilterHistoryItems(const QString& );
    void slotShowHistoryFilter();
    void slotHideHistoryFilter();
    void slotRequestDetails();
    void slotGroup();
    void slotUnGroup();

    void slotViewMode();

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
    Menu *m_menu;
    MainPanel *m_mainPanel;
    RestClient *m_restClient;
    RequestHistory *m_history;
    Request *m_request;

    void loadHistory(const QString& query = "");
    void loadPairs(const QHash<QString, QString>& pair, ParamsList* list);
    void saveHistory();
    void waitDialog();
    void clearItems(QList<QTreeWidgetItem*>& items);
    void parseUrlParams();
    void renderContentType(const QString& contentType);
    void setTitle(const QString& method, const QString& url);
};

#endif // RESTCLIENTMAINWINDOW_H
