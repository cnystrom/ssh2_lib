
/* exmaple3.c */

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
		printf("init_session() failed.\n");
		close_socket(sock);
		return (-1);
	}
	if (send_file("./example3.c", "~/example3.c", 00644) == -1) {
		printf("File copy failed.\n");
	} else {
		printf("File copy succeeded.\n");
	}
	printf("--\n");
	if (recv_file("/etc/hosts", "my_hosts") == -1) {
		printf("File copy failed.\n");
	} else {
		printf("File copy succeeded.\n");
	}
	printf("--\n");
	free_session();
	close_socket(sock);
	printf("all done\n--\n");
	return 0;
}
