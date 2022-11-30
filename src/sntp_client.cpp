#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "sntp_config.h"

#define IP "192.168.1.42" // cambiar cuando no se trabaje en local

int create_msg(struct ntp_msg *recv_msg);
int print_msg(struct ntp_msg *recv_msg);
int main(int argc, char* argv[]) {
  int sockfd;
  char buffer[BUF_SIZE] = "Hello from the client"; 
  struct sockaddr_in server_addr;
  struct ntp_msg recv_msg;
  socklen_t addr_size;
  int tos = IPTOS_LOWDELAY;



  // CONFIG
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket creation failed");
    return -1;
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(IP);
  server_addr.sin_port = htons(PORT);
  addr_size = sizeof(server_addr);
  setsockopt(sockfd, IPPROTO_IP, IP_TOS, &tos, sizeof(tos));


  create_msg(&recv_msg);

  // ENVIAR MENSAJE
  sendto(sockfd, &recv_msg, BUF_SIZE, 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));
  printf("[CLIENT]: Message sent\n");

  // RECIBIR MENSAJE
  recvfrom(sockfd, (char *)buffer, BUF_SIZE, 0, (struct sockaddr *)&server_addr, &addr_size);

  // PROCESAR MENSAJE
  memcpy(&recv_msg, buffer, sizeof(recv_msg));
  printf("[SERVER]:\n");
  print_msg(&recv_msg);

  close(sockfd);
  printf("Fin del programa\n");
  return 0;
}

int create_msg(struct ntp_msg *recv_msg) {
  recv_msg->status = 27; // li=0, mode=3(client) version=3 00 011 011 = 27 / recomended version = 1
  // Todo el resto 0
  recv_msg->stratum = 1;
  recv_msg->ppoll = 2;
  recv_msg->precision = 3;
  recv_msg->rootdelay.int_parts = 4; recv_msg->rootdelay.fractions = 1;
  recv_msg->dispersion.int_parts = 5; recv_msg->dispersion.fractions = 2;
  recv_msg->refid = 6;
  recv_msg->reftime.int_partl = 7; recv_msg->reftime.fractionl = 3;
  recv_msg->orgtime.int_partl = 8; recv_msg->orgtime.fractionl = 4;
  recv_msg->rectime.int_partl = 9; recv_msg->rectime.fractionl = 5;
  recv_msg->xmttime.int_partl = 10; recv_msg->xmttime.fractionl = 6;

  return 0;
}

int print_msg(struct ntp_msg *recv_msg) {
  printf("status: %d\n", recv_msg->status);
  printf("stratum: %d\n", recv_msg->stratum);
  printf("ppoll : %d\n", recv_msg->ppoll);
  printf("precision : %d\n", recv_msg->precision);
  printf("rootdelay : %d.%d\n", recv_msg->rootdelay.int_parts, recv_msg->rootdelay.fractions);
  printf("dispersion : %d.%d\n", recv_msg->dispersion.int_parts, recv_msg->dispersion.fractions);
  printf("refid : %d\n", recv_msg->refid);
  printf("reftime : %d.%d\n", recv_msg->reftime.int_partl, recv_msg->reftime.fractionl);
  printf("orgtime : %d.%d\n", recv_msg->orgtime.int_partl, recv_msg->orgtime.fractionl);
  printf("rectime: %d.%d\n", recv_msg->rectime.int_partl, recv_msg->rectime.fractionl);
  printf("xmttime: %d.%d\n", recv_msg->xmttime.int_partl, recv_msg->xmttime.fractionl);

  return 0;
}
