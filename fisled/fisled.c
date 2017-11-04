#include "../fispi.h"
#include "fisled.h"

#include <unistd.h>
#include <stdlib.h>
#include <wiringPi.h>

struct fis_comm_context *ctx;

/* this function initialises the leds used by fispi
   the leds from 0 to MAXLEDS can be used */
int led_init(){
	int i;

	if (wiringPiSetup() == -1) {
		FISDEBUG("Error could not initialise wiringPi setup\n")
    		return -1 ;
  	}

	for (i=0;i<MAXLEDS;i++) {
		// here we need the code to initialize the leds
		FISDEBUG("initilizing LED %d\n", i)
  		pinMode (i, OUTPUT);
        }

	return(0);
}

int led_on(int led){
	if ((led<0) || (led>MAXLEDS)) return(-1);
	digitalWrite(led, 1);
	FISDEBUG("LED %d switched on\n", led)
	return(0);
}

int led_off(int led){
	if ((led<0) || (led>MAXLEDS)) return(-1);
	digitalWrite(led, 0);
	FISDEBUG("LED %d switched off\n", led)
	return(0);
}

int led_toggle(int led){
	if ((led<0) || (led>MAXLEDS)) return(-1);
	// here we need the code to talk to the led
  if(digitalRead(led) == 0)
  {
    digitalWrite(led,1);
  }
  else
  {
    digitalWrite(led,0);
  }
	FISDEBUG("LED %d toggled\n", led)
	return(0);
}

int main(int argc, char **argv){
        int c, r;

	char *arg[MAXCMDARGS] = { NULL };

	FISDEBUG(" -- fisled started --\n")
	FISDEBUG("listening on port %d\n", FISLEDPORT)

	if ((ctx = (struct fis_comm_context *) calloc(1, sizeof(*ctx))) == NULL) {
		printf("ERROR: allocate memory for context struct\n");
		exit(0);
  	}


	/* initialize sockets for communication with the fisled module */
	if(fis_comm_open(ctx, FISLEDPORT)<0){
		printf("ERROR: initialize sockets\n");
		exit(0);
	}

	if(led_init()<0){
		printf("ERROR: initialize leds\n");
		exit(0);
	}

	while(1){	
		r = fis_comm_select(ctx);
		if (r>0){ // socket r has data
			if(fis_comm_read(ctx,r) == 1) {
				fis_tokenize( &(ctx->buf[r][0]), arg, MAXCMDARGS);
				printf("command received: %s\n", arg[0]);
				if (arg[0]!=NULL) {
					if (!strcmp("test",arg[0])) {
						printf("TEST\n");
					}
					if (!strcmp("led",arg[0]) && (arg[1]!=NULL) && (arg[2]!=NULL)) {
						int led = atoi(arg[1]);
						if (!strcmp("on",arg[2])) {
							printf("LED %d ON\n", led);
							led_on(led);
						}
						if (!strcmp("off",arg[2])) {
							printf("LED %d OFF\n", led);
							led_off(led);
						}
						if (!strcmp("toggle",arg[2])) {
							printf("LED %d TOGGLE\n", led);
							led_toggle(led);
						}
					}
				}
			}
		}
		else if (r==0) { // a new client has connected 
			FISDEBUG("new client connected\n")
		}
		else if (r==-1) { // timeout
			//FISDEBUG("timeout\n")
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


