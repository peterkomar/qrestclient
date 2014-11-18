#ifndef QCSVVIEW_H
#define QCSVVIEW_H

#include <QWidget>

class QTreeView;

class QCsvView : public QWidget
{
  Q_OBJECT
public:
  explicit QCsvView(QWidget *parent = 0);
  void setText(const QString& text);

private:

  void renderCsv();

  QString m_Delimeter;
  QString m_Enclosure;
  QString m_Text;
  bool m_firstRowHeader;

  QTreeView* m_csvBody;

signals:

public slots:
  void slotChangeDelimeter(QString text);
  void slotChangeEnclosure(QString text);
  void slotFirstRowHeader(bool checked);

};

#endif // QCSVVIEW_H
