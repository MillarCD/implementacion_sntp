#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <climits>

#include "sntp_config.h"

#define IP "0.0.0.0"

int create_res(struct ntp_msg *msg, struct ntp_msg *reply, u_int32_t ref_sec, u_int32_t ref_milsec);
int print_msg(struct ntp_msg *recv_msg);
u_int32_t swap_bytes(u_int32_t num) {
  return ((num & 255) << 24) + ((num & (255<<8)) << 8) + ((num & (255<<16)) >> 8) + ((num & (255<<24)) >> 24);
}

int main(int argc, char* argv[]) {
  int sockfd;
  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_size;
  char buffer[BUF_SIZE];
  int tos = IPTOS_LOWDELAY;
  ssize_t size;
  struct ntp_msg query, reply;

  // Crear timepo de referencia
  struct timeval reftime;
  if (gettimeofday(&reftime, NULL) == -1) return -1;
  uint32_t ref_sec, ref_milsec;
  ref_sec = swap_bytes(reftime.tv_sec + JAN_1970);
  ref_milsec = swap_bytes(htonl((uint32_t)((1.0e-6 * reftime.tv_usec) * UINT_MAX)));

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
  printf("Server is running at PORT %d\n", PORT);
  while (true) {
    // printf("[SERVER]: Esperando mensajes...\n");
    memset(&buffer, 0, BUF_SIZE);
    memset(&query, 0, BUF_SIZE);

    // RECIBIR MENSAJE
    size = recvfrom(sockfd, &buffer, BUF_SIZE, 0, (struct sockaddr *)&client_addr, &addr_size);
    printf("[SERVER]: bytes received: %ld\n", size);

    // PROCESAR MENSAJE

    memcpy(&query, buffer, sizeof(query));

    // printf("[CLIENT]:\n");
    // print_msg(&query);
    // printf("\n\n");
    if (create_res(&query, &reply, ref_sec, ref_milsec) == -1) continue;
    

    // printf("[SERVER]: reply\n");
    // print_msg(&reply);
    // printf("\n\n");
    // ENVIAR REPUESTA
    sendto(sockfd, &reply, BUF_SIZE, 0, (const struct sockaddr *)&client_addr, addr_size);    
  }

  close(sockfd);
  printf("Fin del programa\n");
  return 0;
}


int create_res(struct ntp_msg *msg, struct ntp_msg *reply, u_int32_t ref_sec, u_int32_t ref_milsec) {
  struct timeval time_now, time_recv;
  u_int32_t  seconds, milisecond;
  if (gettimeofday(&time_recv, NULL) == -1) return -1;
  uint32_t recv_sec, recv_milsec;
  recv_sec = swap_bytes(time_recv.tv_sec + JAN_1970);
  recv_milsec = swap_bytes(htonl((uint32_t)((1.0e-6 * time_recv.tv_usec) * UINT_MAX)));

  memset(&time_now, 0, sizeof(time_now));

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
  reply->refid = 0;

  reply->orgtime.int_partl = msg->xmttime.int_partl;
  reply->orgtime.fractionl = msg->xmttime.fractionl;

  if (gettimeofday(&time_now, NULL) == -1) return -1;
  seconds = swap_bytes(time_now.tv_sec + JAN_1970);
  milisecond = swap_bytes(htonl((u_int32_t)((1.0e-6 * time_now.tv_usec) * UINT_MAX)));

  reply->reftime.int_partl = ref_sec; reply->reftime.fractionl = ref_milsec;
  reply->rectime.int_partl = recv_sec; reply->rectime.fractionl = recv_milsec;
  reply->xmttime.int_partl = seconds; reply->xmttime.fractionl = milisecond;

  return 0;
}

int print_msg(struct ntp_msg *recv_msg) {

  u_int64_t t1 = recv_msg->reftime.int_partl, tf1 = recv_msg->reftime.fractionl;
  u_int64_t t2 = recv_msg->orgtime.int_partl, tf2 = recv_msg->orgtime.fractionl;
  u_int64_t t3 = recv_msg->rectime.int_partl, tf3 = recv_msg->rectime.fractionl;
  u_int64_t t4 = recv_msg->xmttime.int_partl, tf4 = recv_msg->xmttime.fractionl;

  printf("status: %d\n", recv_msg->status);
  printf("stratum: %d\n", recv_msg->stratum);
  printf("ppoll : %d\n", recv_msg->ppoll);
  printf("precision : %d\n", recv_msg->precision);
  printf("rootdelay : %d.%d\n", recv_msg->rootdelay.int_parts, recv_msg->rootdelay.fractions);
  printf("dispersion : %d.%d\n", recv_msg->dispersion.int_parts, recv_msg->dispersion.fractions);
  printf("refid : %d\n", recv_msg->refid);
  printf("reftime : %ld.%ld\n", t1, tf1);
  printf("orgtime : %ld.%ld\n", t2, tf2);
  printf("rectime: %ld.%ld\n", t3, tf3);
  printf("xmttime: %ld.%ld\n", t4, tf4);

  return 0;
}
