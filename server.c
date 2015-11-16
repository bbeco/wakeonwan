/*
 * This program listen for a specific message on a given port and then wake on
 * the specified machine using a wake-on-LAN tool.
 * This server uses a pre-fork() approach. After a child has started its
 * execution, it runs execl() changing its image to the WOL tool.
 */

#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#define BACKLOG 10
#define N_CHILDREN 5

void
usage(void)
{
	printf("Usage:\tserver <destination MAC address> <listening port>\n");
}

int
main(int argc, char *argv[])
{
	int ret;
	int pid;
	int sock = 0, cl_sock, i;
	socklen_t cl_addr_len;
	uint16_t port;
	struct sockaddr_in addr, cl_addr;
	char *mac = argv[1];

	if (argc < 3) {
		usage();
		return 1;
	}

	port = (unsigned short)atoi(argv[2]);

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("E: socket()");
		goto fail;
	}
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	ret = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
	if (ret < 0) {
		perror("E: bind()");
		goto fail;
	}
	ret = listen(sock, BACKLOG);
	if (ret < 0) {
		perror("E: listen()");
		goto fail;
	}

	/* Creating children pool */
	for (i = 0; i < N_CHILDREN; i++) {
		pid = fork();
		if (pid == 0) //child
			break;
	}
	if (pid != 0) { //father process

		/* The following loop performs a blocking wait while looking
		 * for terminated children. When the wait() call finds one of the
		 * dead child, the father creates a new child and starts sleeping
		 * again.
		 */
		while (1) {
			pid = waitpid(-1, NULL, 0); //blocking wait looking for terminated child
			if (pid < 0) { //error
				continue;
			} else {
				pid = fork();
				if (pid == 0) //child process
					break;
			}
		}
	} else { //child process
		cl_addr_len = sizeof(cl_addr);
		memset(&cl_addr, 0, cl_addr_len);
		cl_sock = accept(sock, (struct sockaddr *)&cl_addr, &cl_addr_len); 
#ifdef DEBUG
		printf("Accepting incoming connection\n");
#endif
		if (cl_sock < 0) {
			perror("E: accept()");
			goto fail;
		}
		close(sock);
		sock = 0;
		close(cl_sock);
		cl_sock = 0;
		ret = execl("/usr/bin/wakeonlan", "/usr/bin/wakeonlan", mac, (char *)NULL);
		if (ret < 0) {
			perror("E: execl()");
			return 1;
		}
		return 0;
	}

fail:
	if (!sock)
		close(sock);
	if (!cl_sock)
		close(cl_sock);
	return 1;
}
