
/* session.c */

#include "../nio_log/nio_log.h"
#include "session.h"

PRIVATE LIBSSH2_SESSION *session;
PRIVATE int socket;

PUBLIC LIBSSH2_SESSION *get_session(void)
{
	return (session);
}

PUBLIC int get_session_socket(void)
{
	return (socket);
}

PUBLIC void free_session(void)
{
	libssh2_session_disconnect(session, "Normal Shutdown.");
	libssh2_session_free(session);
}

PUBLIC int init_session(int sock, const char *username, const char *passwd)
{
	int auth_pw = 1;
	const char *fingerprint;

	socket = -1;
	/* Create a session instance and start it up
	 * This will trade welcome banners, exchange keys, and setup crypto,
	 * compression, and MAC layers
	 */
	session = libssh2_session_init();
	if (libssh2_session_startup(session, sock)) {
		nio_log_print("init_session(): failure establishing SSH session\n");
		return -1;
	}

	/* At this point we havn't authenticated */
	fingerprint =
	    libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_MD5);

//	int i;
//
//	printf("Fingerprint: ");
//	for (i = 0; i < 16; i++) {
//		printf("%02X ", (unsigned char) fingerprint[i]);
//	}
//	printf("\n");

	if (auth_pw) {

		/* We could authenticate via password */
		if (libssh2_userauth_password(session, username, passwd)) {
			nio_log_print("init_session(): authentication by password failed.\n");
			free_session();
			return (-1);
		}
	} else {

		/* Or by public key */
		if (libssh2_userauth_publickey_fromfile
		    (session, "username",
		     "/home/username/.ssh/id_rsa.pub",
		     "/home/username/.ssh/id_rsa", "pasphrase")) {
			nio_log_print("init_session(): authentication by public key failed\n");
			free_session();
			return (-1);
		}
	}
	socket = sock;
	return 0;
}
