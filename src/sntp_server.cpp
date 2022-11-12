#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "sntp.h"

#define IP "127.0.0.1"

int main(int argc, char* argv[]) {
  int sockfd;
  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_size;

  char buffer[BUF_SIZE];
  int tos = IPTOS_LOWDELAY;

  ssize_t size;

  struct ntp_msg recv_msg;

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(IP);
  server_addr.sin_port = htons( PORT );

  setsockopt(sockfd, IPPROTO_IP, IP_TOS, &tos, sizeof(tos)); // revisar

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    fprintf(stderr, "[SERVER-ERROR]: socket create failed. %d: %s\n", errno, strerror(errno));
    return 0;
  }

  if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
    fprintf(stderr, "[SERVER-ERROR]: socket bind failed. %d: %s\n", errno, strerror(errno));
    return 0;
  }

  addr_size = sizeof(client_addr);
  while (true) {
    printf("Esperando mensajes...\n");
    memset(&buffer, 0, BUF_SIZE);
    size = recvfrom(sockfd, &buffer, BUF_SIZE, 0, (struct sockaddr *)&client_addr, &addr_size);
    printf("[SERVER]: bytes received: %ld\n", size);

    memcpy(&recv_msg, buffer, sizeof(recv_msg));
    printf("[CLIENT]: %d\n", recv_msg.status);
    // <- procesar mensaje
    // <- editar mensaje
    
    sendto(sockfd, (const char *)buffer, strlen(buffer), 0, (const struct sockaddr *)&client_addr, addr_size);
  }

  close(sockfd);
  printf("fin del programa\n");
  return 0;
}
