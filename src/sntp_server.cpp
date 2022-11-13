#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "sntp_config.h"


int create_res(struct ntp_msg *recv_msg);
int print_msg(struct ntp_msg *recv_msg);

int main(int argc, char* argv[]) {
  int sockfd;
  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_size;
  char buffer[BUF_SIZE];
  int tos = IPTOS_LOWDELAY;
  ssize_t size;
  struct ntp_msg recv_msg;

  // CONFIG
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(IP);
  server_addr.sin_port = htons( PORT );
  addr_size = sizeof(client_addr);

  setsockopt(sockfd, IPPROTO_IP, IP_TOS, &tos, sizeof(tos));

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    fprintf(stderr, "[SERVER-ERROR]: socket create failed. %d: %s\n", errno, strerror(errno));
    return -1;
  }

  if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
    fprintf(stderr, "[SERVER-ERROR]: socket bind failed. %d: %s\n", errno, strerror(errno));
    return -1;
  }

  while (true) {
    printf("Esperando mensajes...\n");
    memset(&buffer, 0, BUF_SIZE);

    // RECIBIR MENSAJE
    size = recvfrom(sockfd, &buffer, BUF_SIZE, 0, (struct sockaddr *)&client_addr, &addr_size);
    printf("[SERVER]: bytes received: %ld\n", size);

    // PROCESAR MENSAJE
    memcpy(&recv_msg, buffer, sizeof(recv_msg));
    create_res(&recv_msg);
    print_msg(&recv_msg);
    

    // ENVIAR REPUESTA
    sendto(sockfd, &recv_msg, BUF_SIZE, 0, (const struct sockaddr *)&client_addr, addr_size);
  }

  close(sockfd);
  printf("Fin del programa\n");
  return 0;
}


int create_res(struct ntp_msg *recv_msg) {
/**
 * 
 *  PETICION
 *  - ignora todo menos el primer octeto
 *  - LI ignorar
 *  - Version 1,2,3 sino descartar
 *  - 
 *
 *  RESPUESTA
 *  - para conecciones primarias LI = 0, Stratum = 1 || LI = 3, Stratum = 0 
 *  - Version Number and Poll are copied
 *  - Mode = 4 || de lo contrario Mode = 2 (symetric pasive)
 *  - if primary server -> root delay and root dispersion = 0
 */
  printf("Creando respuesta...\n");
  return 0;
}

int print_msg(struct ntp_msg *recv_msg) {
  printf("status: %d\n", recv_msg->status);
  printf("stratum: %d\n", recv_msg->stratum);
  printf("ppoll : %d\n", recv_msg->ppoll);
  printf("precision : %d\n", recv_msg->precision);
  printf("rootdelay : %d\n", recv_msg->rootdelay.int_parts);
  printf("dispersion : %d\n", recv_msg->dispersion.int_parts);
  printf("refid : %d\n", recv_msg->refid);
  printf("reftime : %d\n", recv_msg->reftime.int_partl);
  printf("orgtime : %d\n", recv_msg->orgtime.int_partl);
  printf("rectime: %d\n", recv_msg->rectime.int_partl);
  printf("xmttime: %d\n", recv_msg->xmttime.int_partl);

  return 0;
}
