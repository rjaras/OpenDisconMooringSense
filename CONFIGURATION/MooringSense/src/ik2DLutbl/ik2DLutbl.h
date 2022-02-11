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
 * @file ik2DLutbl.h
 *
 * @brief Class ik2DLutbl interface
 */

#include <stdlib.h>

#ifndef IK2DLUPTBL_H
#define IK2DLUPTBL_H

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * @struct ik2DLutbl
     * @brief 2D look-up table
     *
     * Instances of this class are implementations of surfaces.
     * Linear interpolation and extrapolation are applied.
     *
     * @par Inputs
     * @li point of evaluation, specify via @link ik2DLutbl_eval @endlink
     *
     * @par outputs
     * @li evaluation result, returned by @link ik2DLutbl_eval @endlink
     *
     * @par Unit block
     *
     * @image html ik2DLutbl_unit_block.svg
     *
     * @par Block diagram
     *
     * @image html ik2DLutbl_block_diagram.svg
     *
     * @par Methods
     * @li @link ik2DLutbl_init @endlink get new instance from file
     * @li @link ik2DLutbl_delete @endlink delete instance
     * @li @link ik2DLutbl_eval @endlink evaluate surface coordinate
     */
    typedef struct ik2DLutbl ik2DLutbl;
    struct ik2DLutbl {
        /**
         * Private members
         */
        /* @cond */
        int xnumpoints; /** Number of columns (X values) in the table*/
        int ynumpoints; /** Number of rows (Y values) in the table*/
        double *xvalues; /** X-axis input values list. */
        double *yvalues; /* Y-axis input values list. */
        double *table; /* table data, array of <code>xvalues</code>X<code>yvalues</code>, arranged in rows.  For example, <code>table[1]</code> is the second y=0 and x=1. */
        /* @endcond */
    };
    
    /**
     * get new instance from file
     * @param obj new instance
     * @param filename path to binary file containing:
     * @li number of dimensions, int32
     * @li number of data points per dimension, int32 (always 1 for the last dimension, which is omitted)
     * @li data points, specified as in @link ikSurf_new @endlink, float64
     * @return error message
     * @li "bad file <filename>": could not open file
     * @li others: see @link ikSurf_new @endlink
     */
    const char *ik2DLutbl_init(ik2DLutbl **obj, const char *filename);

    /**
     * delete instance
     * @param self instance
     */
    void ik2DLutbl_delete(ik2DLutbl *self);

    /**
     * This function performs a 2-D table lookup with interpolation/extrapolation.
     * @param self instance
     * @param ix known X coordinate value
     * @param iy known Y coordinate value
     * @return surface value interpolated/extrapolated
     */
    double ik2DLutbl_eval(ik2DLutbl *self, double ix, double iy);


#ifdef __cplusplus
}
#endif

#endif /* IK2DLUPTBL_H */
