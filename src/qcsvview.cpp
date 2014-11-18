#include "qcsvview.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QHeaderView>
#include <QTreeView>

#include <QDebug>

QCsvView::QCsvView(QWidget *parent) :
  QWidget(parent)
{

  m_Delimeter = ";";
  m_Enclosure = "\"";
  m_Text = "";
  m_firstRowHeader = true;

  QFormLayout *form = new QFormLayout();
  QLineEdit *txtDelimeter = new QLineEdit(m_Delimeter);
  QLineEdit *txtEnclosure = new QLineEdit(m_Enclosure);

  QCheckBox *chkFirstRowHeader = new QCheckBox("First row is header");
  chkFirstRowHeader->setChecked(true);

  form->addRow("Delimeter:", txtDelimeter);
  form->addRow("Enclosure:", txtEnclosure);
  form->addRow("", chkFirstRowHeader);

  m_csvBody = new QTreeView;

  QVBoxLayout *lyt = new QVBoxLayout(this);
  lyt->addLayout(form);
  lyt->addWidget(m_csvBody);

  connect(txtDelimeter,SIGNAL(textChanged(QString)),this,SLOT(slotChangeDelimeter(QString)));
  connect(txtEnclosure,SIGNAL(textChanged(QString)),this,SLOT(slotChangeEnclosure(QString)));
  connect(chkFirstRowHeader,SIGNAL(toggled(bool)),this,SLOT(slotFirstRowHeader(bool)));
}

void QCsvView::setText(const QString &text)
{
  m_Text = text;
  renderCsv();
}

void QCsvView::renderCsv()
{
  QRegExp regRow("\n|\r|\n\r|\r\n");
  QRegExp regColumn(m_Delimeter + "|" + m_Delimeter + m_Enclosure+"|" + m_Enclosure + m_Delimeter + "|" + m_Enclosure + m_Delimeter + m_Enclosure);

  QStringList rows = m_Text.split(regRow);

  QStringList headerColumns = rows.first().split(regColumn);
  int countColumns = headerColumns.size();

  QStandardItemModel *model = new QStandardItemModel(0, countColumns, this);

  if(m_firstRowHeader) {

    rows.removeFirst();
    for(int i = 0; i < countColumns; i++) {
      model->setHeaderData(i, Qt::Horizontal, headerColumns.at(i).toLocal8Bit().constData());
    }

  } else {
    for(int i = 0; i < countColumns; i++) {
      model->setHeaderData(i, Qt::Horizontal, QString::number(i));
    }
  }

  for(int i = 0; i < rows.size(); i++) {

    QStringList columns = rows.at(i).split(regColumn);

    for(int y = 0; y < columns.size(); y++) {

      QStandardItem* item = new QStandardItem(columns.at(y).toLocal8Bit().constData());
      model->setItem(i,y,item);
    }
  }

  m_csvBody->setModel(model);
  m_csvBody->setAlternatingRowColors(true);

  for(int i = 0; i < countColumns; i++) {
    m_csvBody->header()->setSectionResizeMode(i,QHeaderView::Stretch);
  }
}
//================ SLOTS ======
void QCsvView::slotChangeDelimeter(QString text)
{
  m_Delimeter = text;
  renderCsv();
}
void QCsvView::slotChangeEnclosure(QString text)
{
  m_Enclosure = text;
  renderCsv();
}
void QCsvView::slotFirstRowHeader(bool checked)
{
  m_firstRowHeader = checked;
  renderCsv();
}
