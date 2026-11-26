
/* channel.h */

#include <libssh2.h>

#ifndef PRIVATE
#define PRIVATE static
#endif				/*  */

#ifndef PUBLIC
#define PUBLIC
#endif				/*  */

PUBLIC LIBSSH2_CHANNEL *get_channel(void);
PUBLIC void free_channel(void);
PUBLIC int open_channel(void);
PUBLIC int puts_channel(const char *str);
PUBLIC int gets_channel(char *str, int max_len);
PUBLIC int read_channel(char *buf, int n, int shouldblock);
PUBLIC int write_channel(char *buf, int n);
PUBLIC int wait_read_channel(int usec);
