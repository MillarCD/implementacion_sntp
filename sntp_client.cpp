#include <stdio.h>
#include <string.h>
#include <errno.h>

// socket
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <netdb.h>
#include <unistd.h>

#define PORT 3003
#define IP "127.0.0.1"
#define BUF_SIZE 384

int main(int argc, char* argv[]) {

  int sockfd;
  char buffer[BUF_SIZE] = "Hello from the client"; 
  struct sockaddr_in server_addr;
  socklen_t addr_size;

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket creation failed");
    return 0;
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(IP);
  server_addr.sin_port = htons(PORT);
  addr_size = sizeof(server_addr);

  sendto(sockfd, (const char *)buffer, BUF_SIZE, 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));
  printf("Message sent\n");

  recvfrom(sockfd, (char *)buffer, BUF_SIZE, 0, (struct sockaddr *)&server_addr, &addr_size);

  printf("[SERVER]: %s\n", buffer);

  close(sockfd);
  return 0;
}
