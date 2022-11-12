#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "sntp.h"

#define IP "127.0.0.1"

int main(int argc, char* argv[]) {
  int sockfd;
  char buffer[BUF_SIZE] = "Hello from the client"; 
  struct sockaddr_in server_addr;
  socklen_t addr_size;

  int tos = IPTOS_LOWDELAY;

  struct ntp_msg recv_msg;

  recv_msg.status = 27; // li=0, mode=3(client) version=3 00 011 011 = 27
  recv_msg.stratum = 0;
  recv_msg.ppoll = 0;
  recv_msg.precision = 0;
  recv_msg.rootdelay.int_parts = 0; recv_msg.rootdelay.fractions = 0;
  recv_msg.dispersion.int_parts = 0; recv_msg.dispersion.fractions = 0;
  recv_msg.refid = 0;
  recv_msg.reftime.int_partl = 0; recv_msg.reftime.fractionl = 0;
  recv_msg.orgtime.int_partl = 0; recv_msg.orgtime.fractionl = 0;
  recv_msg.rectime.int_partl = 0; recv_msg.rectime.fractionl = 0;
  recv_msg.xmttime.int_partl = 0; recv_msg.xmttime.fractionl = 0;

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket creation failed");
    return 0;
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(IP);
  server_addr.sin_port = htons(PORT);
  addr_size = sizeof(server_addr);

  setsockopt(sockfd, IPPROTO_IP, IP_TOS, &tos, sizeof(tos)); // revisar

  sendto(sockfd, &recv_msg, BUF_SIZE, 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));
  printf("Message sent\n");

  recvfrom(sockfd, (char *)buffer, BUF_SIZE, 0, (struct sockaddr *)&server_addr, &addr_size);

  memcpy(&recv_msg, buffer, sizeof(recv_msg));

  printf("[SERVER]: %d\n", recv_msg.status);

  close(sockfd);
  return 0;
}
