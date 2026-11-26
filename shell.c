
#include "../nio_log/nio_log.h"
#include "ssh2_lib.h"

PUBLIC int open_shell(void)
{
	LIBSSH2_CHANNEL *channel;
	if (open_channel() != 0) {
		nio_log_print("open_shell(): open_channel() failed.\n");
		return (-1);
	}
	channel = get_channel();

	/* Some environment variables may be set,
	 * It's up to the server which ones it'll allow though
	 */
//      libssh2_channel_setenv(channel, "FOO", "bar");
	/* Request a terminal with 'vanilla' terminal emulation
	 * See /etc/termcap for more options
	 */
	if (libssh2_channel_request_pty(channel, "vanilla")) {
		nio_log_print("open_shell(): failed requesting pty\n");
		return (-1);
	}

	/* Open a SHELL on that pty */
	if (libssh2_channel_shell(channel)) {
		nio_log_print(
		"open_shell(): unable to request shell on allocated pty\n");
		return (-1);
	}

	/* At this point the shell can be interacted with using
	 * libssh2_channel_read()
	 * libssh2_channel_read_stderr()
	 * libssh2_channel_write()
	 * libssh2_channel_write_stderr()
	 *
	 * Blocking mode may be (en|dis)abled with:
	 *   libssh2_channel_set_blocking()
	 *
	 * If the server send EOF, libssh2_channel_eof() will return non-0
	 * To send EOF to the server use: libssh2_channel_send_eof()
	 * A channel can be closed with: libssh2_channel_close()
	 */
	return (0);
}
