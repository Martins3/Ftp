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
    QString  f = fileName->text();
    qDebug() << "main window upload\n" << f;
    netManager->upload(f.toStdString());
  }

  void download() {
    QString  f = fileName->text();
    qDebug() << "main window download\n" << f;
    netManager->download(f.toStdString());
  }

  void dir() {
    qDebug() << "main window dir\n" << fileName->text();
    netManager->getDir();
  }

private:
  /* Ui::MainWindow *ui; */
  FtpClient *netManager;
  int counter;

  QDialogButtonBox *buttons;
  QLineEdit *fileName;
  QLabel *labelPassword;
  QListWidget *listWidget;

public:
  QWidget *centralwidget;
  QMenuBar *menubar;
  QStatusBar *statusbar;

  // 并不需要对应的 get 方法
  void setNetManager(FtpClient *ftp) { netManager = ftp; }

  void setupUI(QMainWindow *);

  void message(std::string msg) { setWindowTitle(QString::fromStdString(msg)); }

  void clearTable() { listWidget->clear(); }

  void addOneFile(const char *f) {
    QString text(f);
    new QListWidgetItem(text, listWidget);
  }

};
#endif // MAINWINDOW_H
