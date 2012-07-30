/*
 * client.c
 *
 *  Created on: Jul 10, 2012
 *      Author: Andrea Romanello, Amir Curic
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
	int sd2 = -1; //, ret;
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
	if (argc != 1 && argc != 4) {
		perror("The program run with 0 argument or with 3 integer");
		exit(1);
	}

	int num_sons = 1;
	int num_reparto = 2; //Reparto di default radiologia(2)
	int prior = 0;

	if (argc == 4) {
		if (sscanf(argv[1], "%d", &num_sons) <= 0) {
			printf("The argument are int int int \n");
			exit(EXIT_FAILURE);
		}
		if (sscanf(argv[2], "%d", &num_reparto) <= 0) {
			printf("The argument are int int int \n");
			exit(EXIT_FAILURE);
		}
		if (sscanf(argv[3], "%d", &prior) <= 0) {
			printf("The argument are int int int \n");
			exit(EXIT_FAILURE);
		}
	}

	/*
	 if (argc >= 2) { //Controllo se ci sono parametri passati in input
	 num_sons = atoi(argv[1]);
	 num_reparto = atoi(argv[2]);
	 prior = atoi(argv[3]);
	 //	printf("\t\tSONS: %d, REP; %d, PRIOR: %d", numSons,numReparto,prior);
	 }
	 */

	int MSG_Q__main_bus;

	int sock_id;
	sock_id = 0;

	MSG_Q__main_bus = msgget(SERVER_KEY, 0);
	if (MSG_Q__main_bus < 0) {
		perror("The message queue don't exist");
		exit(1);
	}

	int son, i = 0;

	printf("\nCreazione di %d figli:\n", num_sons);

	for (i = 0; i < num_sons; i++) {

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
			printf("\t\t\n==> Sono il %d. figlio PID: %d\n", i + 1, getpid());

			create_socket(&sock_id);

			richiesta->mtype = TOSRV;
			richiesta->clientId = getpid();
			richiesta->priority = prior;
			richiesta->kindof_service = num_reparto;
			richiesta->turn = 0;
			richiesta->price = 0;

			msgsnd(MSG_Q__main_bus, richiesta, sizeof(request), TOSRV);
			printf("\t\tRichiesta inviata\n");

			recive_socket(&sock_id);

			printf("\t\tIn attesa della risposta\n");

			msgrcv(MSG_Q__main_bus, risposta, sizeof(response), TOCLI, 0);

			printf("\t\tRicevuta, ho il turno: %d\n\n", risposta->turn);
			printf(
					"...............................................................................\n\n");

			free(richiesta);
			free(risposta);
			close_socket(&sock_id, getpid());
			//exit(0); Da togliere
		} else {
			wait(0);
			exit(0);
		}
	}

	return 0;
}
