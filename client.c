/*
 * client.c
 *
 *  Created on: Jul 10, 2012
 *      Author: rommel
 */


#include "header_proj.h"

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
	richiesta->kindof_service = RADIOLOGY;
	richiesta->turn = 0;
	richiesta->price = 0;

	msgsnd(MSG_Q__main_bus, richiesta, sizeof(request), TOSRV);
	printf("richiesta inviata\n");

	printf("il mio pid %d\n", getpid());

	free(richiesta);

	return 0;
}
