/* test_ssh2_lib.c - exercises ssh2_lib without a real SSH server (2026).
 * Run with `make test`.
 *
 * It checks that every API function links against the system libssh2,
 * that init_session() fails cleanly when the peer is not an SSH server,
 * and that scp_get() fails cleanly for a host that does not resolve.
 * Real logins, commands and file copies need an SSH server and account,
 * so they are not tested here. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <signal.h>
#include "sock_lib.h"
#include "log_lib.h"
#include "ssh2_lib.h"
#include "ssh2_lib_version.h"

static int n_pass, n_fail;
#define CHECK(c) do { if (c) { n_pass++; printf("  ok   %s\n", #c); } \
	else { n_fail++; printf("  FAIL %s  (%s:%d)\n", #c, __FILE__, __LINE__); } } while (0)
#define DONE(name) do { printf("%s: %d passed, %d failed\n", name, n_pass, n_fail); \
	return n_fail ? 1 : 0; } while (0)

/* in scp_get.c; send_file() and recv_file() take their session from it */
extern LIBSSH2_SESSION *_get_session(void);

/* every public function, so the link proves they all resolve */
static void *api[] = {
	(void *) get_session, (void *) free_session, (void *) init_session,
	(void *) get_session_socket, (void *) get_channel,
	(void *) free_channel, (void *) open_channel, (void *) puts_channel,
	(void *) gets_channel, (void *) read_channel, (void *) write_channel,
	(void *) wait_read_channel, (void *) remote_command,
	(void *) open_shell, (void *) request_subsystem, (void *) send_file,
	(void *) recv_file, (void *) scp_get,
};

int main(void)
{
	int port = 20000 + (getpid() % 20000);
	SOCKET c = INVALID_SOCKET;
	pid_t pid;
	int i, status = -1;
	unsigned n;

	printf("ssh2_lib %s (%s), libssh2 %s\n", SSH2_LIB_VERSION,
	       SSH2_LIB_ORIGIN, libssh2_version(0));
	log_lib_use_stdout(FALSE);
	alarm(30);

	for (n = 0, i = 0; i < (int) (sizeof(api) / sizeof(api[0])); i++)
		n += api[i] != NULL;
	CHECK(n == sizeof(api) / sizeof(api[0]));

	CHECK(libssh2_init(0) == 0);

	/* a peer that accepts and hangs up without speaking SSH */
	pid = fork();
	if (pid == 0) {
		SOCKET s;

		alarm(30);
		s = init_tcp_server(port);
		close_socket(s);
		_exit(s >= 0 ? 0 : 1);
	}
	for (i = 0; i < 100 && c == INVALID_SOCKET; i++) {
		c = init_tcp_client("127.0.0.1", port);
		if (c == INVALID_SOCKET)
			usleep(50000);
	}
	CHECK(c != INVALID_SOCKET);
	CHECK(init_session(c, "nobody", "not-a-password") == -1);
	close_socket(c);

	/* 1.1 fix: send_file()/recv_file() used scp_get()'s session, which
	 * is NULL outside scp_get(). They now use init_session()'s. Here
	 * that session never finished its handshake, so they fail cleanly
	 * (NewIO 0.12 passed NULL to libssh2). */
	signal(SIGPIPE, SIG_IGN);
	CHECK(get_session() != NULL && _get_session() == get_session());
	CHECK(send_file("Makefile", "/tmp/not-sent", 0644) == -1);
	CHECK(recv_file("/remote/file", "local.file") == -1);

	waitpid(pid, &status, 0);
	CHECK(WIFEXITED(status) && WEXITSTATUS(status) == 0);

	CHECK(scp_get("/remote/file", "local.file", "pw", "user",
		      "no-such-host.invalid") == -1);
	CHECK(access("local.file", F_OK) != 0);
	CHECK(_get_session() == get_session());	/* scp_get left none set */

	libssh2_exit();

	DONE("ssh2_lib");
}
