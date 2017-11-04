/*
* Returns maximum depth from file depth.txt
*/

#define DEPTH_FILE "/home/pi/fispi/well_profile/depth.txt"

float getMaxDepth()
{
      printf("RUNNING getMaxDepth() \n");
      FILE *ptr_file;
      float readout;
      char buf[20];    //ten characters per line
      
      ptr_file =fopen(DEPTH_FILE,"r");

       //scan file but would return last value
      while(fgets(buf, 20, ptr_file) != NULL)
      {
       	 printf("fputs in while loop: %s \n",buf);
      }
      fclose(ptr_file);
      printf("Value fetched from max depth file is: %f \n", atof(buf));
      readout = atof(buf);
      return readout;
}