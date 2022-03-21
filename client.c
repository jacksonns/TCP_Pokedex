#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFSZ 1024

int main(int argc, char **argv) {
	if (argc < 3) {
		exit(EXIT_FAILURE);
	}

	struct sockaddr_storage storage;
	if (0 != addrparse(argv[1], argv[2], &storage)) {
		exit(EXIT_FAILURE);
	}

	int s;
	s = socket(storage.ss_family, SOCK_STREAM, 0);
	if (s == -1) {
		logexit("socket");
	}
	struct sockaddr *addr = (struct sockaddr *)(&storage);
	if (0 != connect(s, addr, sizeof(storage))) {
		logexit("connect");
	}

	char buf[BUFSZ];
	while(1){
		memset(buf, 0, BUFSZ);
		printf("> ");
		fgets(buf, BUFSZ-1, stdin);
		size_t count = send(s, buf, strlen(buf)+1, 0);
		if (count != strlen(buf)+1) {
			logexit("send");
		}

		memset(buf, 0, BUFSZ);

		unsigned total = 0;
		count = recv(s, buf + total, BUFSZ - total, 0);
		if (count == 0 || count == -1){
            close(s);
            break;
		}
		printf("< ");
		puts(buf);
	}

	exit(EXIT_SUCCESS);
}