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

static void do_something(int connfd) {
  char rbuf[64] = {};
  ssize_t n = read(connfd, rbuf, sizeof(rbuf) - 1);
  if (n < 0) {
    std::cerr << "read() error\n";
    return;
  }
  std::cout << "client says: " << rbuf << "\n";

  char wbuf[] = "world";
  write(connfd, wbuf, strlen(wbuf));
}

int main() {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    die("socket()");
  }

  int val = 1;
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0) {
    die("setsockopt()");
  }

  struct sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(1234);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(fd, (const struct sockaddr *)&addr, sizeof(addr)) < 0) {
    die("bind()");
  }

  if (listen(fd, SOMAXCONN) < 0) {
    die("listen()");
  }

  while (true) {
    struct sockaddr_in client_addr = {};
    socklen_t addrlen = sizeof(client_addr);
    int connfd = accept(fd, (struct sockaddr *)&client_addr, &addrlen);
    if (connfd < 0) {
      continue; // error
    }
    do_something(connfd);
    close(connfd);
  }

  close(fd);
  return 0;
}
