
/* exmaple2.c */

#include "ssh2_lib.h"
#include <stdio.h>
#include "../sock_lib/sock_lib.h"

#define SSH_PORT (22)
int main(int argc, char *argv[])
{
	int sock;
	char buf[256] = "";
	char version[] = "0.09_bin_050\n";
	const char *server = "shuttle.newio.org";
	const char *username = "cnystrom";
	const char *passwd = "CHANGEME";
	printf("--\n");
	sock = init_tcp_client(server, SSH_PORT);
	if (sock <= 0) {
		return (-1);
	}
	if (init_session(sock, username, passwd) != 0) {
		close_socket(sock);
		return (-1);
	}
	if (request_subsystem("newio")) {
		printf("Remote command failed.\n");
	} else {
		printf("Remote command succeeded.\n");
	}
	gets_channel(buf, 256);
	printf("%s", buf);
	puts_channel(version);
	gets_channel(buf, 256);
	printf("%s", buf);
	free_channel();
	free_session();
	close_socket(sock);
	printf("all done\n--\n");
	return 0;
}
