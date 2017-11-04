/*
FIS LINUX READ PIPE
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../fis.h"
#include "../fis_post_proc.h"
#include <pthread.h>
#include "../tariff.h"
#include <fcntl.h>
#include <sys/stat.h>
#include "../fis_functions.h"
#include "../../output/led_activity.h"
#include "../../well_profile/depth.h"  //use of getMaxDepth()


#if defined (__CYGWIN32__) || defined (WIN32)
   # define OPSYS 1
   #include "../win.h"
#endif
#ifdef linux
   #define OPSYS 2
  #include "../linux.h"
#endif

/************/
//definitions/ 
/************/
#define NPARAMS 10
// depth needs to be in separate file
//#define MAX_DEPTH  15.0 // suppose it is metres
//pipe value buffer
#define MAX_BUF 10
#define THEPIPE  "/tmp/fis"
float MAX_DEPTH;

int main()//int argc, char* argv[])
{
    if(OPSYS == 1)
    {
        readPipeWin();
    }
    else
    {
        MAX_DEPTH = getMaxDepth();
        readPipeLinux();
    }
    return 0;
}

 
int readPipeLinux()
{
    float curReading = 0.0;
    float prevReading = 0.0;
    float du = 0.0;
    float relLevel = 0.0;
    float params[NPARAMS];
    char buf[MAX_BUF];
    int tariff = getTariff();
    
    char booleanTar; //tariff
    int fd;
    char *myFifo = THEPIPE;
    /* create the FIFO (named pipe)
     * LEDS */
    //clear leds
    pumpFunction(-2);
    mkfifo(myFifo, 0666);
    printf("hPipe=%p\n", myFifo);
    //open pipe file
    printf("opening pipe \n");
    fd = open(myFifo, O_RDONLY);
    if (fd < 0)
    {
       printf("Error opening pipe \n");
       return 1;
    }

    while(1)
    {
    printf("Getting depth from variable: %f \n", MAX_DEPTH);
        int disOut = 0; // no change to pumps
        printf("Waiting for sensor aggregate result \n");
        //usleep(40000000);
        read(fd, buf, MAX_BUF);
        
	      printf("read [%s]\n", buf);
        //get tariff type parameters
        if(tariff == 0)
        {
           floats_from_file("l.txt", NPARAMS, params);
           booleanTar = "low";
           printf("Setting low tariff \n");
        }
        else //its high
        {
           floats_from_file("h.txt", NPARAMS, params);
           booleanTar = "high";
           printf("Setting high tariff \n");
        }
        prevReading = curReading;
        curReading = atof(buf);
        printf("Current reading is %f \n", curReading);
        printf("Previous reading is %f \n", prevReading);
        printf("Max depth is: %f\n ", MAX_DEPTH);
        printf("relative percentage is : %f\n ", (curReading/MAX_DEPTH));
        du = rateOfChange(curReading, prevReading, MAX_DEPTH);
        relLevel = curReading / MAX_DEPTH;
        printf("Relative Level %f \n", relLevel);
        onFis(); //make the fis LED blink
        //discretise_fis_output is the defuzzification stage to provide a crisp output which can be 5 values
        // -2. -1, 0, 1, 2
        // outputs are for pump activations/deactivations as there are 2 pumps
        disOut = discretise_fis_output(fis(du, relLevel, params, booleanTar));
        offFis();
        
        printf("Output is: %d \n", disOut);
        pumpFunction(disOut);
        
    }
    close(fd);
    return 1;
}


