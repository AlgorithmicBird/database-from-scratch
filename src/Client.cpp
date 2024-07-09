#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void die(const char *message) {
  perror(message);
  exit(EXIT_FAILURE);
}

int main() {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    die("socket()");
  }

  struct sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(1234);
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

  if (connect(fd, (const struct sockaddr *)&addr, sizeof(addr)) < 0) {
    die("connect()");
  }

  char msg[] = "hello";
  write(fd, msg, strlen(msg));

  char rbuf[64] = {};
  ssize_t n = read(fd, rbuf, sizeof(rbuf) - 1);
  if (n < 0) {
    die("read()");
  }
  std::cout << "server says: " << rbuf << "\n";

  close(fd);
  return 0;
}
