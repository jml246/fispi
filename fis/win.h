/* 
 * File:   win.h
 * Author: Jose
 *
 * Created on 13 April 2013, 02:08
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "fis.h"
#include "fis_post_proc.h"
#include <pthread.h>
#include "tariff.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include"fis_functions.h"
#include <windows.h>

/************/
//definitions/ 
/************/
#define NPARAMS 10
// depth needs to be in separate file
#define MAX_DEPTH  15 // suppose it is metres
//pipe value buffer
#define MAX_BUF 100
#define  THE_PIPE "\\\\.\\pipe\\FIS"

/*
 * pipeWin function
  
 */
int readPipeWin()
{
    float curReading = 0.0;
    float prevReading = 0;
    float du;
    int relLevel = 0;
    float params[NPARAMS];
    char buf[MAX_BUF];
    HANDLE      hPipe;
    DWORD	dwBytesRead;   
    hPipe = CreateNamedPipe(THE_PIPE, 	// Name
		PIPE_ACCESS_DUPLEX,
                PIPE_WAIT,
                1,
                1024,
                1024,
                120 * 1000,
                NULL);
    	if (hPipe == INVALID_HANDLE_VALUE)
	{
		printf("Could not create the pipe\n");
		exit(1);
	}
	printf("hPipe=%p\n", hPipe);

	printf("connect...\n");
	ConnectNamedPipe(hPipe, NULL);
	printf("...connected\n");
   while(1)
    {
    int disOut = 0; // no change to pumps
        //sleep(40);
        int tariff = getTariff();
        char booleanTar;
        
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
        //open pipe file
        printf("opening pipe \n");
        //fd = open(myFifo, O_RDONLY);
        //read(fd, buf, MAX_BUF);
        //close(fd);

        
 	//for (;;){
	
        if (ReadFile(hPipe, buf, sizeof(buf), &dwBytesRead, NULL) == FALSE)
        {
                printf("ReadFile failed -- probably EOF\n");
                break;
        }
        
	buf[dwBytesRead] = '\0';
	printf("read [%s]\n", buf);
          
//        printf("closed reading pipe \n");
//        printf("Read buffer %c \n", buf);

       prevReading = curReading;
       curReading = atof(buf);
        printf("Current reading is %f \n", curReading);
        printf("Previous reading is %f \n", prevReading);
        du = rateOfChange(curReading, prevReading, MAX_DEPTH);
        relLevel = curReading / MAX_DEPTH;
        
        disOut = discretise_fis_output(fis(du, relLevel, params, booleanTar));
        printf("Output is: %d \n", disOut);
        //}

    }
        DisconnectNamedPipe(hPipe);
        CloseHandle(hPipe);
    return 1;
}
