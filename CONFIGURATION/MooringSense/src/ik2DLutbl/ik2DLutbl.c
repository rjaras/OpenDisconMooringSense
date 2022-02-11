/*
  Copyright (C) 2021-2022 IKERLAN

  This file is part of OpenDiscon.

  OpenDiscon is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  OpenDiscon is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with OpenDiscon. If not, see <http://www.gnu.org/licenses/>.
*/

 /**
  * @file ik2DLutbl.c
  *
  * @brief Class ik2DLutbl implementation
  */

  /* @cond */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ik2DLutbl.h"


/**
 * "private method" to check that the coordinate values are ascending
 * for x and y dimensions
 */
const char *ik2DLutbl_checkData(ik2DLutbl *self) {
    int i;

    for (i = 1; i < self->xnumpoints; i++) {
        if (self->xvalues[i] < self->xvalues[i - 1]) {
            return "bad data: X not ascending";
        }
    }

    for (i = 1; i < self->ynumpoints; i++) {
        if (self->yvalues[i] < self->yvalues[i - 1]) {
            return "bad data: Y not ascending";
        }
    }

    return "";
}


/**
 * "private method" to initialize instance values after reading a surface file
 * @param obj new instance
 * @param dims number of dimensions
 * @param ndata number of data points per dimension (always 1 for the last dimension, which may be omitted if copy!=0)
 * @param data data points, specified as follows:
 * @li surface coordinate values (x0) for the first dimension, in ascending order
 * @li surface coordinate values (x1) for the second dimension, in ascending order
 * @li ...
 * @li surface coordinate values (y) for the last dimension, in the following order:
 *     for i = number of different x0 values
 *         for j = number of different x1 values
 *             for k = number of different x2 values
 *                 y( x0_i, x1_j, x2_k, ... )
 */
const char * ik2DLutbl_new(ik2DLutbl **obj, int dims, int *ndata, double *data) {
    ik2DLutbl *newobj;
    long numel = 1;
    int i, totalPoints;
    double *x_vals;
    double *y_vals;
    double *z_vals; 
    const char *err = "";
    
    /*check dims*/
    if (dims != 3) {
        *obj = NULL;
        return "bad dims: expecting 2D table";
    }
    /*check ndata*/
    for (i = 0; i < dims - 1; i++) {
        if (ndata[i] < 1) {
            *obj = NULL;
            return "bad ndata";
        }
    }
    if (ndata[dims - 1] != 1) {
        *obj = NULL;
        return "bad ndata";
    }

    /*allocate new object*/
    newobj = (ik2DLutbl *) malloc(sizeof(ik2DLutbl));
    
    /*remember number of points per dimension*/
    newobj->xnumpoints = ndata[0];
    newobj->ynumpoints = ndata[1];

    for (i = 0; i < dims - 1; i++) numel *= ndata[i];
    for (i = 0; i < dims - 1; i++) numel += ndata[i];
        
    /*allocate data array*/
    x_vals = (double *) malloc(sizeof(double) * ndata[0]);
    y_vals = (double *) malloc(sizeof(double) * ndata[1]);
    totalPoints = ndata[0] * ndata[1];
    z_vals = (double *) malloc(sizeof(double) * totalPoints);
    for (i = 0; i < numel; i++) {
        if (i < ndata[0]) {
            x_vals[i] = data[i];
        } else if (i >= ndata[0] && i < (ndata[0] + ndata[1])) {
            y_vals[i - ndata[0]] = data[i];
        } else {
            z_vals[i - ndata[0] - ndata[1]] = data[i];
        }
    }

    newobj->xvalues = x_vals;
    newobj->yvalues = y_vals;
    newobj->table = z_vals;

    /*check data*/
    err = ik2DLutbl_checkData(newobj);
    if (strcmp(err, "")) {
        ik2DLutbl_delete(newobj);
        *obj = NULL;
        return err;
    }
    
    /*point at the new object*/
    *obj = newobj;
    
    /*return error message*/
    return err;
}


void ik2DLutbl_delete(ik2DLutbl *self) {
    free(self->xvalues);
    free(self->yvalues);
    free(self->table);
    free(self);
}


const char *ik2DLutbl_init(ik2DLutbl **obj, const char *filename) {
    ik2DLutbl *newobj;
    long numel = 1;
    int i;
    FILE *f;
    int dims;
    int *ndata;
    double *data;
    const char *err;

    f = fopen(filename, "rb"); /*open the file for reading*/
    fread(&dims, sizeof(int), 1, f); /*read dims*/
    ndata = (int *) malloc(sizeof(int) * dims);/*allocate ndata*/
    fread(ndata, sizeof(int), dims - 1, f); /*read ndata*/
    ndata[dims - 1] = 1;
    
    /*allocate data*/
    for (i = 0; i < dims - 1; i++) numel *= ndata[i];
    for (i = 0; i < dims - 1; i++) numel += ndata[i];
    data = (double *) malloc(sizeof(double) * numel);
    
    /*read data*/
    fread(data, sizeof(double), numel, f);
    
    /*close the file*/
    fclose(f);
    f = NULL;
        
    /*pass dims, ndata and data to constructor*/
    err = ik2DLutbl_new(&newobj, dims, ndata, data);
    
    /*point at new instance*/
    *obj = newobj;
   
    /*return error string*/
    return err;
}


double ik2DLutbl_eval(ik2DLutbl *self, double ix, double iy) {
    /* The upper X and Y coordinates of the interpolation box */
    int i, j;

    int iaux, jaux, k;
    double result;

    /* ------------------------------------------------------------------------ */
    /* X axis coordinate lookup: find the index of the table point which is 
    closest to x from above */    
    i = self->xnumpoints - 1; /*put upper limit at the end */    
    iaux = 1; /*put lower limit at 1 */

    /*check for extrapolation cases */
    if (ix <= self->xvalues[1]) i = 1;
    if (ix >= self->xvalues[self->xnumpoints - 1]) iaux = self->xnumpoints - 1;
    
    while (1) {
        /*see if we already have a solution */
        if (i <= iaux + 1) break;
        /*put middle point in the middle, or 0.5 lower if the middle is not int */
        k = (int) (floor((i + iaux) / 2 + 0.1));
        /*choose sides */
        if (ix <= self->xvalues[k]) i = k;
        if (ix >= self->xvalues[k]) iaux = k;
    }

    /* ------------------------------------------------------------------------ */
    /* Y axis coordinate lookup: find the index of the table point which is 
    closest to x from above */    
    j = self->ynumpoints - 1; /*put upper limit at the end */
    jaux = 1; /*put lower limit at 1 */

    /*check for extrapolation cases */
    if (iy <= self->yvalues[1]) i = 1;
    if (iy >= self->yvalues[self->ynumpoints - 1]) jaux = self->ynumpoints - 1;

    while (1) {
        /*see if we already have a solution */
        if (j <= jaux + 1) break;
        /*put middle point in the middle, or 0.5 lower if the middle is not int */
        k = (int) (floor((j + jaux) / 2 + 0.1));
        /*choose sides */
        if (iy <= self->yvalues[k]) j = k;
        if (iy >= self->yvalues[k]) jaux = k;
    }

    /* ------------------------------------------------------------------------ */
    /* 2-D interpolation */

    /* At this point we know that the input X value is between xvalues[i-1] and
    xvalues[i] and that the input Y value is between yvalues[j-1] and 
    yvalues[j]. Therefore we have a rectangle in which we need to interpolate.
    
    To do the interpolation, we first interpolate between column i and column i+1 
    on the upper row j.  Then, we interpolate between the same xvalues on row j+1.  
    Finally, we interpolate vertically between the two rows based on the input Y 
    value.

       row0 is the upper row data and row1 is the lower (higher subscript) row
       data. */
    
    const double *row0 = &self->table[(j - 1) * self->xnumpoints];
    const double *row1 = &row0[self->xnumpoints];
    /* Difference between the two adjacent column values */
    double i_delta = self->xvalues[i] - self->xvalues[i - 1];
    /* Difference between the two adjacent row values */
    double j_delta = self->yvalues[j] - self->yvalues[j - 1];
    /* Interpolation results for the upper and lower rows */
    double o0, o1, o_low, o_delta;

    /* Interpolate between xvalues in the upper and lower row. */
    /* Interpolate the upper row */
    o_low = row0[i - 1]; /* Row value at low column # */
    o_delta = row0[i] - row0[i - 1]; /* Difference from next column */
    o0 = o_low + ((ix - self->xvalues[i - 1]) * o_delta) / i_delta;

    /* Interpolate the lower (higher subscript) row */
    o_low = row1[i - 1]; /* Row value at low column # */
    o_delta = row1[i] - row1[i - 1]; /* Difference from next column */
    o1 = o_low + ((ix - self->xvalues[i - 1]) * o_delta) / i_delta;

    /* interpolate between the two row interpolation results from earlier. */
    return o0 + ((iy - self->yvalues[j - 1]) * (long) (o1 - o0)) / j_delta;
}
