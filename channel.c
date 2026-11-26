
#include "../nio_log/nio_log.h"
#include "channel.h"
#include "session.h"

#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/select.h>
#endif

#define BLOCK    (1)
#define NO_BLOCK (0)

PRIVATE LIBSSH2_CHANNEL *channel;

PUBLIC LIBSSH2_CHANNEL *get_channel(void)
{
	return (channel);
}

PUBLIC void free_channel(void)
{
	if (channel) {
		libssh2_channel_flush(channel);
		libssh2_channel_free(channel);
		channel = NULL;
	}
}

PUBLIC int open_channel(void)
{
	if (!(channel = libssh2_channel_open_session(get_session()))) {
		nio_log_print("open_channel(): Unable to open a session\n");
		return (-1);
	}
	return (0);
}

PUBLIC int puts_channel(const char *str)
{
	int i;
	i = strlen(str);

//      printf("OUT> %s\n", str);

	if (libssh2_channel_write(channel, str, i) <= -1) {
		nio_log_print(
			"puts_channel(): libssh2_channel_write() failed.\n");
		return (-1);
	}
	return (i);
}

PUBLIC int gets_channel(char *str, int max_len)
{
	int i;
	libssh2_channel_set_blocking(channel, BLOCK);
	if ((i = libssh2_channel_read(channel, str, max_len - 1)) <= -1) {
		nio_log_print("gets_channel(): libssh2_channel_read() failed.\n");
		return (-1);
	}
	str[i] = '\0';

//      printf("IN> %s\n", str);

	return (strlen(str));
}

PUBLIC int read_channel(char *buf, int n, int shouldblock)
{
	libssh2_channel_set_blocking(channel,
				     shouldblock ? BLOCK : NO_BLOCK);

	return (libssh2_channel_read(channel, buf, n));
}

PUBLIC int write_channel(char *buf, int n)
{
	return (libssh2_channel_write(channel, buf, n));
}

PUBLIC int wait_read_channel(int usec)
{
	fd_set readfds;
	struct timeval tv;
	int rv;

	int s = get_session_socket();

	// check first for held ssh2 internal data
	if (libssh2_poll_channel_read(channel, 0)) {
//              nio_log_print("wait_read_channel(): ssh2 queue\n");
		return 1;
	}
	// clear the set ahead of time
	FD_ZERO(&readfds);
	// add our descriptors to the set
	FD_SET(s, &readfds);

	// then check for socket data with timeout
	tv.tv_sec = 0;
	tv.tv_usec = usec;
	rv = select(s + 1, &readfds, NULL, NULL, &tv);

	if (rv == -1) {
		return 0;
	} else if (rv == 0) {
		return 0;
	} else {
		if (FD_ISSET(s, &readfds)) {
//                      nio_log_print("wait_read_channel(): socket data\n");
			return 1;
		}
	}

	return 0;
}
