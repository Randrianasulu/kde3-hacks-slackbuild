/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-03-27
 * Description : a class to calculate filter weights
 * 
 * Copyright (C) 2005-2006 by Unai Garro <ugarro at users dot sourceforge dot net>
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

#ifndef WEIGHTS_H
#define WEIGHTS_H

// Qt includes.

#include <qrect.h>
#include <qvaluelist.h>

namespace DigikamHotPixelsImagesPlugin
{

class Weights 
{
public:

    Weights(){};
    Weights(const Weights &w);
    void operator=(const Weights &w);
    
    ~Weights()
    {    
        if (!mWeightMatrices) return;
        for (unsigned int i=0; i<mPositions.count(); i++)
        {
            for (unsigned int j=0; j<mHeight; j++) delete[] mWeightMatrices[i][j];
        }
    }
    
    unsigned int height()        const   { return mHeight;        };
    unsigned int polynomeOrder() const   { return mPolynomeOrder; };
    bool         twoDim()        const   { return mTwoDim;        };
    unsigned int width()         const   { return mWidth;         };
    
    void     setHeight(int h)            { mHeight=h;             };
    void     setPolynomeOrder(int order) { mPolynomeOrder=order;  };
    void     setTwoDim(bool td)          { mTwoDim=td;            };
    void     setWidth(int w)             { mWidth=w;              };
    
    void     calculateWeights();
    bool     operator==(const Weights& ws) const;
    double** operator[](int n) const            { return mWeightMatrices[n]; };
    const QValueList <QPoint> positions() const { return mPositions;         };

protected:

    int       coefficientNumber() const { return mCoefficientNumber; };
    
    double*** weightMatrices() const    { return mWeightMatrices;    };

private:
    
    double polyTerm (const size_t i_coeff, const int x, const int y, const int poly_order);
    void   matrixInv (double *const a, const size_t size);
    
private:

    unsigned int        mHeight,mWidth;
    unsigned int        mCoefficientNumber;
    bool                mTwoDim;
    unsigned int        mPolynomeOrder;
    double ***          mWeightMatrices; //Stores a list of weight matrices
    QValueList <QPoint> mPositions;
};

}  // NameSpace DigikamHotPixelsImagesPlugin

#endif  // WEIGHTS_H
