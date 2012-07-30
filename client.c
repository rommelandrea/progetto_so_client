/*
 * client.c
 *
 *  Created on: Jul 10, 2012
 *      Author: Andrea Romanello, Amir Curic
 */

#include "funzioni_client.h"





int main(int argc, char **argv) {
	int num_sons;
	int num_reparto;
	int prior;
	int MSG_Q__main_bus;
	int sock_id;
	int i, son;

	if (argc != 1 && argc != 4) {
		perror("The program run with 0 argument or with 3 integer");
		exit(1);
	}

	num_sons = 1;
	num_reparto = 2; /*Reparto di default radiologia(2)*/
	prior = 0; /* priorita' default 0 */

	if (argc == 4) {
		if (sscanf(argv[1], "%d", &num_sons) <= 0) {
			perror("The argument are int int int \n");
			exit(EXIT_FAILURE);
		}
		if (sscanf(argv[2], "%d", &num_reparto) <= 0) {
			perror("The argument are int int int \n");
			exit(EXIT_FAILURE);
		}
		if (sscanf(argv[3], "%d", &prior) <= 0) {
			perror("The argument are int int int \n");
			exit(EXIT_FAILURE);
		}
		if(num_sons<1){
			perror("Wrong number of sons, insert int from 0 to inf");
			exit(EXIT_FAILURE);
		}
		if(num_reparto<0 || num_reparto >2){
			perror("The division is wrong insert number from 0 to 2");
			exit(EXIT_FAILURE);
		}
		if(prior<0 || prior >4){
			perror("The priority is wrong insert number from 0 to 4");
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

	sock_id = 0;

	MSG_Q__main_bus = msgget(SERVER_KEY, 0);
	if (MSG_Q__main_bus < 0) {
		perror("The message queue don't exist");
		exit(1);
	}

	son = -1; i = 0;

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
			/*exit(0); Da togliere*/
		} else {
			wait(0);
			exit(0);
		}
	}

	return 0;
}
