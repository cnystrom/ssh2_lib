# ssh2_lib: SSH sessions, commands, subsystems and SCP on libssh2

Version 1.0 (2026). Split from NewIO 0.12, https://github.com/cnystrom/newio

ssh2_lib is NewIO's thin layer over libssh2. It opens an SSH session on a
connected socket with a password, opens a channel, runs a remote command,
starts a shell or an ssh subsystem (NewIO's client ran its server this way),
moves data on the channel, and copies files with SCP.

## API

From `ssh2_lib.h`, `session.h` and `channel.h`:

| Function | What it does |
|---|---|
| `int init_session(int sock, const char *username, const char *passwd)` | SSH handshake and password login on a connected socket. 0 on success. |
| `LIBSSH2_SESSION *get_session(void)`, `int get_session_socket(void)`, `void free_session(void)` | The current session. |
| `int open_channel(void)`, `LIBSSH2_CHANNEL *get_channel(void)`, `void free_channel(void)` | The current channel. |
| `int puts_channel(const char *str)`, `int gets_channel(char *str, int max_len)` | String write and read. |
| `int read_channel(char *buf, int n, int shouldblock)`, `int write_channel(char *buf, int n)` | Raw channel I/O. |
| `int wait_read_channel(int usec)` | 1 if channel data is waiting (checks libssh2's buffer, then `select()`). |
| `int remote_command(const char *command)` | Runs a command on a new channel. |
| `int open_shell(void)` | Opens a channel with a pty and a shell. |
| `int request_subsystem(const char *system)` | Opens a channel and starts an ssh subsystem. |
| `int send_file(...)`, `int recv_file(...)` | SCP upload and download on the current session: `scp_get()`'s while it runs, otherwise the `init_session()` one. |
| `int scp_get(from, to, pass, user, host)` | Connects to `host:22`, logs in, downloads one file and disconnects. |

`ssh2_lib_version.h` defines `SSH2_LIB_VERSION` ("1.0") and `SSH2_LIB_ORIGIN`.

## Dependencies

- **libssh2** (system library, `-lssh2`), not copied into this project.
  Tested with libssh2 1.11.1. Install `libssh2-1-dev` on Debian/Ubuntu.
- **sock_lib** (sibling project): `scp_get()` uses `init_tcp_client()`.
- **log_lib** (sibling project) for log messages.

The `Makefile` looks in `../sock_lib` and `../log_lib` (override with
`SOCK_LIB_DIR=`/`LOG_LIB_DIR=`). Link order:
`ssh2_lib.a sock_lib.a log_lib.a -lssh2`.

## Build

Check out sock_lib and log_lib next to this one, install the libssh2
development package, then:

```sh
make            # builds ssh2_lib.a
make test       # builds the siblings if needed, then runs test_ssh2_lib
make CC=clang test
make clean
```

When unpacked from the release tarballs, the directory names include the
version, so give the sibling paths:

```sh
make SOCK_LIB_DIR=../sock_lib-1.0 LOG_LIB_DIR=../log_lib-1.0 test
```

`make test` needs no SSH server. It checks that every function links
against libssh2, that `init_session()` fails cleanly against a peer that
isn't an SSH server, and that `scp_get()` fails cleanly for a host that
doesn't resolve. Real logins, commands and copies need an SSH server and an
account, so they are **not** tested.

## License

LGPL 2.1 or later, the same as NewIO. See `LICENSE` (GNU LGPL 2.1). `COPYING`
holds the GNU GPL 2, as in NewIO. Copyright Chris Nystrom; see the header of
each file for its years.

## Origin

Split from NewIO 0.12, https://github.com/cnystrom/newio (tag `newio-0.12`,
commit `ce54c5b`), directory `common/ssh2_lib/`. The library code is Chris
Nystrom's original, copied unchanged except for the lines listed under
"Changes from NewIO" below.

Contact: Chris Nystrom <cnystrom@gmail.com>

## Changes from NewIO

- `channel.c:2`, `file_copy.c:12`, `remote_command.c:2`, `scp_get.c:2`,
  `session.c:4`, `shell.c:2`, `subsystem.c:2`:
  `#include "../nio_log/nio_log.h"` became `#include "log_lib.h"`.
- `session.c:37` and `scp_get.c:28`: `libssh2_session_startup()` became
  `libssh2_session_handshake()`. The old name has been deprecated since
  libssh2 1.2.8 and gave a warning. Same arguments, same behaviour.
- `file_copy.c:148` and `:150`: `struct stat sb` with `libssh2_scp_recv()`
  became `libssh2_struct_stat sb` with `libssh2_scp_recv2()`. The old call
  has been deprecated since libssh2 1.7.0 and gave a warning. Only
  `sb.st_size` is used.
- `scp_get.c:10-17`, `:30`, `:37`, `:47` and `:74-75`: bug fix, see
  "Fixed in 1.0".
- `Makefile:34-36`: `-I.. -I../sock_lib` became `-I. -I$(LOG_LIB_DIR)
  -I$(SOCK_LIB_DIR)`.
- Added `ssh2_lib_version.h`, `test_ssh2_lib.c`, the sibling and `make test`
  rules at the end of the `Makefile`, this README, `LICENSE` and `COPYING`.
- Not copied: `archive.sh` and `attic/` (old examples).
- Left as is: `fingerprint` is set but not used (`session.c:29`). See
  "Known issues".

## Fixed in 1.0

- **`send_file()` and `recv_file()` only worked inside `scp_get()`**
  (`scp_get.c`). They take their session from `_get_session()`, which
  returned `scp_get()`'s private session. Outside `scp_get()` that was
  `NULL`. The fix:
  - `scp_get.c:12-17`: `_get_session()` returns `scp_get()`'s session while
    one is open, and otherwise the `init_session()` session.
  - `scp_get.c:47`: `_free_session()` resets that session to `NULL`.
  - `scp_get.c:30`, `:37` and `:74-75`: every failure path now frees
    `scp_get()`'s own session. Before, a failed handshake left a dead session
    behind, a failed login freed the `init_session()` session by mistake,
    and a failed copy leaked the session and the socket.
  - `scp_get.c:10`: a forward declaration of `_free_session()`.

  Tested in `test_ssh2_lib.c`: `_get_session()` now matches
  `get_session()`, and `send_file()`/`recv_file()` return -1 instead of
  crashing. A real copy over SSH still needs a server, so it is untested.

## Known issues

Kept as in NewIO 0.12:

- Only password login is supported, and the server's host key is not
  checked (`session.c:43` reads the fingerprint but doesn't use it).
- `file_copy.c:34`: `get_file_size()` ignores a failed `stat()`.
  `file_copy.c:86`: `scp_read_file()` keeps adding the read result even when
  it is negative.
- There is one session and one channel per process: all state is static.
- Some comments in `session.c` and `shell.c` follow libssh2's example
  programs.
