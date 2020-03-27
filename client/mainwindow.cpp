#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <iostream>

using namespace std;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {

  setupUi(this);

  // TODO 移动到 main 中间，让
  LoginDialog *loginDialog = new LoginDialog(this);
  QString name = QStringLiteral("huxueshi");
  loginDialog->setUsername(name); // optional
  connect(loginDialog, SIGNAL(acceptLogin(QString &, QString &, int &)), this,
          SLOT(slotAcceptUserLogin(QString &, QString &)));

  // 此处是 阻碍 MainWindow 的出现
  loginDialog->exec();
}

MainWindow::~MainWindow() {}

/**
 * 1. 一共两个Window，全部持有 client *
 *    1. 发送消息给网络
 * 2. client 持有 window 和 loginWindow : 返回消息
 *    1. LoginWindow <-- LoginOk
 *    2. MainWindow <--- Dir
 *    3. 
 */
