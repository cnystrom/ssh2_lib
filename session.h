
/* session.h */

#include <libssh2.h>

#ifndef PRIVATE
#define PRIVATE static
#endif				/*  */

#ifndef PUBLIC
#define PUBLIC
#endif				/*  */

PUBLIC LIBSSH2_SESSION *get_session(void);
PUBLIC void free_session(void);
PUBLIC int init_session(int sock, const char *username,
			const char *passwd);
PUBLIC int get_session_socket(void);
