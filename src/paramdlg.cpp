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
#include "paramdlg.h"

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>

ParamDlg::ParamDlg(Mode mode, QWidget *parent) :
    QDialog(parent)
  ,i_mode(mode)
  ,m_prevHeader("")
{
    QGridLayout *gridLayout = new QGridLayout(this);

    switch (i_mode) {
    case MODE_HEADER:
        guiHeader(gridLayout);
        initPredefinedHeaders();
        setWindowTitle(tr("Headers"));
        break;
    case MODE_REQUEST:
        guiRequest(gridLayout);
        setWindowTitle(tr("Params"));
        break;
    }

    QPushButton *okBtn = new QPushButton(tr("OK"), this);
    gridLayout->addWidget(okBtn, 0, 2, 1, 1);

    QPushButton *cancelBtn = new QPushButton(tr("Cancel"), this);
    gridLayout->addWidget(cancelBtn, 1, 2, 1, 1);

    connect(cancelBtn, SIGNAL(clicked()), this, SLOT(reject()));
    connect(okBtn, SIGNAL(clicked()), this, SLOT(accept()));

    resize(400, 80);
}

void ParamDlg::guiHeader(QGridLayout *gridLayout)
{
    QLabel *labelName = new QLabel(tr("Header:"), this);
    gridLayout->addWidget(labelName, 0, 0, 1, 1);

    m_cName = new QComboBox(this);
    m_cName->setEditable(true);
    m_cName->setMinimumWidth(270);
    gridLayout->addWidget(m_cName, 0, 1, 1, 1);

    QLabel *labelValue = new QLabel(tr("Value"), this);
    gridLayout->addWidget(labelValue, 1, 0, 1, 1);

    m_cValue = new QComboBox(this);
    m_cValue->setEditable(true);
    m_cValue->setMinimumWidth(270);
    gridLayout->addWidget(m_cValue, 1, 1, 1, 1);

    connect(m_cName, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotChangeHeader(QString)));
}

void ParamDlg::guiRequest(QGridLayout *gridLayout)
{
    QLabel *label = new QLabel(tr("Name:"), this);
    gridLayout->addWidget(label, 0, 0, 1, 1);
    m_name = new QLineEdit(this);
    gridLayout->addWidget(m_name, 0, 1, 1, 1);

    QLabel *label_2 = new QLabel(tr("Value:"), this);
    gridLayout->addWidget(label_2, 1, 0, 1, 1);
    m_value = new QLineEdit(this);
    gridLayout->addWidget(m_value, 1, 1, 1, 1);
}

void ParamDlg::setName(const QString &name)
{
    if (i_mode == MODE_HEADER) {
        m_cName->addItem(name);
        m_cName->setCurrentText(name);
    } else {
        m_name->setText(name);
    }
}

QString ParamDlg::getName() const
{
    if (i_mode == MODE_HEADER) {
        return m_cName->currentText();
    } else {
        return m_name->text();
    }
}

void ParamDlg::setValue(const QString &value)
{
    if (i_mode == MODE_HEADER) {
        m_cValue->addItem(value);
        m_cValue->setCurrentText(value);
    } else {
        m_value->setText(value);
    }
}

QString ParamDlg::getValue() const
{
    if (i_mode == MODE_HEADER) {
        return m_cValue->currentText();
    } else {
        return m_value->text();
    }
}

void ParamDlg::slotChangeHeader(const QString &text)
{
    QString current = m_cValue->currentText();
    m_cValue->clear();
    //Check if item from prev header values
    if (m_preHeaders.contains(m_prevHeader)) {
        if (!current.isEmpty() && !m_preHeaders[m_prevHeader].contains(current)) {
            m_cValue->addItem(current);
        }
    }

    if (m_preHeaders.contains(text)) {
        m_prevHeader = text;
        QStringList list = m_preHeaders[text];
        m_cValue->addItems(list);
    }
}

void ParamDlg::initPredefinedHeaders()
{
    QStringList list;

    list << "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_5) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2490.86 Safari/537.36"
         << "Opera/5.11 (Windows 98; U) [en]"
         << "Mozilla/5.0 (Windows NT 6.4; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/36.0.1985.143 Safari/537.36 Edge/12.0"
         << "Mozilla/5.0 (X11; Linux x86_64; rv:2.0b4) Gecko/20100818 Firefox/4.0b4"
         << "Dredd/1.0.1 (Linux 3.11.10-29-desktop; x64)";
    addHeader("User-Agent", list);

    list << "text/plain"
         << "application/json";
    addHeader("Accept", list);

    list << "no-cache";
    addHeader("Cache-Control", list);

    list << "keep-alive"
         << "Upgrade";
    addHeader("Connection", list);
}

void ParamDlg::addHeader(const QString& header, QStringList& values)
{
    m_preHeaders.insert(header, values);
    m_cName->addItem(header);
    values.clear();
}
