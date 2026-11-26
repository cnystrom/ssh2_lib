
#include "../nio_log/nio_log.h"
#include "ssh2_lib.h"

PUBLIC int remote_command(const char *command)
{
	if (open_channel() != 0) {
		nio_log_print("remote_command(): open_channel() failed.\n");
		return (-1);
	}

	if (libssh2_channel_exec(get_channel(), command) != 0) {
		nio_log_print("remote_command(): libssh2_channel_exec() failed.\n");
		free_channel();
		return (-1);
	}

	free_channel();
	return (0);
}
