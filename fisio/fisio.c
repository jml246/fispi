#include "../fispi.h"
#include "fisio.h"

#include <unistd.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <gertboard.h>
#include <stdlib.h>
#include <stdbool.h>
#define WATCHDOG !(digitalRead(4))
#define TOGGLE !(digitalRead(5))
struct fis_comm_context *ctx;

/* this function initialises the leds and other ports used by fispi
   the leds from 0 to MAXLEDS can be used */
int io_init(){
	int i;

	if (wiringPiSetup() == -1) {
		FISDEBUG("Error could not initialise wiringPi setup\n")
    		return -1 ;
  	}
  if(gertboardSPISetup() < 0)
  {
     printf("Error could not initialise gertboard SPI setup \n");
     return 1;
  } 
	for (i=0;i<MAXLEDS;i++) {
		// here we need the code to initialize the leds
		FISDEBUG("initilizing LED %d\n", i)
  		pinMode (i, OUTPUT);
		digitalWrite(i,0);
        }
        pinMode(4, INPUT); //switch for watchdog
        pinMode(5, INPUT); //toggle AI - comments - in doubt
        return(0);
}

int pump_on(int pump){
	if ((pump<0) || (pump>=MAXPUMPS)) return(-1);
        gertboardAnalogWrite(pump,255); //2.04 volt
	FISDEBUG("PUMP %d switched on (2.04V)\n", pump)
	return(0);
}

int pump_off(int pump){
	if ((pump<0) || (pump>=MAXPUMPS)) return(-1);
        gertboardAnalogWrite(pump,0); //0 volt
	FISDEBUG("PUMP %d switched off (0V)\n", pump)
	return(0);
}

int led_on(int led){
	if ((led<0) || (led>=MAXLEDS)) return(-1);
	digitalWrite(led, 1);
	FISDEBUG("LED %d switched on\n", led)
	return(0);
}

int led_off(int led){
	if ((led<0) || (led>=MAXLEDS)) return(-1);
	digitalWrite(led, 0);
	FISDEBUG("LED %d switched off\n", led)
	return(0);
}

int led_toggle(int led){
	if ((led<0) || (led>=MAXLEDS)) return(-1);
	digitalWrite(led, digitalRead(led) ^ 1); 
	FISDEBUG("LED %d toggled\n", led)
	return(0);
}

int main(int argc, char **argv){
        int c, r;
	struct timeval t, t_exec;
	char *arg[MAXCMDARGS] = { NULL };
	char cmd[100];
	bool runOnce = true;
	FISDEBUG(" -- fisled started --\n")
	FISDEBUG("listening on port %d\n", FISIOPORT)

	if ((ctx = (struct fis_comm_context *) calloc(1, sizeof(*ctx))) == NULL) {
		printf("ERROR: allocate memory for context struct\n");
		exit(0);
  	}


	/* initialize sockets for communication with the fisled module */
	if(fis_comm_open(ctx, FISIOPORT)<0){
		printf("ERROR: initialize sockets\n");
		exit(0);
	}

	if(io_init()<0){
		printf("ERROR: initialize leds\n");
		exit(0);
	}

	gettimeofday( &t_exec, NULL );
        ++t_exec.tv_sec;

	while(1){
		if(!WATCHDOG){runOnce = true;}
		FISDEBUG("TOGGLE IS %d\n", TOGGLE);
		//if(TOGGLE){
		//	system("sudo /home/pi/fispi/restart.sh");
		//}
		if(set_timer(&t,&t_exec,1)){
			led_toggle(FISIOID+2);
                        //read_port(0);
			//expect to send something like: fisalg {well 0 123}
      			r = gertboardAnalogRead(0);  //arg is channel
			snprintf(cmd, sizeof(cmd), "fisalg {well 0 %d}\n", r);
      			FISDEBUG("sending data reading to all connected modules : %s\n", cmd);
      			if(fis_comm_send(ctx,0,cmd)<0){
              			printf("ERROR: cannot send\n");
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
					if (!strcmp("led",arg[0]) && (arg[1]!=NULL) && (arg[2]!=NULL) && !WATCHDOG) {
						int led = atoi(arg[1]);
						if (!strcmp("on",arg[2])) {
							FISDEBUG("LED %d ON\n", led);
							led_on(led);
						}
						if (!strcmp("off",arg[2])) {
							FISDEBUG("LED %d OFF\n", led);
							led_off(led);
						}
						if (!strcmp("toggle",arg[2])) {
							FISDEBUG("LED %d TOGGLE\n", led);
							led_toggle(led);
						}
					}
					if (!strcmp("pump",arg[0]) && (arg[1]!=NULL) && (arg[2]!=NULL) && !WATCHDOG) {
						int pump = atoi(arg[1]);
						if (!strcmp("on",arg[2])) {
							FISDEBUG("PUMP %d ON\n", pump);
							pump_on(pump);
							led_on(pump);
						}
						if (!strcmp("off",arg[2])) {
							FISDEBUG("PUMP %d OFF\n", pump);
							pump_off(pump);
							led_off(pump);
						}	
					}
					if(WATCHDOG && runOnce){
						FISDEBUG("WATCHDOG IS ON...");
						pump_off(0);
						pump_off(1);
						led_off(0);
						led_off(1);
						runOnce = false;
					}
				}
			}
		}
		else if (r==0) { // a new client has connected 
			FISDEBUG("new client connected\n")
		}
		else if (r==-1) { // timeout
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
