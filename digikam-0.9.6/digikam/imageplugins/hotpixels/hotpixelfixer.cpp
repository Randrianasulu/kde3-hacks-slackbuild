/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-03-27
 * Description : Threaded image filter to fix hot pixels
 * 
 * Copyright (C) 2005-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// C++ includes.

#include <cmath>
#include <cstdlib>

// Qt includes.

#include <qcolor.h>
#include <qregexp.h>
#include <qstringlist.h>

// Local includes.

#include "dimg.h"
#include "ddebug.h"
#include "hotpixelfixer.h"

#ifdef HAVE_FLOAT_H
#if HAVE_FLOAT_H
# include <float.h>
#endif
#endif

#ifndef DBL_MIN
# define DBL_MIN 1e-37
#endif
#ifndef DBL_MAX
# define DBL_MAX 1e37
#endif

namespace DigikamHotPixelsImagesPlugin
{

HotPixelFixer::HotPixelFixer(Digikam::DImg *orgImage, QObject *parent, const QValueList<HotPixel>& hpList, 
                             int interpolationMethod)
             : Digikam::DImgThreadedFilter(orgImage, parent, "HotPixels")
{
    m_hpList              = hpList;
    m_interpolationMethod = interpolationMethod;
    mWeightList.clear();
    
    initFilter();
}

HotPixelFixer::~HotPixelFixer()
{
}

void HotPixelFixer::filterImage(void)
{
    QValueList <HotPixel>::ConstIterator it;
    QValueList <HotPixel>::ConstIterator end(m_hpList.end()); 
    for (it = m_hpList.begin() ; it != end ; ++it)
    {
        HotPixel hp = *it;
        interpolate(m_orgImage, hp, m_interpolationMethod);
    }
        
    m_destImage = m_orgImage;
}

// Interpolates a pixel block
void HotPixelFixer::interpolate (Digikam::DImg &img, HotPixel &hp, int method)
{
    const int xPos = hp.x();
    const int yPos = hp.y();
    bool sixtBits  = img.sixteenBit();
    
    // Interpolate pixel.
    switch (method)
    {
        case AVERAGE_INTERPOLATION:
        {
            // We implement the bidimendional one first.
            // TODO: implement the rest of directions (V & H) here
        
            //case twodim:
            // {
            int sum_weight = 0;
            double vr=0.0,vg=0.0,vb=0.0;
            int x, y;
            Digikam::DColor col;
        
            for (x = xPos; x < xPos+hp.width(); ++x)
            {
                if (validPoint(img,QPoint(x,yPos-1)))
                {
                    col=img.getPixelColor(x,yPos-1);
                    vr += col.red();
                    vg += col.green();
                    vb += col.blue();
                    ++sum_weight;
                }
                if (validPoint(img,QPoint(x,yPos+hp.height())))
                {
                    col=img.getPixelColor(x,yPos+hp.height());
                    vr += col.red();
                    vg += col.green();
                    vb += col.blue();
                    ++sum_weight;
                }
            }
        
            for (y = yPos; y < hp.height(); ++y)
            {
                if (validPoint(img,QPoint(xPos-1,y)))
                {
                    col=img.getPixelColor(xPos,y);
                    vr += col.red();
                    vg += col.green();
                    vb += col.blue();
                    ++sum_weight;
                }
                if (validPoint(img,QPoint(xPos+hp.width(),y)))
                {
                    col=img.getPixelColor(xPos+hp.width(),y);
                    vr += col.red();
                    vg += col.green();
                    vb += col.blue();
                    ++sum_weight;
                }
            }
        
            if (sum_weight > 0)
            {
                vr /= (double)sum_weight;
                vg /= (double)sum_weight;
                vb /= (double)sum_weight;
    
                
                for (x = 0; x < hp.width(); ++x)
                for (y = 0; y < hp.height(); ++y)
                if (validPoint(img,QPoint(xPos+x,yPos+y)))
                {
                    int alpha=sixtBits ? 65535 : 255;
                    int ir=(int )round(vr),ig=(int) round(vg),ib=(int) round(vb);
                    img.setPixelColor(xPos+x,yPos+y,Digikam::DColor(ir,ig,ib,alpha,sixtBits));
                }
            }
            break;
        } //Case average
    
        case LINEAR_INTERPOLATION:
            //(Bi)linear interpolation.
            weightPixels (img,hp,LINEAR_INTERPOLATION,TWODIM_DIRECTION,sixtBits ? 65535: 255);
            break;
    
        case QUADRATIC_INTERPOLATION:
            // (Bi)quadratic interpolation.
            weightPixels (img,hp,QUADRATIC_INTERPOLATION,TWODIM_DIRECTION,sixtBits ? 65535 : 255);
            break;
    
        case CUBIC_INTERPOLATION:
            // (Bi)cubic interpolation. 
            weightPixels (img,hp,CUBIC_INTERPOLATION,TWODIM_DIRECTION,sixtBits ? 65535 : 255);
            break;
    } //switch
}

void HotPixelFixer::weightPixels (Digikam::DImg &img, HotPixel &px, int method, Direction dir,int maxComponent)
{
    //TODO: implement direction here too
        
    for (int iComp = 0; iComp < 3; ++iComp)
    {
        // Obtain weight data block.  
    
        Weights w;
        int polynomeOrder=-1;
        
        switch (method)
        {
            case AVERAGE_INTERPOLATION:  // Gilles: to prevent warnings from compiler.
                break;
            case LINEAR_INTERPOLATION:
                polynomeOrder=1;
                break;
            case QUADRATIC_INTERPOLATION:
                polynomeOrder=2;
                break;
            case CUBIC_INTERPOLATION:
                polynomeOrder=3;
                break;        
        }
        if (polynomeOrder<0) return;
        
        // In the one-dimensional case, the width must be 1,
        // and the size must be stored in height 
        
        w.setWidth(dir == TWODIM_DIRECTION ? px.width() : 1);
        w.setHeight(dir == HORIZONTAL_DIRECTION ? px.width() : px.height());
        w.setPolynomeOrder(polynomeOrder);
        w.setTwoDim(dir == TWODIM_DIRECTION);
    
        //TODO: check this, it must not recalculate existing calculated weights
        //for now I don't think it is finding the duplicates fine, so it uses
        //the previous one always...
        
        //if (mWeightList.find(w)==mWeightList.end())
        //{
            w.calculateWeights();
            
        //    mWeightList.append(w);
            
        //}
     
        // Calculate weighted pixel sum.  
        for (int y = 0; y<px.height(); ++y)
        {
            for (int x = 0; x < px.width(); ++x)
            {
                if (validPoint (img,QPoint(px.x()+x,px.y()+y)))
                {
                    double sum_weight = 0.0, v = 0.0;
                    size_t i;
            
                    for (i = 0; i < w.positions().count(); ++i)
                    {
                        // In the one-dimensional case, only the y coordinate is used.  
                        const int xx = px.x()+(dir == VERTICAL_DIRECTION ? x : 
                                  dir== HORIZONTAL_DIRECTION ? w.positions()[i].y() : w.positions()[i].x());
                        const int yy = px.y()+(dir == HORIZONTAL_DIRECTION ? y : 
                                  w.positions()[i].y());
            
                        if (validPoint (img,QPoint(xx, yy)))
                        {
                            //TODO: check this. I think it is broken
                            double weight;
                            if (dir==VERTICAL_DIRECTION)
                            {
                                weight = w[i][y][0];
                            }
                            else if (dir==HORIZONTAL_DIRECTION)
                            {
                                weight=w[i][0][x];
                            }
                            else
                            {
                                weight=w[i][y][x];
                            }
                                    
                            if (iComp==0) v += weight * img.getPixelColor(xx, yy).red();
                            else if (iComp==1) v += weight * img.getPixelColor(xx, yy).green();
                            else v += weight * img.getPixelColor(xx, yy).blue();
                            
                            sum_weight += weight;
                        }
                    } 
                        
                    Digikam::DColor color=img.getPixelColor(px.x()+x,px.y()+y);
                    int component;
                    if (fabs (v) <= DBL_MIN)
                        component=0;
                    else if (sum_weight >= DBL_MIN)
                    {	
                        component=(int) (v/sum_weight);
                        //Clamp value
                        if (component<0) component=0;
                        if (component>maxComponent) component=maxComponent;
                    }
                    else if (v >= 0.0)
                        component=maxComponent;
                    else
                        component=0;
                    
                    if (iComp==0) color.setRed(component);
                    else if (iComp==1) color.setGreen(component);
                    else color.setBlue(component);

                    
                    img.setPixelColor(px.x()+x,px.y()+y,color);
                } 
            } 
        }
    }
}

}  // NameSpace DigikamHotPixelsImagesPlugin
