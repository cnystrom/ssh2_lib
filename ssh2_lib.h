
/* ssh2_lib.h */

#include <libssh2.h>
#include "session.h"
#include "channel.h"

#ifndef PRIVATE
#define PRIVATE static
#endif				/*  */

#ifndef PUBLIC
#define PUBLIC
#endif				/*  */

#define BLOCK    (1)
#define NO_BLOCK (0)

PUBLIC int remote_command(const char *command);
PUBLIC int open_shell(void);
PUBLIC int request_subsystem(const char *system);
PUBLIC int send_file(const char *local_path, const char *remote_path,
		     int remote_mode);
PUBLIC int recv_file(const char *remote_path, const char *local_path);
PUBLIC int scp_get(const char *from, const char *to, const char *pass,
		   const char *user, const char *host);
