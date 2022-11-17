#include <time.h>

// socket
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <netdb.h>
#include <unistd.h>

#define BUF_SIZE 48 // bytes
#define PORT 3003
#define IP "0.0.0.0"

struct l_fixedpt {
  u_int32_t int_partl;
  u_int32_t fractionl;
};

struct s_fixedpt {
  u_int16_t int_parts;
  u_int16_t fractions;
};

struct ntp_msg {
  u_int8_t status;	/* status of local clock and leap info */
  u_int8_t stratum;	/* Stratum level */
  u_int8_t ppoll;	/* poll value */
  int8_t precision;
  struct s_fixedpt rootdelay;
  struct s_fixedpt dispersion;
  u_int32_t refid;
  struct l_fixedpt reftime;
  struct l_fixedpt orgtime;
  struct l_fixedpt rectime;
  struct l_fixedpt xmttime;
};
