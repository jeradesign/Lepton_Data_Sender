#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <cstring>
#include <cassert>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "Lepton_Frame.h"
#include "Lepton_Driver.h"

int socket_fd;
struct sockaddr_in servaddr;

static void setupConnection(char *hostname)
{
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(11539);
    servaddr.sin_addr.s_addr = inet_addr(hostname);
    connect(socket_fd, (const sockaddr*)&servaddr, sizeof(servaddr));
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		return -1;
	}

	setupConnection(argv[1]);

	setupSPI();

	for (;;) {
        uint8_t *frame = nextFrame();
        if (!checkFrame()) {
            continue;
        }
        write(socket_fd, frame, FRAME_SIZE);
	}
}

