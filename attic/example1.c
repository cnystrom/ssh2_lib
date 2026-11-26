
/* exmaple1.c */

#include "ssh2_lib.h"
#include <stdio.h>
#include "../sock_lib/sock_lib.h"

#define SSH_PORT (22)
int main(int argc, char *argv[])
{
	int sock;
	printf("--\n");
	sock = init_tcp_client("shuttle.newio.org", SSH_PORT);
	if (sock <= 0) {
		return (-1);
	}
	if (init_session(sock, "cnystrom", "CHANGEME") != 0) {
		close_socket(sock);
		return (-1);
	}
	if (remote_command("/bin/touch testx3")) {
		printf("Remote command failed.\n");
	} else {
		printf("Remote command succeeded.\n");
	}
	free_session();
	close_socket(sock);
	printf("all done\n--\n");
	return 0;
}
