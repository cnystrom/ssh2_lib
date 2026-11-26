
#include "../nio_log/nio_log.h"
#include "sock_lib.h"
#include "ssh2_lib.h"

#define SSH_PORT (22)

PRIVATE LIBSSH2_SESSION *session;

PUBLIC LIBSSH2_SESSION *_get_session(void)
{
	return (session);
}

PRIVATE int _init_session(int sock, const char *username,
			  const char *passwd)
{

	/* Create a session instance and start it up
	 * This will trade welcome banners, exchange keys, and setup crypto,
	 * compression, and MAC layers
	 */
	session = libssh2_session_init();
	if (libssh2_session_startup(session, sock)) {
		nio_log_print("_init_session(): failure establishing SSH session\n");
		return -1;
	}

	/* We could authenticate via password */
	if (libssh2_userauth_password(session, username, passwd)) {
		nio_log_print("_init_session(): authentication by password failed.\n");
		free_session();
		return (-1);
	}
	return 0;
}

PRIVATE void _free_session(void)
{
	libssh2_session_disconnect(session, "Normal Shutdown.");
	libssh2_session_free(session);
}

PUBLIC int scp_get(const char *from, const char *to, const char *pass,
		   const char *user, const char *host)
{
	int sock;

//      printf("%s %s %s %s %s\n", from, to, pass, user, host);
//      printf("--\n");

	sock = init_tcp_client(host, SSH_PORT);

	if (sock <= 0) {
		return (-1);
	}

	if (_init_session(sock, user, pass) != 0) {
		nio_log_print("scp_get(): init_session() failed.\n");
		close_socket(sock);
		return (-1);
	}	

//      printf("from: %s to: %s\n", from, to);

	if (recv_file(from, to) == -1) {
		nio_log_print("scp_get(): file copy failed.\n");
		return (-1);
	}

	_free_session();
	close_socket(sock);

//      printf("all done\n--\n");

	return 0;
}
