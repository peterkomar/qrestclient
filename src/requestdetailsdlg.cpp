#include "requestdetailsdlg.h"

#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QGridLayout>
#include <QClipboard>
#include <QApplication>

RequestDetailsDlg::RequestDetailsDlg(QWidget *parent) : QDialog(parent)
{
    QGridLayout *gridLayout = new QGridLayout(this);

    m_view = new QPlainTextEdit(this);
    m_view->setFont(QFont("Monospace"));
    gridLayout->addWidget(m_view, 0, 0, 1, 1);

    resize(800, 500);
}

void RequestDetailsDlg::setRequest(const QString &text)
{
    m_view->setPlainText(text);
}

void RequestDetailsDlg::slotSendToBuffer()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(m_view->toPlainText());
}

