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
#ifndef RESTDIALOGIMPL_H
#define RESTDIALOGIMPL_H

#include <QDialog>
#include <QNetworkReply>
#include "ui_form.h"

class RequestHistory;

class RestDialogImpl : public QDialog, public Ui::RestDialog
{
    Q_OBJECT
public:
    explicit RestDialogImpl(QWidget *parent = 0);
    ~RestDialogImpl();

signals:

public slots:
    void slotEditHeader();
    void slotDeleteHeader();

    void slotEditParam();
    void slotDeleteParam();

    void slotSendRequest();

    void slotHistoryLoad(QTreeWidgetItem*,int);
    void slotRemoveHistoryItem(QTreeWidgetItem*);

    //Requests slots
    void slotReplyResponse();
    void slotReplyFinish();
    void slotReplyError(QNetworkReply::NetworkError error);


private:
    void initConnections();
    void loadHistory();
    void saveHistory(int resposeCode);

    void releaseReplyResources();
    void waitDialog();

    RequestHistory *m_history;
    QNetworkReply *m_reply;

    QDialog *m_waitDialog;
};

#endif // RESTDIALOGIMPL_H
