/* 
 * File:   rateofchange.h
 * Author: Jose
 *
 * Created on 13 April 2013, 02:15
 */

int floats_from_file(char* filename, int num_vals, float* out_vec);

float rateOfChange(float level, float prev_level, float max_level);

void print_floats(float* vec, int num_vals);
