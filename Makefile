TARGETS =	$(SSH2_LIB_BIN)

#==============================================================================

SSH2_LIB_PATH =	$(NULL)
SSH2_LIB_BIN =	ssh2_lib.a

SSH2_LIB_H =     \
                session.h \
                channel.h \
                $(NULL)

SSH2_LIB_C =     \
                file_copy.c \
                session.c \
                channel.c \
                subsystem.c \
                remote_command.c \
                scp_get.c \
                shell.c \
                $(NULL)

SSH2_LIB_OBJS =  \
                ${SSH2_LIB_C:.c=.o} \
                $(NULL)

#==============================================================================

CC =		gcc

COPTS =		-O3 -g -Wall

INCLUDES = 	\
		-I.. \
		-I../sock_lib \
		-I/usr/local/include \
		$(NULL)

LIBDIRS = 	\
		$(NULL)

ifndef $(TARGET_OS)
TARGET_OS =     UNIX
endif
 
CFLAGS =	$(COPTS) $(INCLUDES) -DDEBUG -D$(TARGET_OS)

LFLAGS =	$(LIBDIRS) -lssh2

#default compilation rules
%.o:		%.c
		$(CC) -c $< $(CFLAGS) -o $@

all: 		$(TARGETS)

default:	$(TARGETS)

new:		clean default

#==============================================================================

$(SSH2_LIB_BIN): $(SSH2_LIB_OBJS) $(SSH2_LIB_H) $(SSH2_LIB_C)
		ar rcs $(SSH2_LIB_BIN) $(SSH2_LIB_OBJS)
		ranlib $(SSH2_LIB_BIN)

$(SOCK_LIB_BIN):
		cd $(SOCK_LIB_PATH); make

indent:
		dos2unix *.h *.c
		indent -kr -i4 *.h *.c 

clean:
		rm -f $(TARGETS) *.o *.a core a.out *~
 
