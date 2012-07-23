#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <stddef.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/un.h> //aggiunta dopo
#include <sys/socket.h> //aggiunta dopo

#include <fnmatch.h>
#include <limits.h>
#include <fcntl.h>
#include <signal.h>

//
//
// -------- LABORATORIO DI SISTEMI OPERATIVI - A.A. 2011-2012 ---------
//
// ________ Un sistema per la gestione delle prenotazioni ________
//
//
// ========= macro per la compilazione (condizionale) =========
//
#define DEBUG 1
//
// ========= macro per l'esecuzione ==========
//
// ----- livello di priorita` specificato dall'utente -----
#define ILLWAIT 0
#define NOHURRY 1
#define IMONHOLODAY 2
#define NEEDTOGO 3
#define CANTWAIT 4
//
// ----- possibili valori del kindof_service -----
#define OPHTHALMOLOGY 0
#define ORTHOPEDICS 1
#define RADIOLOGY 2
//
// =========== costanti di utilit�� generale ============
//
// ----- max lunghezza del file contenente la fattura -----
#define FILENAMELEN 32
//
#define PERMESSI (0666)
//
#define MSG_BUF_SIZE 1024
//
#define SERVER_KEY 0x1aaaaaa1
//
#define SEM_KEY 1490941
//
#define SHM_KEY 1491941
//
#define OPH_queue_KEY 1492941
#define ORT_queue_KEY 1493941
#define RAD_queue_KEY 1494941
//
#define INVOICE_PATH "invoices/"

#define TRUE 1
#define FALSE 0
//variabili per definire i codici della coda
#define TOSRV 10
#define TOCLI 11
#define TORES 12

//definizione della struttura
typedef struct _msgbuf{
	long mtype; //
	int  clientId; // utilizzato dal client, per indicare al server
		// a chi spedire la risposta
	short priority; // in {0,1,2,3,4} 0: ILLWAIT, ..., 4: CANTWAIT
	int kindof_service; // in {0,1,2} cfr. #define sopra
	int turn; // numero progressivo con cui il client sar�� servito
	int price; // prezzo che il client dovr�� pagare: (a) un tanto fisso per
		// ogni prestazione, pi�� (b) una quota in funzione dell'urgenza
		// sia (a) sia (b) sono fissati tramite il file di configurazione
} request, response, reservation;

// suggerimento
// request: client->server
// response: server->client
// reservation: server->coda_singolo_reparto


