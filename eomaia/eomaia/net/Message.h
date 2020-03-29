#ifndef MESSAGE_H_POSYNQI3
#define MESSAGE_H_POSYNQI3
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <string>
#include <iostream>
#include <filesystem>

namespace eomaia {
namespace net {

// 对于 Client : wirte : 将 Message 放到队列中间，发送之后释放空间。
// read 调用解析器，然后分发函数
//
//
// 对于 Server : 调用解析器，然后立刻写回。

class Message {
  char *data;
  uint32_t length;
  char *free_base;

  void alloc(uint32_t size) {
    data = (char *)malloc(size);
    free_base = data;
    length = size;
    memset(data, 0, size);
  }

public:
  Message() : data(nullptr), length(0), free_base(nullptr) {}
  Message(char *d, uint32_t l) : data(d), length(l), free_base(data) {}

  void free() { ::free(free_base); }

  char *getData() { return data; }

  uint32_t getLength() { return length; }

  // malloc
  void clientLogin(const char *name, const char *password) {
    alloc(sizeof(char) * 100);
    // 没有必要固定长度 ！
    sprintf(data, "%-3d %s %s", C_LOGIN, name, password);
  }

  void clientDir() {
    alloc(sizeof(char) * 100);
    sprintf(data, "%-3d", C_DIR);
  }

  void clientDownload(std::string filename) {
    alloc(sizeof(char) * 100);
    // length 重新赋值只会导致 client 出现错误
    sprintf(data, "%-3d %s", C_DOWNLOAD, filename.c_str());
  }

  // 前面三个字节 : 利用 scanf HHHHH
  // 只有携带文件的时候 : 使用文件名
  enum CMD {
    C_LOGIN,
    C_DIR,
    C_UPLOAD,
    C_DOWNLOAD,
    S_DIR, // 从正文开始，一百个字节一个。
    S_LOGIN_OK,
    S_LOGIN_FAILED,
    S_DOWNLOAD,
    S_UPLOAD, // 总是成功
  };

  enum CMD getCmd() {
    enum CMD cmd;
    sscanf(data, "%u", &cmd);
    data += 3;
    return cmd;
  };

  void serverLogin(char *name, char *password) {
    sscanf(data, "%s %s", name, password);
  }

  void LoginOk() {
    alloc(sizeof(char) * 100);
    sprintf(data, "%-3d", S_LOGIN_OK);
  }

  void LoginFailed() {
    alloc(sizeof(char) * 100);
    length = sprintf(data, "%-3d", S_LOGIN_FAILED);
  }

  void fileMsg(enum CMD cmd, std::string base, std::string filename) {
    std::ifstream file(base + filename, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    alloc(sizeof(char) * 100 + size);
    sprintf(data, "%-3d %s", cmd, filename.c_str());
    file.read(data + 100, size);
    file.close();
    printf("file content : [%s]", data + 100);
  }

  void permissions(char *output, std::string base, std::string filename) {
    std::string path = base + filename;
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();

    struct stat st;
    char *modeval = (char *)malloc(sizeof(char) * 9 + 1);
    if (stat(path.c_str(), &st) == 0) {
      mode_t perm = st.st_mode;
      modeval[0] = (perm & S_IRUSR) ? 'r' : '-';
      modeval[1] = (perm & S_IWUSR) ? 'w' : '-';
      modeval[2] = (perm & S_IXUSR) ? 'x' : '-';
      modeval[3] = (perm & S_IRGRP) ? 'r' : '-';
      modeval[4] = (perm & S_IWGRP) ? 'w' : '-';
      modeval[5] = (perm & S_IXGRP) ? 'x' : '-';
      modeval[6] = (perm & S_IROTH) ? 'r' : '-';
      modeval[7] = (perm & S_IWOTH) ? 'w' : '-';
      modeval[8] = (perm & S_IXOTH) ? 'x' : '-';
      modeval[9] = '\0';

      sprintf(output, "%-20s|%-15s|%-10ld", filename.c_str(), modeval, size);
      ::free(modeval);
    } else {
      std::cerr << "server stat failed " << std::endl;
    }
  }

  void server_upload(enum CMD cmd, std::string base, std::string filename) {
    alloc(sizeof(char) * 100 + 3);
    sprintf(data, "%-3d", cmd);
    permissions(data + 3, base, filename);
  }

  void server_dir(enum CMD cmd, std::string path){
    int size = 0;
    for (const auto & entry : std::filesystem::directory_iterator(path)){
      size ++;
    }
    alloc(sizeof(char) * 100 * size + 3);
    sprintf(data, "%-3d", cmd);
    char * ptr = data + 3;
    for (const auto & entry : std::filesystem::directory_iterator(path)){
      permissions(ptr, path, entry.path().filename());
      printf("%s", ptr);
      ptr += 100;
    }
  }
};
} // namespace net
} // namespace eomaia

#endif /* end of include guard: MESSAGE_H_POSYNQI3 */
