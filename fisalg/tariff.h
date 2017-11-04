/* 
 * File:   tariff.h
 * Author: Jose
 * LT starts at 00:00 and ends at 7:00 am. HT starts at 7:00 am and ends at 00:00.
 * Created on 18 March 2013, 19:30
 */
#include<time.h>
#include<stdio.h>

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
    printf("getting tariff \n");
    int tariff = 0;


    int hour = tm->tm_hour;
    printf("Hour: %d \n", hour);

    if(hour >= 0 && hour < 7)
    {
        tariff = 0;
    }
    else // high tariff
    {
        tariff = 1;
    }
     printf("Returning tariff \n");
    return tariff;
}

