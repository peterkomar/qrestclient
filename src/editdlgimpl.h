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
#ifndef EDITDLGIMPL_H
#define EDITDLGIMPL_H

#include <QDialog>

class QLineEdit;
class QComboBox;

class EditDlgImpl : public QDialog
{
    Q_OBJECT
public:
    explicit EditDlgImpl(bool headerMode, QWidget *parent = 0);
    QString getName() const;
    void setName(const QString& name);

    QString getValue() const;
    void setValue(const QString& value);

private:
    QLineEdit *m_name;
    QLineEdit *m_value;
    QComboBox *m_cName;
    QComboBox *m_cValue;

    bool b_mode;
};

#endif // EDITDLGIMPL_H
