#include "libft.h"
#include "server.h"
#include "websocket.h"

void				pipe_com_write(int fd, const char *msg)
{
	unsigned char	buf[BUFSIZE];

	if (msg && fd > 0)
	{
		strncpy((char*)buf, (const char*)msg, BUFSIZE);
// 		fprintf(stdout, "buf = [%s]\n", buf);//_DEBUG_//
		if (write(fd, buf, BUFSIZE) != BUFSIZE && errno != EAGAIN)
		{
			perror("write");
			exit(EXIT_FAILURE);
		}
	}
}

unsigned char		*pipe_com_read(int fd)
{
	unsigned char	*buf;

	if (!(buf = (unsigned char*)malloc(sizeof(unsigned char) * BUFSIZE)))
	{
		fprintf(stderr, "Error : malloc");
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	memset(buf, 0, BUFSIZE);
	if (read(fd, buf, BUFSIZE) != BUFSIZE && errno != EAGAIN)
	{
		perror("read");
 		exit(EXIT_FAILURE);
	}
// 	fprintf(stdout, "buf = [%s]\n", buf);//_DEBUG_//
	return (buf);
}
