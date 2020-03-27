#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <iostream>

using namespace std;
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) { setupUI(this); }

MainWindow::~MainWindow() {}

void MainWindow::setupUI(QMainWindow *MainWindow) {
  if (MainWindow->objectName().isEmpty())
    MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
  MainWindow->resize(800, 600);
  centralwidget = new QWidget(MainWindow);
  centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
  MainWindow->setCentralWidget(centralwidget);
  menubar = new QMenuBar(MainWindow);
  menubar->setObjectName(QString::fromUtf8("menubar"));
  MainWindow->setMenuBar(menubar);
  statusbar = new QStatusBar(MainWindow);
  statusbar->setObjectName(QString::fromUtf8("statusbar"));
  MainWindow->setStatusBar(statusbar);

  MainWindow->setWindowTitle(
      QCoreApplication::translate("MainWindow", "ftpClient", nullptr));
  QMetaObject::connectSlotsByName(MainWindow);

  QGridLayout *formGridLayout = new QGridLayout(this);

  buttons = new QDialogButtonBox(this);
  buttons->addButton(QDialogButtonBox::Ok);
  buttons->addButton(QDialogButtonBox::Cancel);
  buttons->addButton(QDialogButtonBox::Abort);
  buttons->button(QDialogButtonBox::Abort)->setText(tr("dir"));
  buttons->button(QDialogButtonBox::Ok)->setText(tr("upload"));
  buttons->button(QDialogButtonBox::Cancel)->setText(tr("download"));

  // connects slots
  connect(buttons->button(QDialogButtonBox::Abort), SIGNAL(clicked()), this,
          SLOT(dir()));

  connect(buttons->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this,
          SLOT(upload()));

  connect(buttons->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this,
          SLOT(download()));

  fileName = new QLineEdit(this);
  labelPassword = new QLabel(this);
  labelPassword->setText(tr("FileName"));
  labelPassword->setBuddy(fileName);

  const int numRows = 1;
  const int numColumns = 3;

  /* QStandardItemModel *model = new QStandardItemModel(numRows, numColumns);
   */

  /* QStandardItem* nameItem = new QStandardItem(QString("Name")); */
  /* QStandardItem* privItem = new QStandardItem(QString("Privilege")); */
  /* QStandardItem* sizeItem = new QStandardItem(QString("Size")); */
  /*  model->setItem(0, 0, nameItem); */
  /*  model->setItem(0, 1, privItem); */
  /*  model->setItem(0, 2, sizeItem); */

  /*  model->setItem(1, 0, nameItem); */
  /*  model->setItem(1, 1, privItem); */
  /*  model->setItem(1, 2, sizeItem); */

  /*  QTableView* view = new QTableView; */
  /*  view->setModel(model); */

  listWidget = new QListWidget(this);
  new QListWidgetItem(tr("Oak"), listWidget);
  new QListWidgetItem(tr("Fir"), listWidget);
  new QListWidgetItem(tr("Pine"), listWidget);

  // place components into the dialog
  formGridLayout->addWidget(labelPassword, 0, 0, 1, 1);
  formGridLayout->addWidget(fileName, 0, 1, 1, 7);

  /* formGridLayout->addWidget(view, 1, 0, 18, 8); */
  formGridLayout->addWidget(listWidget, 1, 0, 18, 8);

  formGridLayout->addWidget(buttons, 19, 0, 1, 6);

  centralwidget->setLayout(formGridLayout);
  /* setLayout(formGridLayout); */
} // setupUi
