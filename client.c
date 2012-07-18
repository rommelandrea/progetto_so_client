/*
 * client.c
 *
 *  Created on: Jul 10, 2012
 *      Author: rommel
 */

#include "header_proj.h"

/**
 * crea un socket
 */
void create_socket(int *sd) {
	//printf("SD %d\n", *sd);
	int ret;
	struct sockaddr_un srvaddr;
	char sock[20];

	int pid = getpid();
	sprintf(sock, "/tmp/%d.sock", pid);

	*sd = (int)socket(AF_UNIX, SOCK_STREAM, 0);
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

void close_socket(int *s, int p){
	char sock[20];
	sprintf(sock, "/tmp/%d.sock", p);

	close(*s);
	unlink(sock);
}

/**
 * questa funzione serve per leggere il socket in ingresso
 */
void recive_socket(int *sd) {
	int sd2 = -1, ret;
	char buf[200];

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

int main(int argc, char **argv) {
	
	int numSons = 0;	 
	int numReparto = 0;	
	int prior = 2; //Reparto di default è radiologia(2)

	if (argc > 1) { //Controllo se ci sono parametri passati in input
		numSons = atoi(argv[1]);
		numReparto = atoi(argv[2]);
		prior = atoi(argv[3]);	
	//	printf("\t\t\t\t SONS: %d, REP; %d, PRIOR: %d", numSons,numReparto,prior);
	}
	
	int MSG_Q__main_bus;
	

	int sock_id;
	sock_id = 0;

	MSG_Q__main_bus = msgget(SERVER_KEY, 0);
	if (MSG_Q__main_bus < 0) {
		perror("The message queue don't exist");
		exit(1);
	}

	int son;
	int count = 0;
	printf("\n\tCreazione %d figli\n", numSons);
	
	for (count=0; count<numSons; count++) {
		printf("\tCount:%d\n\n", count);
		
		son = fork();
		if (son < 0) {
			perror("Unable to create son\n");
			fflush(stdout);
			exit(1);	
		}
		if (son == 0) {
			request *richiesta = malloc(sizeof(request));
			response *risposta = malloc(sizeof(response));
				
			/**
		 	* apro il socket
		 	*/
		 	printf("\t\t\n\t==> Sono figlio! %d\n\n", getpid());
		 	
			create_socket(&sock_id);

			richiesta->mtype = TOSRV;
			richiesta->clientId = getpid();
			richiesta->priority = IMONHOLODAY;
			richiesta->kindof_service = OPHTHALMOLOGY;
			richiesta->turn = 0;
			richiesta->price = 0;

			msgsnd(MSG_Q__main_bus, richiesta, sizeof(request), TOSRV);
			printf("\t\trichiesta inviata\n");

			recive_socket(&sock_id);

			printf("\t\tin attesa della risposta\n");

			msgrcv(MSG_Q__main_bus, risposta, sizeof(response), TOCLI, 0);

			printf("\t\tricevuta, ho il turno: %d\n\n", risposta->turn);

			free(richiesta);
			close_socket(&sock_id, getpid());	
		}
		else {
			wait(0);	
		}		
	}


	return 0;
}
