#include "client.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define ERROR 1
#define EXITO 0

bool set_remote_address(struct addrinfo *hints, struct addrinfo **client_info, 
const char *host, const char *service) {

	memset(hints, 0, sizeof(struct addrinfo));
   	hints->ai_family = AF_INET;       // IPv4 (or AF_INET6 for IPv6)
   	hints->ai_socktype = SOCK_STREAM; // TCP  (or SOCK_DGRAM for UDP) 
   	hints->ai_flags = 0;              // None

   	int s;
   	if ((s = getaddrinfo(host, service, hints, client_info)) != 0) { 
      printf("Error in getaddrinfo: %s\n", gai_strerror(s));
      return false;
	}
	printf("%d\n", s); 
	printf("Remote address successfully setted..\n"); 
	return true;
}

bool client_run(const char *host, const char *service, const char *data) {
	struct addrinfo hints;
   	struct addrinfo *client_info;

   	if (!set_remote_address(&hints, &client_info, host, service)) {
   	 	return ERROR;
   	}
    return EXITO;
}
