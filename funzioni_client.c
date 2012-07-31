/*
 * funzioni_client.c
 *
 *  Created on: Jul 30, 2012
 *      Author: Andrea Romanello, Amir Curic
 */

/**
 * crea un socket
 */
#ifndef FUNZIONI_CLIENT_H_
#include "funzioni_client.h"
#endif

void create_socket(int *sd) {
	int ret;
	struct sockaddr_un srvaddr;
	char sock[20];

	int pid = getpid();
	sprintf(sock, "/tmp/%d.sock", pid);

	*sd = (int) socket(AF_UNIX, SOCK_STREAM, 0);
	if (*sd < 0) {
		perror("Unable to open server socket");
		exit(1);
	}
	memset(&srvaddr, 0, sizeof(srvaddr));

	srvaddr.sun_family = AF_UNIX;
	strcpy(srvaddr.sun_path, sock);

	ret = bind(*sd, (struct sockaddr *) &srvaddr, sizeof(srvaddr));
	if (ret < 0) {
		perror("Unable to bind socket server");
		close(*sd);
		exit(1);
	}

	ret = listen(*sd, 1);
	if (ret < 0) {
		perror("Unable to listen socket server");
		exit(1);
	}
}

/**
 * funzione per la chiusura del socket
 * @param s puntatore al socket
 * @param p pid del processo che ha aperto il socket, serve
 * per la creazione del nome
 */
void close_socket(int *s, int p) {
	char sock[20];
	sprintf(sock, "/tmp/%d.sock", p);

	close(*s);
	unlink(sock);
}


/**
 * questa funzione serve per leggere il socket in ingresso
 */
void recive_socket(int *sd) {
	int sd2;
	char buf[200];

	sd2 = -1;

	sd2 = accept(*sd, NULL, NULL );
	if (sd2 < 0) {
		perror("Unable to accept socket server");
		exit(1);
	}

	if (read(sd2, buf, sizeof(char) * 200) < 0) {
		perror("Unable st read on socket");
		exit(1);
	}

	printf("\n%s\n", buf);

	close(*sd);
	close(sd2);
}
