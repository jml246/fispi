/** @file fis.c 
 *
 * C code largely based on C that was auto-generated from MATLAB *.fis files using
 *  - http://makeproto.com/blog/?p=35 
 *  - http://www.makeproto.com/projects/fuzzy/matlab_arduino_FIST/index.php
 */

// NB the following might not be necessary on the Arduino
#include <stdio.h>
#include <stdbool.h>

#include "fis.h"

// NB the following might already be available on the Arduino
inline float max (float a, float b)
{
    return a > b ? a : b;
}
inline float min (float a, float b)
{
    return a > b ? b : a;
}

// Not Function
float _not(float x)
{
    return (float)(1.0-x);
}
// Generates the values in the range [min,max] with increments
// defined by 'step'.
void generate(float minimum, float maximum, float st, float* values,int length)
{
    float x = minimum;
    int i;
    for (i=0; i<length; i++,x+=st) {
        values[i] = x;
    }
}
// Sets all elements in the array with the same value
void set(float* values,int length,float value)
{
    int i;
    for (i=0; i<length; i++) {
        values[i] = value;
    }
}
// Returns the smallest element of an array
float min_array(float* x, int length)
{
    if (length <= 0) {
        return (float)0;
    }
    float m = x[0];
    int i=1;
    for (; i<length; i++) {
        m = min(m,x[i]);
    }
    return m;
}
// Returns the largest element of an array
float max_array(float* x, int length)
{
    if (length <= 0) {
        return (float)0;
    }
    float m = x[0];
    int i=1;
    for(; i<length; i++) {
        m = max(m,x[i]);
    }
    return m;
}

// NB the following wasn't auto-generated from the MATLAB *.FIS files;
// the original centroid function the C code generator produced was utter rubbish
float centroid(float* x, float* y, int length)
{
    int i = 0;
    float numer_sum = 0;
    float denom_sum = 0;
    float cent = 0;

    for (i=0; i<length; i++) {
        numer_sum += x[i] * y[i];
        denom_sum += y[i];
    }

    if (denom_sum != 0) {
        cent = numer_sum / denom_sum;
    }
    return cent;
}

// Trapezoidal Member Function
float trapmf(float x, float a, float b, float c, float d)
{
    return (float)max(min((x - a) / (b - a), min(1.0, (d - x) / (d - c))), 0);
}
// Triangular Member Function
float trimf(float x, float a, float b, float c)
{
    return (float)max(min((x - a) / (b - a), (c - x) / (c - b)), 0);
}
// Aggregation code for membership functions that require 3 setting values
void aggregate3(float* rules, int* ruleIndexes, int ruleIndexLength, float* xArr, float *yArr, int resultLength,_mffloat3 mf,float a,float b,float c)
{
    int i=0, j=0;
    for(; i<resultLength; i++) {
        for(j=0; j<ruleIndexLength; j++) {
            yArr[i] = max(yArr[i], min(rules[ruleIndexes[j]], mf(xArr[i],a,b,c)));
        }
    }
}

// Aggregation code for membership functions that require 4 setting values
void aggregate4(float* rules, int* ruleIndexes, int ruleIndexLength, float* xArr, float *yArr, int resultLength,_mffloat4 mf,float a,float b,float c,float d)
{
    int i=0, j=0;
    for(;i<resultLength;i++)
    {
        for(j=0;j<ruleIndexLength;j++) {
            yArr[i] = max(yArr[i],min(rules[ruleIndexes[j]],mf(xArr[i],a,b,c,d)));
        }
    }
}

/**
 * @brief Fuzzy Inference System for calculating sewer pump statuses from the relative wet well level and its derivative w.r.t. time.
 *
 * @param rate Rate of change of wet well level
 * @param level Relative level in wet well 
 * @param params Pointer to a tariff-specific, 10-element array of parameters that define the shape of the fuzzy logic membership functions
 * @param is_low_tariff Boolean to signify to use high-tariff or low-tariff fuzzy input parameters and rules
 * 
 * @return non-discretised FIS output
 */
float fis(float rate, float level, float* params, bool is_low_tariff)
{
    // Transformation of inputs to fuzzy inputs.
    float neg_big_rate_fi   = trapmf(rate, -0.8, -0.7, params[0], params[1]);
    float neg_small_rate_fi = trimf(rate, params[0], params[1], params[2]);
    float pos_big_rate_fi   = trapmf(rate, params[3], params[4], 0.7, 0.8);
    float zero_rate_fi      = trimf(rate, params[1], params[2], params[3]);
    float pos_small_rate_fi = trimf(rate, params[2], params[3], params[4]);

    float very_low_level_fi = trapmf(level, -0.225,-0.025, params[5], params[6]);
    float low_level_fi = trimf(level, params[5], params[6], params[7]);
    float middle_level_fi = trimf(level, params[6], params[7], params[8]);
    float high_level_fi = trimf(level, params[7], params[8], params[9]);
    float very_high_level_fi = trapmf(level, params[8], params[9], 1.025, 1.225);

    // 25 fuzzy rules
    float R[25]; 
    if (is_low_tariff == true) 
    {
        R[0] =  1   * min(neg_big_rate_fi, very_low_level_fi);
        R[1] =  1   * min(neg_big_rate_fi, low_level_fi);
        R[2] =  1   * min(neg_big_rate_fi, middle_level_fi);
        R[3] =  1   * min(neg_big_rate_fi, high_level_fi);
        R[4] =  1   * min(neg_big_rate_fi, very_high_level_fi);
        R[5] =  0.8 * min(neg_small_rate_fi, very_low_level_fi);
        R[6] =  1   * min(neg_small_rate_fi, low_level_fi);
        R[7] =  0.8 * min(neg_small_rate_fi, middle_level_fi);
        R[8] =  0.8 * min(neg_small_rate_fi, high_level_fi);
        R[9] =  0.8 * min(neg_small_rate_fi, very_high_level_fi);
        R[10] = 1   * min(zero_rate_fi, very_low_level_fi);
        R[11] = 1   * min(zero_rate_fi, low_level_fi);
        R[12] = 1   * min(zero_rate_fi, middle_level_fi);
        R[13] = 1   * min(zero_rate_fi, high_level_fi);
        R[14] = 1   * min(zero_rate_fi, very_high_level_fi);
        R[15] = 0.5 * min(pos_small_rate_fi, very_low_level_fi);
        R[16] = 0.5 * min(pos_small_rate_fi, low_level_fi);
        R[17] = 0.5 * min(pos_small_rate_fi, middle_level_fi);
        R[18] = 0.5 * min(pos_small_rate_fi, high_level_fi);
        R[19] = 0.5 * min(pos_small_rate_fi, very_high_level_fi);
        R[20] = 1   * min(pos_big_rate_fi, very_low_level_fi);
        R[21] = 1   * min(pos_big_rate_fi, low_level_fi);
        R[22] = 1   * min(pos_big_rate_fi, middle_level_fi);
        R[23] = 1   * min(pos_big_rate_fi, high_level_fi); 
        R[24] = 1   * min(pos_big_rate_fi, very_high_level_fi);
    }
    else // high tariff
    {
	    R[0]  = 1 * min(neg_big_rate_fi, very_low_level_fi);
	    R[1]  = 1 * min(neg_big_rate_fi, low_level_fi);
	    R[2]  = 1 * min(neg_big_rate_fi, middle_level_fi);
	    R[3]  = 1 * min(neg_big_rate_fi, high_level_fi);
	    R[4]  = 1 * min(neg_big_rate_fi, very_high_level_fi);
	    R[5]  = 1 * min(neg_small_rate_fi, very_low_level_fi);
	    R[6]  = 1 * min(neg_small_rate_fi, low_level_fi);
	    R[7]  = 1 * min(neg_small_rate_fi, middle_level_fi);
	    R[8]  = 1 * min(neg_small_rate_fi, high_level_fi);
	    R[9]  = 1 * min(neg_small_rate_fi, very_high_level_fi);
	    R[10] = 1 * min(zero_rate_fi, very_low_level_fi);
	    R[11] = 1 * min(zero_rate_fi, low_level_fi);
	    R[12] = 1 * min(zero_rate_fi, middle_level_fi);    
	    R[13] = 1 * min(zero_rate_fi, high_level_fi);
	    R[14] = 1 * min(zero_rate_fi, very_high_level_fi);
	    R[15] = 1 * min(pos_small_rate_fi, very_low_level_fi);
	    R[16] = 1 * min(pos_small_rate_fi, low_level_fi);
	    R[17] = 1 * min(pos_small_rate_fi, middle_level_fi);
	    R[18] = 1 * min(pos_small_rate_fi, high_level_fi);
	    R[19] = 1 * min(pos_small_rate_fi, very_high_level_fi);
	    R[20] = 1 * min(pos_big_rate_fi, very_low_level_fi);
	    R[21] = 1 * min(pos_big_rate_fi, low_level_fi);
	    R[22] = 1 * min(pos_big_rate_fi, middle_level_fi);
	    R[23] = 1 * min(pos_big_rate_fi, high_level_fi);
	    R[24] = 1 * min(pos_big_rate_fi, very_high_level_fi);
    }

    // Basic output processing
    int res = 100;
    float step;
    float xMin;
    float xMax;
    float x[res];
    float y[res];
    int value[25];
    xMin = -2;
    xMax = 2;
    step = (xMax - xMin) / (float)(res - 1);
    generate(xMin, xMax, step, x, res);
    set(y, res, 0);
    if (is_low_tariff == true)
    {
        value[0] = 20;
        value[1] = 21;
        aggregate3(R,value,2,x,y,res,trimf,-3,-2,-1);
        value[0] = 10;
        value[1] = 15;
        value[2] = 16;
        value[3] = 22;
        aggregate3(R,value,4,x,y,res,trimf,-2,-1,0);
        value[0] = 0;
        value[1] = 5;
        value[2] = 12;
        value[3] = 13;
        value[4] = 17;
        value[5] = 18;
        value[6] = 19;
        value[7] = 23;
        value[8] = 24;
        aggregate3(R,value,9,x,y,res,trimf,-1,0,1);
        value[0] = 1;
        value[1] = 2;
        value[2] = 6;
        value[3] = 7;
        value[4] = 8;
        value[5] = 11;
        aggregate3(R,value,6,x,y,res,trimf,0,1,2);
        value[0] = 3;
        value[1] = 4;
        value[2] = 9;
        value[3] = 14;
        aggregate3(R,value,4,x,y,res,trimf,1,2,3);
    }
    else // high tariff
    {
    	value[0] = 10;
	    value[1] = 15;
	    value[2] = 16;
	    value[3] = 20;
	    value[4] = 21;
	    aggregate3(R,value,5,x,y,res,trimf,-3,-2,-1);
	    value[0] = 22;
	    aggregate3(R,value,1,x,y,res,trimf,-2,-1,0);
	    value[0] = 0;
	    value[1] = 5;
	    value[2] = 6;
	    value[3] = 11;
	    value[4] = 12;
	    value[5] = 13;
	    value[6] = 17;
	    value[7] = 18;
	    value[8] = 19;
	    value[9] = 23;
	    value[10] = 24;
	    aggregate3(R,value,11,x,y,res,trimf,-1,0,1);
	    value[0] = 1;
	    value[1] = 2;
	    value[2] = 7;
	    value[3] = 8;
	    aggregate3(R,value,4,x,y,res,trimf,0,1,2);
	    value[0] = 3;
	    value[1] = 4;
	    value[2] = 9;
	    value[3] = 14;
	    aggregate3(R,value,4,x,y,res,trimf,1,2,3);
    }
    return centroid(x, y, res);
}

// Necessary to build DLL
#ifdef BUILD_DLL
EXPORT int main()
{
    return 0;
}
#endif
