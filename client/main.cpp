#include "LoginDialog.h"
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  // TODO 如何处理此处的 CONFIG_SSL
  // TODO 将 client 放到此处呀 !
  SSLOperation::initClientCTX();


  MainWindow w;
  w.show();

  return a.exec();
}
