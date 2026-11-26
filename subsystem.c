
#include "../nio_log/nio_log.h"
#include "ssh2_lib.h"

PUBLIC int request_subsystem(const char *system)
{
	if (open_channel() != 0) {
		nio_log_print("request_subsystem(): open_channel() failed.\n");
		return (-1);
	}
	if (libssh2_channel_subsystem(get_channel(), system) != 0) {
		nio_log_print(
		"request_subsystem(): libssh2_channel_subsystem() failed.\n");
		free_channel();
		return (-1);
	}
	return (0);
}
