#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "FtpClient.h"
#include "LoginDialog.h"
#include <QAbstractItemView>
#include <QMainWindow>
#include <QStringList>
#include <QStringListModel>
#include <QtCore/QVariant>
#include <QtGui>
#include <QtWidgets/QApplication>
#include <QtWidgets/QListWidgetItem>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>
#include <net/SSL.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

public slots:
  void slotAcceptUserLogin(QString &a, QString &b) {
    qDebug() << a << " : " << b << "\n";
  }

  void upload() {
    qDebug() << "main window upload\n" << fileName->text();
    listWidget->clear();
  }

  void download() {
    qDebug() << "main window download\n" << fileName->text();
    QString text = QString("add ") + QString::number(counter++);
    new QListWidgetItem(text, listWidget);
  }

  void dir() { 
    qDebug() << "main window dir\n" << fileName->text();
  }

private:
  /* Ui::MainWindow *ui; */
  /* FtpClient client; */
  int counter;

  QDialogButtonBox *buttons;
  QLineEdit *fileName;
  QLabel *labelPassword;
  QListWidget *listWidget;

public:
  QWidget *centralwidget;
  QMenuBar *menubar;
  QStatusBar *statusbar;

  void setupUi(QMainWindow *MainWindow) {
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

    retranslateUi(MainWindow);
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

  void retranslateUi(QMainWindow *MainWindow) {
    MainWindow->setWindowTitle(
        QCoreApplication::translate("MainWindow", "ftpClient", nullptr));
  } // retranslateUi
};
#endif // MAINWINDOW_H
