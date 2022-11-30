#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>

#include "sntp_config.h"

#define IP "0.0.0.0"

int create_res(struct ntp_msg *query, struct ntp_msg *reply);
int print_msg(struct ntp_msg *recv_msg);

int main(int argc, char* argv[]) {
  int sockfd;
  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_size;
  char buffer[BUF_SIZE];
  int tos = IPTOS_LOWDELAY;
  ssize_t size;
  struct ntp_msg query, reply;

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
    printf("[SERVER]: Esperando mensajes...\n");
    memset(&buffer, 0, BUF_SIZE);
    memset(&query, 0, BUF_SIZE);

    // RECIBIR MENSAJE
    size = recvfrom(sockfd, &buffer, BUF_SIZE, 0, (struct sockaddr *)&client_addr, &addr_size);
    printf("[SERVER]: bytes received: %ld\n", size);

    // PROCESAR MENSAJE

    memcpy(&query, buffer, sizeof(query));

    printf("[CLIENT]:\n");
    print_msg(&query);
    printf("\n\n");
    if (create_res(&query, &reply) == -1) continue;
    

    // ENVIAR REPUESTA
    sendto(sockfd, &reply, BUF_SIZE, 0, (const struct sockaddr *)&client_addr, addr_size);
    
  }

  close(sockfd);
  printf("Fin del programa\n");
  return 0;
}


int create_res(struct ntp_msg *msg, struct ntp_msg *reply) {
  struct timeval time_now;
  u_int32_t  seconds, milisecond;

  // se descarta el mensaje
  int version_number = (msg->status & VERSION_MASK);
  if (version_number<1 && version_number>4) return -1;

  // se borra el contenido de la variable reply
  memset(reply, 0, BUF_SIZE);  

  // fija el LI = 0 version = msg version y modo = 4
  reply->status = (msg->status & (7<<3)) + 4;

  reply->stratum = 1;
  reply->ppoll = msg->ppoll;
  reply->precision = 0;
  reply->rootdelay.int_parts = 0; reply->rootdelay.fractions = 0;
  reply->dispersion.int_parts = 0; reply->dispersion.fractions = 0;
  reply->refid = 0; // ip del shoa????

  reply->orgtime.int_partl = msg->xmttime.int_partl;
  reply->orgtime.fractionl = msg->xmttime.fractionl;

  if (gettimeofday(&time_now, NULL) == -1) return -1;

  seconds = time_now.tv_sec + JAN_1970;
  // SERVER: htonl((u_int32_t)((d - (u_int32_t)d) * UINT_MAX))
  // (d - (u_int32_t)d) = 1.0e-6 * time_now.tv_usec
  // milisecond = htonl((u_int32_t)((d - (u_int32_t)d) * UINT_MAX))
  // CLIENT: ((double)lfp.fractionl / UINT_MAX)
  milisecond = time_now.tv_usec / 1000; 

  reply->reftime.int_partl = seconds; reply->reftime.fractionl = milisecond;
  reply->rectime.int_partl = seconds; reply->rectime.fractionl = milisecond;
  reply->xmttime.int_partl = seconds; reply->xmttime.fractionl = milisecond;

  printf("[SERVER]: Se creo la respuesta...\n");
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
