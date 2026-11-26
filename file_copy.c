
/* file_copy.c */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifndef WIN32
#include <unistd.h>
#endif				/*  */

#include "../nio_log/nio_log.h"
#include "ssh2_lib.h"

#ifdef WIN32
#include <io.h>
#define S_IWUSR 0
#define S_IRUSR 0
#endif				/*  */

#ifndef BUFSIZ
#define BUFSIZ  (1024)
#endif				/*  */

#define BLOCK    (1)
#define NO_BLOCK (0)

LIBSSH2_SESSION *_get_session(void);
PRIVATE LIBSSH2_CHANNEL *channel;

PRIVATE int get_file_size(const char *path)
{
	struct stat sb;
	stat(path, &sb);
	return (sb.st_size);
}

PRIVATE int scp_write_file(const char *local_path)
{
	int file, n;
	char buf[BUFSIZ];

	if ((file = open(local_path, O_RDWR
#ifdef WIN32
			 | _O_BINARY
#endif				/*  */
			 , 0)) == -1) {
		nio_log_print( "scp_write_file(): unable to open file %s.\n",
			local_path);
		return (-1);
	}

	libssh2_channel_set_blocking(channel, BLOCK);

	while ((n = read(file, buf, BUFSIZ)) > 0) {
		if (libssh2_channel_write(channel, buf, n) != n) {
			nio_log_print("scp_write_file(): write error on file %s\n",
				local_path);
		}
	}

	close(file);
	return (0);
}

PRIVATE int scp_read_file(const char *local_path, int filesize)
{
	int file, n;
	char buf[BUFSIZ];
	int count = 0;

	if ((file = open(local_path, O_RDWR | O_CREAT | O_TRUNC
#ifdef WIN32
			 | _O_BINARY
#endif				/*  */
			 , S_IRUSR | S_IWUSR)) == -1) {
		nio_log_print("scp_read_file(): unable to create file %s.\n",
			local_path);
		return (-1);
	}

	libssh2_channel_set_blocking(channel, BLOCK);

	while (count < filesize) {
		n = libssh2_channel_read(channel, buf, BUFSIZ);
		count += n;
		if (write(file, buf, n) != n) {
			nio_log_print(
				"scp_read_file(): write error on file %s\n",
				local_path);
		}
	}

	close(file);
	return (0);
}

PRIVATE void free_scp_channel(void)
{
	if (channel) {
		libssh2_channel_free(channel);
		channel = NULL;
	}
}

PUBLIC int send_file(const char *local_path, const char *remote_path,
		     int remote_mode)
{
	int f_size;

	/* does file exist? */
	f_size = get_file_size(local_path);
	if (f_size < 0) {
		nio_log_print(
			"send_file(): unable to get file size for file %s\n",
			local_path);
		return (-1);
	}

	nio_log_print("send_file():\n");
	nio_log_print("  file size = %d\n", f_size);
	nio_log_print("  local path = %s\n", local_path);
	nio_log_print("  remote path = %s\n", remote_path);
	nio_log_print("  remote mode = %o\n", remote_mode);

	/* open the channel */
	channel =
	    libssh2_scp_send(_get_session(), remote_path, remote_mode,
			     f_size);

	if (channel == NULL) {
		nio_log_print("send_file(): unable to open channel\n");
		return (-1);
	}

	if (scp_write_file(local_path) < 0) {
		nio_log_print("send_file(): unable to write file\n");
		free_scp_channel();
		return (-1);
	}

	free_scp_channel();
	return (0);
}

PUBLIC int recv_file(const char *remote_path, const char *local_path)
{
	struct stat sb;

	channel = libssh2_scp_recv(_get_session(), remote_path, &sb);

	if (channel == NULL) {
		nio_log_print("recv_file(): unable to open channel\n");
		return (-1);
	}

	if (scp_read_file(local_path, sb.st_size) < 0) {
		nio_log_print("recv_file(): unable to read file\n");
		free_scp_channel();
		return (-1);
	}

	free_scp_channel();
	return (0);
}
