#include "../fispi.h"
#include "fiscore.h"

#include <unistd.h>
#include <stdlib.h>

struct fis_comm_context *ctx;

int fisio  = -1;
int fiscom = -1;
int fisalg = -1;

int fisio_connected(){
	if((fisio > 0) && (ctx->FD[fisio]>=0)) return(1);
	fisio = -1;
	return (0);
}

int fisalg_connected(){
	if((fisalg > 0) && (ctx->FD[fisalg]>=0)) return(1);
	fisalg = -1;
	return (0);
}

int main(int argc, char **argv){
        int c, r;
	struct timeval t, t_exec;
	char *arg[MAXCMDARGS] = { NULL };
	char buf[500];

	FISDEBUG(" -- fiscore started --\n")
	FISDEBUG("listening on port %d\n", FISCOREPORT)

	if ((ctx = (struct fis_comm_context *) calloc(1, sizeof(*ctx))) == NULL) {
		printf("ERROR: allocate memory for context struct\n");
		exit(0);
  	}


	/* initialize sockets for communication with the fiscore module */
	if(fis_comm_open(ctx, FISCOREPORT)<0){
		printf("ERROR: initialize sockets\n");
		exit(0);
	}

	gettimeofday( &t_exec, NULL );
	++t_exec.tv_sec;
	while(1){	
		if(set_timer(&t,&t_exec,1)){
			snprintf(buf, sizeof(buf), "led %d toggle\n", FISCOREID+2);
			if(fis_comm_send(ctx,fisio,buf)<0){
                                printf("ERROR: cannot send\n");
                        }

		}
		if(!fisio_connected()){
			FISDEBUG("fisio module not connected, connecting ...\n");
			if((fisio=fis_comm_connect(ctx, FISIOPORT))<0){
				printf("ERROR: cannot connect to fisio on port %d\n", FISIOPORT);
			}
		}
		if(!fisalg_connected()){
			FISDEBUG("fisalg module not connected, connecting ...\n");
			if((fisalg=fis_comm_connect(ctx, FISALGPORT))<0){
				printf("ERROR: cannot connect to fisalg on port %d\n", FISALGPORT);
			}
		}

		r = fis_comm_select(ctx,&t);
		if (r>0){ // socket r has data
			if(fis_comm_read(ctx,r) == 1) {
				fis_tokenize( &(ctx->buf[r][0]), arg, MAXCMDARGS);
				FISDEBUG("command received: %s\n", arg[0]);
				if (arg[0]!=NULL) {
					if (!strcmp("test",arg[0])) {
						FISDEBUG("TEST\n");
					}
					if (!strcmp("fisio",arg[0]) && (arg[1]!=NULL)) {
						FISDEBUG("data for fisio: %s\n", arg[1]);
						snprintf(buf,500,"%s\n",arg[1]);
						fis_comm_send(ctx,fisio,buf);
					}
					if (!strcmp("fiscom",arg[0]) && (arg[1]!=NULL)) {
						FISDEBUG("data for fiscom: %s\n", arg[1]);
						snprintf(buf,500,"%s\n",arg[1]);
						fis_comm_send(ctx,fiscom,buf);
					}
					if (!strcmp("fisalg",arg[0]) && (arg[1]!=NULL)) {
						FISDEBUG("data for fisalg: %s\n", arg[1]);
						snprintf(buf,500,"%s\n",arg[1]);
						fis_comm_send(ctx,fisalg,buf);
					}
				}
			}
		}
		else if (r==0) { // a new client has connected 
			FISDEBUG("new client connected\n")
		}
		else if (r==-1) { // timeout 
			// this is handled with set_timer
		}
		else {	// an error
			printf("ERROR: select on sockets\n");
			break;
		}
	}

	if(fis_comm_close(ctx)<0){
		printf("ERROR: closing sockets\n");
		exit(0);
	}

	exit(0);
}


