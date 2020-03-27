#include "LoginDialog.h"
#include "mainwindow.h"
#include <TcpClient.h>
#include <base/LogInterface.h>
#include <cstring>
#include <iostream>
#include <net/IOEventLoop.h>
#include <net/Socket.h>
#include <net/TcpServer.h>
#include <string>
#include <sys/sendfile.h>
#include <thread>

using namespace eomaia::net;

void TcpClient::readEvent() {
  // 应该是只有存在内容触发 epoll 的 !
  printf("readEvent trigger\n");

  int error = 0;
  /* int n = readBuf.readFromIO(event->getFd(), error); */
  int n = readBuf.readFromIO(socket.get(), error);
  if (n > 0) {
    auto msg = readBuf.readAllAsMessage();
    auto cmd = msg.getCmd();

    if (cmd == Message::S_LOGIN_OK) {
      printf("Server tell us : Login Ok\n");
      login->close();
    } else if (cmd == Message::S_LOGIN_FAILED) {
      printf("Server tell us : Login Failed\n");
      login->message("Login failed");
    } else if (cmd == Message::S_DOWNLOAD) {
      // FIXME : merge the code !
      const std::string base = "/home/shen/Core/ftp/eomaia/www/";
      char filename[50];
      sscanf(msg.getData(), "%s", filename);
      auto file = std::fstream(
          base + filename, std::ios::out | std::ios::binary | std::ios::trunc);
      std::cout << "try open file " << base + filename << std::endl;
      if (file.is_open()) {
        printf("write [%s]\n", msg.getData() - 3 + 100);
        file.write(msg.getData() - 3 + 100, msg.getLength() - 100);
        file.close();

        // TODO 添加文件到列表
        // 1.
        // 2. window show something !
      } else {
        std::cout << "client receive file failed !\n";
        // 2. window show something !
      }
    } else if (cmd == Message::S_DIR) {
      // Server 端 sprintf 进去
      char filename[100];
      int len = 3;
      window->clearTable();
      while (len < msg.getLength()) {
        len += 100;
        sscanf(msg.getData(), "%s", filename);
        memset(filename, 0, sizeof(filename));
      }
      window->addOneFile(filename);
    } else if (cmd == Message::S_UPLOAD) {
      char filename[100];
      sscanf(msg.getData(), "%s", filename);
      window->addOneFile(filename);
    } else {
      printf("Client : Receive unrecognized Message !\n");
    }

    /* std::string data; */
    /* readBuf.readAllAsString(data); // TODO 负责解析数据，然后告知Qt
     * 系统的插件 */
    /* std::cout << data << std::endl; */
    /* if (!waiting_for_file) { */
    /*   std::unique_lock<std::mutex> lock(mutex); */
    /*   if (data == "LOGIN_OK") { */
    /*     response = LOGIN_OK; */
    /*     std::cout << "I am notify_one with Login in OK" << std::endl; */
    /*     ; */
    /*   } else if (data == "LOGIN_FAILED") { */
    /*     response = LOGIN_FAILED; */
    /*   } else if (data == "UNPRIVILEGED_OP") { */
    /*     // TODO */
    /*   } */
    /*   condtion.notify_one(); */
    /* } else { */
    /*   // TODO buffer into file */
    /*   std::unique_lock<std::mutex> lock(mutex); */
    /*   response = DOWNLOAD_OK; */
    /*   condtion.notify_one(); */
    /* } */

  } else if (n == 0) {
    closeEvent();
  } else {
    base::Log::OUT(base::Log::Error)
        << "TcpConnection::handleRead error :" << std::to_string(error);
    closeEvent();
  }
}
