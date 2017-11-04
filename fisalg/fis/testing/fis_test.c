#include <stdio.h>


#include <stdlib.h>

#include <stdbool.h>

#include <string.h>


#include "../fis.h"

#include "../fis_post_proc.h"


#define NPARAMS 10

#define NLEVELS 1000



void floats_from_file(char* filename, int num_vals, float* out_vec)

{

    int i = 0;

    FILE *file;

    float tmp = 0;


    file = fopen(filename, "r");

    for (i=0; i< num_vals; i++)
    {

        if ( fscanf(file, "%09f", &tmp) != 1)
        {

            printf("ERROR: Could not access or parse %s\n", filename);

        }

            out_vec[i] = tmp;

    }

    fclose(file);

    return;

}



void print_floats(float* vec, 	    int num_vals)

{

    int i = 0;

    for (; i < num_vals - 1; i++)
    {

        printf("%.4f, ", vec[i]);

    }

    printf("%.4f\n", vec[num_vals - 1]);

    return;

}



float rate_of_change(float level, float prev_level, float max_level)

{

    float rate;


    rate = (prev_level - level) / max_level;


    if (rate <= 0.05 && rate >= -0.05)
    {

        rate = 0.0;

    }


    return rate;

}



#ifndef BUILD_DLL
int main(int argc, char* argv[])

{

    float params[NPARAMS];

    float levels[NLEVELS];

    int t;

    float rate;

    float max_depth = 1.71;

    float rel_level;

    float fis_output;

    int disc_fis_output;

    bool is_low_tariff;

    
    if (argc != 4)
        {

            printf("Usage: <progname> <params>.txt <levels>.txt <low|high>\n");

            return -1;

        }


    floats_from_file(argv[1], NPARAMS, params);

    floats_from_file(argv[2], NLEVELS, levels);

    is_low_tariff = strcmp(argv[3], "high") ? false : true;


    printf("abs lev, rel lev, rate_of_change, fis_output, discretise_fis_output\n");

    
    for (t = 1; t < NLEVELS; t++) 
    {

        rel_level = levels[t] / max_depth;

        rate = rate_of_change(levels[t], levels[t-1], max_depth);


        fis_output = fis(rate, rel_level, params, is_low_tariff);

        disc_fis_output = discretise_fis_output(fis_output);


        printf("%f, %f, %f, %f, %d\n", levels[t], rel_level, rate, fis_output, disc_fis_output);

    }


    return 0;

}

#endif
