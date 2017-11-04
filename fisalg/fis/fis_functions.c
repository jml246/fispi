/* 
 * File:   rateofchange.h
 * Author: Jose
 *
 * Created on 13 April 2013, 02:15
 */

#include <stdio.h>

int floats_from_file(char* filename, int num_vals, float* out_vec)
{
    int i = 0;
    FILE *file;
    float tmp = 0;
    file = fopen(filename, "r");

    if(file == NULL) {
            printf("ERROR: file not found %s\n", filename);
	    return(-1);
    }

    for (i=0; i< num_vals; i++)
    {
        if ( fscanf(file, "%09f", &tmp) != 1)
        {
            printf("ERROR: Could not access or parse %s\n", filename);
        }
        out_vec[i] = tmp;
    }
    fclose(file);
    return(0);
}
float rateOfChange(float level, float prev_level, float max_level)
{
    float rate;
    rate = (prev_level - level) / max_level;
    if (rate <= 0.05 && rate >= -0.05)
    {
        rate = 0.0;
    }
    return rate;
}
void print_floats(float* vec, int num_vals)
{
    int i = 0;
    for (; i < num_vals - 1; i++)
    {
        printf("%.4f, ", vec[i]);
    }
    printf("%.4f\n", vec[num_vals - 1]);
    return;
}

