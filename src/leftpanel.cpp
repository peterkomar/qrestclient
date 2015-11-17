#include "leftpanel.h"
#include "restclientmainwindow.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QTextEdit>
#include <QDockWidget>
#include <QPushButton>

LeftPanel::LeftPanel(RestClientMainWindow* app)
{
    m_params             = new ParamsList(ParamDlg::MODE_REQUEST);
    m_headers            = new ParamsList(ParamDlg::MODE_HEADER);
    m_requestContentType = new QComboBox;
    m_rawContent         = new QTextEdit;

    m_rawContent->setAcceptRichText(false);

    QStringList types;
    types << "application/json" << "text/csv" << "text/plain" << "text/xml";
    m_requestContentType->addItems(types);
    m_requestContentType->setEditable(true);

    QWidget *rawTab              = new QWidget;

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(new QLabel(QObject::tr("Content Type:")));
    hLayout->addWidget(m_requestContentType);

    QVBoxLayout *rawLayout = new QVBoxLayout;
    rawLayout->addLayout(hLayout);
    rawLayout->addWidget(m_rawContent);
    rawTab->setLayout(rawLayout);

    m_tab = new QTabWidget;
    m_tab->addTab(buildParamsWidget(m_params), QObject::tr("Params"));
    m_tab->addTab(buildParamsWidget(m_headers), QObject::tr("Headers"));
    m_tab->addTab(rawTab, QObject::tr("Content to Send"));
    m_tab->setCurrentIndex(0);

    QDockWidget *dock = new QDockWidget(app);
    dock->setObjectName("Left");
    dock->setWidget(m_tab);
    dock->setFeatures(QDockWidget::DockWidgetMovable
                      | QDockWidget::DockWidgetFloatable
                      | QDockWidget::DockWidgetVerticalTitleBar);
    dock->setMinimumWidth(450);
    app->addDockWidget(Qt::LeftDockWidgetArea, dock);
}

QWidget* LeftPanel::buildParamsWidget(ParamsList *widget)
{
    QWidget *paramsWidget = new QWidget;
    QHBoxLayout *phLayout = new QHBoxLayout;

    QPushButton *addBtn = new QPushButton(QObject::tr("Add"));
    addBtn->setProperty("add", true);
    QObject::connect(addBtn, SIGNAL(clicked()), widget, SLOT(slotEditItem()));

    QPushButton *editBtn = new QPushButton(QObject::tr("Edit"));
    QObject::connect(editBtn, SIGNAL(clicked()), widget, SLOT(slotEditItem()));

    QPushButton *deleteBtn = new QPushButton(QObject::tr("Delete"));
    QObject::connect(deleteBtn, SIGNAL(clicked()), widget, SLOT(slotDeleteItem()));

    phLayout->addStretch(10);
    phLayout->addWidget(addBtn);
    phLayout->addWidget(editBtn);
    phLayout->addWidget(deleteBtn);

    QVBoxLayout *pvLayout = new QVBoxLayout;
    pvLayout->addLayout(phLayout);
    pvLayout->addWidget(widget);
    paramsWidget->setLayout(pvLayout);

    return paramsWidget;
}

