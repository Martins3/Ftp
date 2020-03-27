#include "FtpClient.h"
#include "LoginDialog.h"
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

#ifdef CONFIG_SSL
  SSLOperation::initClientCTX();
#endif

  MainWindow *w = new MainWindow();
  LoginDialog *loginDialog = new LoginDialog(w);
  FtpClient *client = new FtpClient(loginDialog, w);

  w->setNetManager(client);
  loginDialog->setNetManager(client);

  QString name = QStringLiteral("huxueshi");
  QString pass = QStringLiteral("pass1234");
  loginDialog->setUsername(name); // optional
  loginDialog->setPassword(pass);

  /* w->connect(loginDialog, SIGNAL(acceptLogin(QString &, QString &, int &)), w, */
  /*            SLOT(slotAcceptUserLogin(QString &, QString &))); */

  loginDialog->exec(); // 当LoginWindow 被关闭的时候，其被关闭!

  w->show();
  return a.exec();
}
