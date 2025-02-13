/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-04-29
 * Description : refocus deconvolution matrix implementation.
 *
 * Copyright (C) 2005-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

#ifndef MATRIX_H_INCLUDED
#define MATRIX_H_INCLUDED

// C ++ includes.

#include <cstdio>

namespace DigikamImagesPluginCore
{
    
/**
* CMat:
* @radius: Radius of the matrix.
*
* Centered matrix. This is a square matrix where
* the indices range from [-radius, radius].
* The matrix contains (2 * radius + 1) ** 2 elements.
*
**/
typedef struct
{
    int     radius;                // Radius of the matrix 
    int     row_stride;            // Size of one row = 2 * radius + 1 
    double *data;                  // Contents of matrix 
    double *center;                // Points to element with index (0, 0) 
}
CMat;

/**
* Mat:
* @rows: Number of rows in the matrix.
*
* Normal matrix type. Indices range from
* [0, rows -1 ] and [0, cols - 1].
*
**/
typedef struct
{
    int     rows;                  // Number of rows in the matrix 
    int     cols;                  // Number of columns in the matrix 
    double *data;                  // Content of the matrix 
}
Mat;

class RefocusMatrix
{

public: 

    static void fill_matrix (CMat * matrix, const int m, double f (int, int, double), const double fun_arg);
    
    static void fill_matrix2 (CMat * matrix, const int m, 
                              double f (const int, const int, const double, const double), 
                              const double fun_arg1, const double fun_arg2);
    
    static void make_circle_convolution (const double radius, CMat *convolution, const int m);
    
    static void make_gaussian_convolution (const double alpha, CMat *convolution, const int m);
    
    static void convolve_star_mat (CMat *result, const CMat *const mata, const CMat* const matb);
    
    static CMat *compute_g_matrix (const CMat * const convolution, const int m,
                                   const double gamma, const double noise_factor,
                                   const double musq, const bool symmetric);
    
    static void finish_matrix (Mat * mat);
    static void finish_and_free_matrix (Mat * mat);
    static void init_c_mat (CMat * mat, const int radius);
    static void finish_c_mat (CMat * mat);

private:

    // Debug methods.
    static void print_c_mat (const CMat * const mat);
    static void print_matrix (Mat * matrix);

    static Mat *allocate_matrix (int nrows, int ncols);
    static double *mat_eltptr (Mat * mat, const int r, const int c);
    static double mat_elt (const Mat * mat, const int r, const int c);
    static CMat *allocate_c_mat (const int radius);
    static inline double *c_mat_eltptr (CMat * mat, const int col, const int row);
    static inline double c_mat_elt (const CMat * const mat, const int col, const int row);
    static void convolve_mat (CMat * result, const CMat * const mata, const CMat * const matb);
    static void convolve_mat_fun (CMat * result, const CMat * const mata, double (f) (int, int));
    static int as_idx (const int k, const int l, const int m);
    static int as_cidx (const int k, const int l);
    static Mat *make_s_matrix (CMat * mat, int m, double noise_factor);
    static Mat *make_s_cmatrix (CMat * mat, int m, double noise_factor);
    static double correlation (const int x, const int y, const double gamma, const double musq);
    static Mat *copy_vec (const CMat * const mat, const int m);
    static Mat *copy_cvec (const CMat * const mat, const int m);
    static CMat *copy_cvec2mat (const Mat * const cvec, const int m);
    static CMat *copy_vec2mat (const Mat * const cvec, const int m);
    static CMat *compute_g (const CMat * const convolution, const int m, const double gamma,
                                           const double noise_factor, const double musq, const bool symmetric);
    static double circle_integral (const double x, const double radius);
    static double circle_intensity (const int x, const int y, const double radius);               
    
    // CLapack interface.
    static int dgesv (const int N, const int NRHS, double *A, const int lda, double *B, const int ldb);

};
            
}  // NameSpace DigikamImagesPluginCore

#endif /* MATRIX_H_INCLUDED */
