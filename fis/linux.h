/* 
 * File:   linux.h
 * Author: Jose
 *
 * Created on 13 April 2013, 02:08
 * FIS LINUX READ PIPE
 */





/*
Here we would do a system call to pump_action.c with arguments to 
activate or deactivate pumps by sending a write command to the
GPIO pins 
*/
void pumpFunction(int pPumpAction){
    int mPumpAction = pPumpAction;
    if(mPumpAction == -2)
    {
        //switch two pumps off
        offPump1();
        offPump2();
    }
    else if(mPumpAction == -1)
    {
        //switch one pump off and keep one pump on
        onPump1();
        offPump2();
    }
    else if(mPumpAction == 0)
    {
      //no change
    }
    else if(mPumpAction == 1)
    {
       //switch one pump on
       onPump1();
       offPump2();
    }
    else if(mPumpAction == 2)
    {
        //switch both pumps
        onPump1();
        onPump2();
    }
}