/*
 * client.c
 *
 *  Created on: Jul 10, 2012
 *      Author: rommel
 */


#include "header_proj.h"

void recive_socket(){
	int sd = -1, sd2 = -1, n;

	struct sockaddr_un srvaddr;

	char sock[20];
	int pid = getpid();
	sprintf(sock, "%d.sock", pid);

	printf("\nSOCKCLI %s\n", sock);

	sd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sd < 0) {
		perror("Unable to open server socket");
		exit(1);
	}
	memset(&srvaddr, 0, sizeof(srvaddr));

	srvaddr.sun_family = AF_UNIX;
	strcpy(srvaddr.sun_path, sock);

//	len = offsetof(struct sockaddr_un, sun_path) + strlen("srv.sock");

	//n = bind(sd, (struct sockaddr *)&srvaddr, SUN_LEN(&srvaddr));
	n = bind(sd, (struct sockaddr *)&srvaddr, sizeof(srvaddr));
	if (n < 0) {
		perror("Unable to bind socket server");
		exit(1);
	}

	n = listen(sd, 1);
	if (n < 0) {
		perror("Unable to listen socket server");
		exit(1);
	}

	sd2 = accept(sd, NULL, NULL );
	if (sd2 < 0) {
		perror("Unable to accept socket server");
		exit(1);
	}

	char buf[100];

	n = read(sd2, buf, 100);
	printf("ho ricevuto %s\n", buf);
	//strcpy(buf, "dati server");
	//n = read(sd2, buf, sizeof("dati server"));

	close(sd);
	close(sd2);
	unlink(sock);
}

int main(int argc, char **argv){
	int MSG_Q__main_bus;
	request *richiesta = malloc(sizeof(request));

	MSG_Q__main_bus = msgget(SERVER_KEY, 0);
	if(MSG_Q__main_bus < 0 ){
		perror("The message queue don't exist");
		exit(1);
	}

	richiesta->mtype = TOSRV;
	richiesta->clientId = getpid();
	richiesta->priority = ILLWAIT;
	richiesta->kindof_service = OPHTHALMOLOGY;
	richiesta->turn = 0;
	richiesta->price = 0;

	msgsnd(MSG_Q__main_bus, richiesta, sizeof(request), TOSRV);
	printf("richiesta inviata\n");

	printf("il mio pid %d\n", getpid());

	recive_socket();

	free(richiesta);

	return 0;
}
