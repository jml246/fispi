#include "../fispi.h"
#include "fisalg.h"
#include <unistd.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <time.h>
#include "./fis/fis.h"
#include "./fis/fis_post_proc.h"
#include "./fis/fis_functions.h"

#define NPARAMS 	10
#define MAXDEPTH	5.0
#define MAXAVGSIZE 	20
#define MINVAL		1023 //The sensor registers 3.3V as empty, so 1023 would be the min val
#define MAXVAL          206 //digital val for 2V is around 620 which is  needed as gertboard max output via DAC is 3.3V (620 = 2V)
#define RANGE		817
//1023 is maximum 3.3V used

struct fis_comm_context *ctx;
double well0[MAXWELL0];
int well0_pos;

//added from fis_test.c under fis/old/testing dir
bool  is_low_tariff;
float curReading = 0.0;   	//the current reading
float prevReading = 0.0; 	//FIS needs two levels, a previous reading and a current reading
float du = 0.0;       		//rate of change
float relLevel = 0.0; 		//relative level between 0 and 1
float params[NPARAMS];

/*
 * decides which tariff to use for fis module
 * PreCondition: none
 * PostCondition: returns 1 for high, returns 0 for low tariff
 *
 */
int getTariff()
{
    time_t now=time(NULL);
    struct tm *tm=localtime(&now);
    int hour = tm->tm_hour;
    if(hour >= 0 && hour < 7)
    {
        return true;
    }
    else // high tariff
    {
        return false;
    }
}



/*
returns an absolute value
        used to calibrate readings from sensor reading and map to well depth
        eg (316/632) = 0.5; 0.5 * 10 = 5 metres
*/
float convertReading(int pReading) {
        float out;
        FISDEBUG("DEBUG convertReading Function\n")
        FISDEBUG("MAXVAL    is: %d \n", MAXVAL)
        FISDEBUG("Previous Reading (pReading)  is: %d \n", pReading)
        FISDEBUG("MAXDEPTH  is: %f \n", MAXDEPTH)

//        out = ((((float)pReading) / (float)MAXVAL)) * MAXDEPTH;

	out = (1- ((float)pReading - MAXVAL) / RANGE) * MAXDEPTH;
        FISDEBUG("convertReading output is: %f\n", out);
        return(out);
}

int alg_init() {
	is_low_tariff = getTariff(); 
	if (is_low_tariff) {
    		if (floats_from_file("l.txt", NPARAMS, params)<0) exit(0);
	}
	else {
    		if (floats_from_file("h.txt", NPARAMS, params)<0) exit(0);
	}
	well0_pos = 0;
}

int alg_run(){
	int nextval = 0;
	int sum_size = 0;
	int sum = 0;
	int i;
	int out;
	char buf[100];

	prevReading = curReading;
	curReading = 0.0;

	for(i=0;i<MAXAVGSIZE;i++) {
		nextval = well0_pos - 1;
		if (nextval<0) nextval = MAXWELL0 - 1;
		if (well0[nextval] >=0) {
			sum_size++;
			sum = sum + well0[nextval];
		}
	}
	FISDEBUG("sum is 	%d\n", sum)
	FISDEBUG("sum_size is	%d\n",sum_size)
	curReading = convertReading(sum/sum_size);
	FISDEBUG("Average reading is	%f\n", curReading)
        FISDEBUG("curReading after calculation:		%f\n", curReading)
    	du = rateOfChange(curReading, prevReading, MAXDEPTH);
	relLevel = curReading / MAXDEPTH;
     	out = discretise_fis_output(fis(du, relLevel, params, is_low_tariff));
	FISDEBUG("fis output: %d\n", out);
	//test to see if sensor is working
/*
out is the variable used for pump activation

-2 = two pumps off
-1 = switch one pump off
0 = no change
1 = switch one pump on
2 = switch two pumps on

*/
        if (out == -2){
                snprintf(buf, sizeof(buf), "fisio {pump 0 off}\n");
                if(fis_comm_send(ctx,0,buf)<0){
                        printf("ERROR: cannot send\n");
                }
                snprintf(buf, sizeof(buf), "fisio {pump 1 off}\n");
                if(fis_comm_send(ctx,0,buf)<0){
                        printf("ERROR: cannot send\n");
                }
        }

	if (out == -1){
		snprintf(buf, sizeof(buf), "fisio {pump 0 on}\n");
                if(fis_comm_send(ctx,0,buf)<0){
                	printf("ERROR: cannot send\n");
                }
		snprintf(buf, sizeof(buf), "fisio {pump 1 off}\n");
                if(fis_comm_send(ctx,0,buf)<0){
                	printf("ERROR: cannot send\n");
                }
	}
	if (out == 0){
	}
	if (out == 1){
		snprintf(buf, sizeof(buf), "fisio {pump 0 on}\n");
                if(fis_comm_send(ctx,0,buf)<0){
                	printf("ERROR: cannot send\n");
                }
		snprintf(buf, sizeof(buf), "fisio {pump 1 off}\n");
                if(fis_comm_send(ctx,0,buf)<0){
                	printf("ERROR: cannot send\n");
                }
	}
	if (out == 2){
		snprintf(buf, sizeof(buf), "fisio {pump 0 on}\n");
                if(fis_comm_send(ctx,0,buf)<0){
                	printf("ERROR: cannot send\n");
                }
		snprintf(buf, sizeof(buf), "fisio {pump 1 on}\n");
                if(fis_comm_send(ctx,0,buf)<0){
                	printf("ERROR: cannot send\n");
                }
	}


	return(0);
}

int add_well0_data(int data)
{
	well0[well0_pos]=data;
	well0_pos++;
	if (well0_pos==MAXWELL0) well0_pos=0;
	return(0);
}

int main(int argc, char **argv) {
  	int c, r;
	struct timeval t, t_exec;
	char *arg[MAXCMDARGS] = { NULL };
	char buf[500];
	int alg_run_timer = 0;

	FISDEBUG(" -- fisalg started --\n")
	FISDEBUG("listening on port %d\n", FISALGPORT)

	if ((ctx = (struct fis_comm_context *) calloc(1, sizeof(*ctx))) == NULL) {
		printf("ERROR: allocate memory for context struct\n");
		exit(0);
  	}


	/* initialize sockets for communication with the fisled module */
	if(fis_comm_open(ctx, FISALGPORT)<0){
		printf("ERROR: initialize sockets\n");
		exit(0);
	}

	if(alg_init()<0){
		printf("ERROR: initialize algorithm\n");
		exit(0);
	}

	gettimeofday( &t_exec, NULL );
        ++t_exec.tv_sec;

	while(1){	
		if(set_timer(&t,&t_exec,1)){
			alg_run_timer++;
			if (alg_run_timer==60) {
				alg_run();	
				alg_run_timer = 0;
			}
			snprintf(buf, sizeof(buf), "fisio {led %d toggle}\n", FISALGID+2);
                        if(fis_comm_send(ctx,0,buf)<0){
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
					if (!strcmp("well",arg[0]) && (arg[1]!=NULL) && (arg[2]!=NULL)) {
            					//here is where the well depth is received so level is the var to be used
						//example: well 0 20
						int   well = atoi(arg[1]);
						int  level = atof(arg[2]);
						FISDEBUG("WELL %d at level %d\n", well, level);
						if(well == 0) add_well0_data(level);
					
                        		if(level < 200)
                        		{
                                		snprintf(buf, sizeof(buf), "fisio {led %d toggle}\n",1);
				                if(fis_comm_send(ctx,0,buf)<0){
        		        		        printf("ERROR: cannot send\n");
                				}
                        		        snprintf(buf, sizeof(buf), "fisio {led %d toggle}\n",0);
                                		if(fis_comm_send(ctx,0,buf)<0){
                		        		printf("ERROR: cannot send\n");
                				}
						sleep(1);
                                		snprintf(buf, sizeof(buf), "fisio {led %d toggle}\n",1);
				                if(fis_comm_send(ctx,0,buf)<0){
        		        		        printf("ERROR: cannot send\n");
                				}
                        		        snprintf(buf, sizeof(buf), "fisio {led %d toggle}\n",0);
                                		if(fis_comm_send(ctx,0,buf)<0){
                                        		printf("ERROR: cannot send\n");
                                		}
                        		}}
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


