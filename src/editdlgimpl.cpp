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
#include "editdlgimpl.h"

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>

EditDlgImpl::EditDlgImpl(bool mode, QWidget *parent) :
    QDialog(parent)
  ,b_mode(mode)
{
    QGridLayout *gridLayout;
        QLabel *label;
        QLineEdit *m_name;
        QPushButton *okBtn;
        QLabel *label_2;
        QLineEdit *m_value;
        QPushButton *cancelBtn;


        EditDlg->resize(366, 80);
                gridLayout = new QGridLayout(EditDlg);
                gridLayout->setObjectName(QStringLiteral("gridLayout"));
                label = new QLabel(EditDlg);
                label->setObjectName(QStringLiteral("label"));

                gridLayout->addWidget(label, 0, 0, 1, 1);

                m_name = new QLineEdit(EditDlg);
                m_name->setObjectName(QStringLiteral("m_name"));

                gridLayout->addWidget(m_name, 0, 1, 1, 1);

                okBtn = new QPushButton(EditDlg);
                okBtn->setObjectName(QStringLiteral("okBtn"));

                gridLayout->addWidget(okBtn, 0, 2, 1, 1);

                label_2 = new QLabel(EditDlg);
                label_2->setObjectName(QStringLiteral("label_2"));

                gridLayout->addWidget(label_2, 1, 0, 1, 1);

                m_value = new QLineEdit(EditDlg);
                m_value->setObjectName(QStringLiteral("m_value"));

                gridLayout->addWidget(m_value, 1, 1, 1, 1);

                cancelBtn = new QPushButton(EditDlg);
                cancelBtn->setObjectName(QStringLiteral("cancelBtn"));

                gridLayout->addWidget(cancelBtn, 1, 2, 1, 1);



                QObject::connect(cancelBtn, SIGNAL(clicked()), EditDlg, SLOT(reject()));
                QObject::connect(okBtn, SIGNAL(clicked()), EditDlg, SLOT(accept()));

                EditDlg->setWindowTitle(QApplication::translate("EditDlg", "Edit", 0));
                        label->setText(QApplication::translate("EditDlg", "Name:", 0));
                        okBtn->setText(QApplication::translate("EditDlg", "OK", 0));
                        label_2->setText(QApplication::translate("EditDlg", "Value:", 0));
                        cancelBtn->setText(QApplication::translate("EditDlg", "Cancel", 0));

}

void EditDlgImpl::setName(const QString &name)
{
    m_name->setText(name);
}

QString EditDlgImpl::getName() const
{
    return m_name->text();
}

void EditDlgImpl::setValue(const QString &value)
{
    m_value->setText(value);
}

QString EditDlgImpl::getValue() const
{
    return m_value->text();
}
