/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-06-14
 * Description : A JPEG2000 IO file for DImg framework
 *
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com> 
 * 
 * This implementation use Jasper API 
 * library : http://www.ece.uvic.ca/~mdadams/jasper
 * Other JPEG2000 encoder-decoder : http://www.openjpeg.org
 *
 * Others Linux JPEG2000 Loader implementation using Jasper:
 * http://cvs.graphicsmagick.org/cgi-bin/cvsweb.cgi/GraphicsMagick/coders/jp2.c  
 * https://subversion.imagemagick.org/subversion/ImageMagick/trunk/coders/jp2.c
 * http://svn.ghostscript.com:8080/jasper/trunk/src/appl/jasper.c
 * http://websvn.kde.org/trunk/KDE/kdelibs/kimgio/jp2.cpp
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

// This line must be commented to prevent any latency time
// when we use threaded image loader interface for each image
// files io. Uncomment this line only for debugging.
//#define ENABLE_DEBUG_MESSAGES

// Qt includes.

#include <qfile.h>
#include <qcstring.h>

// Local includes.

#include "ddebug.h"
#include "dimg.h"
#include "dimgloaderobserver.h"
#include "jp2kloader.h"

namespace Digikam
{

JP2KLoader::JP2KLoader(DImg* image)
          : DImgLoader(image)
{
    m_hasAlpha   = false;
    m_sixteenBit = false;
}

bool JP2KLoader::load(const QString& filePath, DImgLoaderObserver *observer)
{    
    readMetadata(filePath, DImg::JPEG);

    FILE *file = fopen(QFile::encodeName(filePath), "rb");
    if (!file)
        return false;

    unsigned char header[9];

    if (fread(&header, 9, 1, file) != 1)
    {
        fclose(file);
        return false;
    }

    unsigned char jp2ID[5] = { 0x6A, 0x50, 0x20, 0x20, 0x0D, };
    unsigned char jpcID[2] = { 0xFF, 0x4F };

    if (memcmp(&header[4], &jp2ID, 5) != 0 &&
        memcmp(&header,    &jpcID, 2) != 0)
    {
        // not a jpeg2000 file
        fclose(file);
        return false;
    }

    fclose(file);

    // -------------------------------------------------------------------
    // Initialize JPEG 2000 API.

    register long  i, x, y;
    int            components[4];
    unsigned int   maximum_component_depth, scale[4], x_step[4], y_step[4];
    unsigned long  number_components;

    jas_image_t  *jp2_image   = 0;
    jas_stream_t *jp2_stream  = 0;
    jas_matrix_t *pixels[4];

    int init = jas_init();
    if (init != 0)
    {
        DDebug() << "Unable to init JPEG2000 decoder" << endl;
        return false;
    }

    jp2_stream = jas_stream_fopen(QFile::encodeName(filePath), "rb");
    if (jp2_stream == 0)
    {
        DDebug() << "Unable to open JPEG2000 stream" << endl;
        return false;
    }

    jp2_image = jas_image_decode(jp2_stream, -1, 0);
    if (jp2_image == 0)
    {
        jas_stream_close(jp2_stream);
        DDebug() << "Unable to decode JPEG2000 image" << endl;
        return false;
    }

    jas_stream_close(jp2_stream);

    // some pseudo-progress
    if (observer)
        observer->progressInfo(m_image, 0.1);

    // -------------------------------------------------------------------
    // Check color space.

    switch (jas_clrspc_fam(jas_image_clrspc(jp2_image)))
    {
        case JAS_CLRSPC_FAM_RGB:
        {
            components[0] = jas_image_getcmptbytype(jp2_image, JAS_IMAGE_CT_RGB_R);
            components[1] = jas_image_getcmptbytype(jp2_image, JAS_IMAGE_CT_RGB_G);
            components[2] = jas_image_getcmptbytype(jp2_image, JAS_IMAGE_CT_RGB_B);
            if ((components[0] < 0) || (components[1] < 0) || (components[2] < 0))
            {
                jas_image_destroy(jp2_image);
                DDebug() << "Error parsing JPEG2000 image : Missing Image Channel" << endl;
                return false;
            }

            number_components = 3;
            components[3]     = jas_image_getcmptbytype(jp2_image, 3);
            if (components[3] > 0)
            {
                m_hasAlpha = true;
                number_components++;
            }
            break;
        }
        case JAS_CLRSPC_FAM_GRAY:
        {
            components[0] = jas_image_getcmptbytype(jp2_image, JAS_IMAGE_CT_GRAY_Y);
            if (components[0] < 0)
            {
                jas_image_destroy(jp2_image);
                DDebug() << "Error parsing JP2000 image : Missing Image Channel" << endl;
                return false;
            }
            number_components=1;
            break;
        }
        case JAS_CLRSPC_FAM_YCBCR:
        {
            components[0] = jas_image_getcmptbytype(jp2_image, JAS_IMAGE_CT_YCBCR_Y);
            components[1] = jas_image_getcmptbytype(jp2_image, JAS_IMAGE_CT_YCBCR_CB);
            components[2] = jas_image_getcmptbytype(jp2_image, JAS_IMAGE_CT_YCBCR_CR);
            if ((components[0] < 0) || (components[1] < 0) || (components[2] < 0))
            {
                jas_image_destroy(jp2_image);
                DDebug() << "Error parsing JP2000 image : Missing Image Channel" << endl;
                return false;
            }
            number_components = 3;
            components[3]     = jas_image_getcmptbytype(jp2_image, JAS_IMAGE_CT_UNKNOWN);
            if (components[3] > 0)
            {
                m_hasAlpha = true;
                number_components++;
            }
            // FIXME : image->colorspace=YCbCrColorspace;
            break;
        }
        default:
        {
            jas_image_destroy(jp2_image);
            DDebug() << "Error parsing JP2000 image : Colorspace Model Is Not Supported" << endl;
            return false;
        }
    }

    // -------------------------------------------------------------------
    // Check image geometry.

    imageWidth()  = jas_image_width(jp2_image);
    imageHeight() = jas_image_height(jp2_image);
  
    for (i = 0; i < (long)number_components; i++)
    {
        if ((((jas_image_cmptwidth(jp2_image, components[i])*
               jas_image_cmpthstep(jp2_image, components[i])) != (long)imageWidth()))  ||
            (((jas_image_cmptheight(jp2_image, components[i])*
               jas_image_cmptvstep(jp2_image, components[i])) != (long)imageHeight())) ||
            (jas_image_cmpttlx(jp2_image, components[i]) != 0)                         ||
            (jas_image_cmpttly(jp2_image, components[i]) != 0)                         ||
            (jas_image_cmptsgnd(jp2_image, components[i]) != false))
        {
            jas_image_destroy(jp2_image);
            DDebug() << "Error parsing JPEG2000 image : Irregular Channel Geometry Not Supported" << endl;
            return false;
        }
        x_step[i] = jas_image_cmpthstep(jp2_image, components[i]);
        y_step[i] = jas_image_cmptvstep(jp2_image, components[i]);
    }

    // -------------------------------------------------------------------
    // Convert image data.

    m_hasAlpha              = number_components > 3;     
    maximum_component_depth = 0;

    for (i = 0; i < (long)number_components; i++)
    {
        maximum_component_depth = QMAX(jas_image_cmptprec(jp2_image,components[i]),
                                       (long)maximum_component_depth);
        pixels[i] = jas_matrix_create(1, ((unsigned int)imageWidth())/x_step[i]);
        if (!pixels[i])
        {
            jas_image_destroy(jp2_image);
            DDebug() << "Error decoding JPEG2000 image data : Memory Allocation Failed" << endl;
            return false;
        }
    }

    if (maximum_component_depth > 8)
        m_sixteenBit = true;

    for (i = 0 ; i < (long)number_components ; i++)
    {
        scale[i] = 1;
        int prec = jas_image_cmptprec(jp2_image, components[i]);
        if (m_sixteenBit && prec < 16)
            scale[i] = (1 << (16 - jas_image_cmptprec(jp2_image, components[i])));
    }

    uchar* data = 0;
    if (m_sixteenBit)          // 16 bits image.
        data = new uchar[imageWidth()*imageHeight()*8];
    else
        data = new uchar[imageWidth()*imageHeight()*4];

    if (!data)
    {
        DDebug() << "Error decoding JPEG2000 image data : Memory Allocation Failed" << endl;
        jas_image_destroy(jp2_image);
        for (i = 0 ; i < (long)number_components ; i++)
            jas_matrix_destroy(pixels[i]);

        jas_cleanup();
        return false;
    }

    uint   checkPoint     = 0;
    uchar *dst            = data;
    unsigned short *dst16 = (unsigned short *)data;
    
    for (y = 0 ; y < (long)imageHeight() ; y++)
    {               
        for (i = 0 ; i < (long)number_components; i++)
        {
            int ret = jas_image_readcmpt(jp2_image, (short)components[i], 0,
                                         ((unsigned int) y)            / y_step[i], 
                                         ((unsigned int) imageWidth()) / x_step[i],
                                         1, pixels[i]);
            if (ret != 0)
            {
                DDebug() << "Error decoding JPEG2000 image data" << endl;
                delete [] data;
                jas_image_destroy(jp2_image);
                for (i = 0 ; i < (long)number_components ; i++)
                    jas_matrix_destroy(pixels[i]);

                jas_cleanup();
                return false;
            }
        }
        
        switch (number_components)
        {
            case 1: // Grayscale.
            {
                for (x = 0 ; x < (long)imageWidth() ; x++)
                {
                    dst[0] = (uchar)(scale[0]*jas_matrix_getv(pixels[0], x/x_step[0]));
                    dst[1] = dst[0];
                    dst[2] = dst[0];
                    dst[3] = 0xFF;                                                        

                    dst += 4;
                }
                break;
            }
            case 3: // RGB.
            { 
                if (!m_sixteenBit)   // 8 bits image.
                {
                    for (x = 0 ; x < (long)imageWidth() ; x++)
                    {
                        // Blue
                        dst[0] = (uchar)(scale[2]*jas_matrix_getv(pixels[2], x/x_step[2]));
                        // Green
                        dst[1] = (uchar)(scale[1]*jas_matrix_getv(pixels[1], x/x_step[1]));
                        // Red
                        dst[2] = (uchar)(scale[0]*jas_matrix_getv(pixels[0], x/x_step[0])); 
                        // Alpha
                        dst[3] = 0xFF;                                                        

                        dst += 4;
                    }
                }
                else                // 16 bits image.
                {
                    for (x = 0 ; x < (long)imageWidth() ; x++)
                    {
                        // Blue
                        dst16[0] = (unsigned short)(scale[2]*jas_matrix_getv(pixels[2], x/x_step[2]));
                        // Green
                        dst16[1] = (unsigned short)(scale[1]*jas_matrix_getv(pixels[1], x/x_step[1]));
                        // Red
                        dst16[2] = (unsigned short)(scale[0]*jas_matrix_getv(pixels[0], x/x_step[0]));
                        // Alpha
                        dst16[3] = 0xFFFF;                                                           

                        dst16 += 4;
                    }
                }

                break;
            }
            case 4: // RGBA.
            {
                if (!m_sixteenBit)   // 8 bits image.
                {
                    for (x = 0 ; x < (long)imageWidth() ; x++)
                    {
                        // Blue
                        dst[0] = (uchar)(scale[2] * jas_matrix_getv(pixels[2], x/x_step[2]));
                        // Green                        
                        dst[1] = (uchar)(scale[1] * jas_matrix_getv(pixels[1], x/x_step[1]));
                        // Red
                        dst[2] = (uchar)(scale[0] * jas_matrix_getv(pixels[0], x/x_step[0]));
                        // Alpha
                        dst[3] = (uchar)(scale[3] * jas_matrix_getv(pixels[3], x/x_step[3]));
                        
                        dst += 4;
                    }
                }
                else                // 16 bits image.
                {
                    for (x = 0 ; x < (long)imageWidth() ; x++)
                    {
                        // Blue
                        dst16[0] = (unsigned short)(scale[2]*jas_matrix_getv(pixels[2], x/x_step[2]));
                        // Green
                        dst16[1] = (unsigned short)(scale[1]*jas_matrix_getv(pixels[1], x/x_step[1]));
                        // Red                        
                        dst16[2] = (unsigned short)(scale[0]*jas_matrix_getv(pixels[0], x/x_step[0]));
                        // Alpha
                        dst16[3] = (unsigned short)(scale[3]*jas_matrix_getv(pixels[3], x/x_step[3]));
                        
                        dst16 += 4;
                    }
                }

                break;
            }
        }

        // use 0-10% and 90-100% for pseudo-progress
        if (observer && y >= (long)checkPoint)
        {
            checkPoint += granularity(observer, y, 0.8);
            if (!observer->continueQuery(m_image))
            {
                delete [] data;
                jas_image_destroy(jp2_image);
                for (i = 0 ; i < (long)number_components ; i++)
                    jas_matrix_destroy(pixels[i]);

                jas_cleanup();

                return false;
            }
            observer->progressInfo(m_image, 0.1 + (0.8 * ( ((float)y)/((float)imageHeight()) )));
        }
    }

    // -------------------------------------------------------------------
    // Get ICC color profile.

    jas_iccprof_t *icc_profile = 0;
    jas_stream_t  *icc_stream  = 0;
    jas_cmprof_t  *cm_profile  = 0;

    cm_profile = jas_image_cmprof(jp2_image);
    if (cm_profile != 0)
        icc_profile = jas_iccprof_createfromcmprof(cm_profile);

    if (icc_profile != 0)
    {
        icc_stream = jas_stream_memopen(NULL, 0);

        if (icc_stream != 0)
        {
            if (jas_iccprof_save(icc_profile, icc_stream) == 0)
            {
                if (jas_stream_flush(icc_stream) == 0)
                {
                    QMap<int, QByteArray>& metaData = imageMetaData();
                    jas_stream_memobj_t *blob = (jas_stream_memobj_t *) icc_stream->obj_;
                    QByteArray profile_rawdata(blob->len_);
                    memcpy(profile_rawdata.data(), blob->buf_, blob->len_);
                    metaData.insert(DImg::ICC, profile_rawdata);
                    jas_stream_close(icc_stream);
                }
            }
        }
    }

    if (observer)
        observer->progressInfo(m_image, 1.0);

    imageSetAttribute("format", "JP2K");
    imageData() = data;

    jas_image_destroy(jp2_image);
    for (i = 0 ; i < (long)number_components ; i++)
        jas_matrix_destroy(pixels[i]);

    jas_cleanup();

    return true;
}

bool JP2KLoader::save(const QString& filePath, DImgLoaderObserver *observer)
{
    FILE *file = fopen(QFile::encodeName(filePath), "wb");
    if (!file)
        return false;

    fclose(file);

    // -------------------------------------------------------------------
    // Initialize JPEG 2000 API.

    register long  i, x, y;
    unsigned long  number_components;

    jas_image_t          *jp2_image   = 0;
    jas_stream_t         *jp2_stream  = 0;
    jas_matrix_t         *pixels[4];
    jas_image_cmptparm_t  component_info[4];

    int init = jas_init();
    if (init != 0)
    {
        DDebug() << "Unable to init JPEG2000 decoder" << endl;
        return false;
    }

    jp2_stream = jas_stream_fopen(QFile::encodeName(filePath), "wb");
    if (jp2_stream == 0)
    {
        DDebug() << "Unable to open JPEG2000 stream" << endl;
        return false;
    }
    
    number_components = imageHasAlpha() ? 4 : 3;

    for (i = 0 ; i < (long)number_components ; i++)
    {
        component_info[i].tlx    = 0;
        component_info[i].tly    = 0;
        component_info[i].hstep  = 1;
        component_info[i].vstep  = 1;
        component_info[i].width  = imageWidth();
        component_info[i].height = imageHeight();
        component_info[i].prec   = imageBitsDepth();
        component_info[i].sgnd   = false;
    }

    jp2_image = jas_image_create(number_components, component_info, JAS_CLRSPC_UNKNOWN);
    if (jp2_image == 0)
    {
        jas_stream_close(jp2_stream);
        DDebug() << "Unable to create JPEG2000 image" << endl;
        return false;
    }

    if (observer)
        observer->progressInfo(m_image, 0.1);

    // -------------------------------------------------------------------
    // Check color space.

    if (number_components >= 3 )    // RGB & RGBA
    {
        // Alpha Channel
        if (number_components == 4 )
            jas_image_setcmpttype(jp2_image, 3, JAS_IMAGE_CT_OPACITY);
        
        jas_image_setclrspc(jp2_image, JAS_CLRSPC_SRGB);
        jas_image_setcmpttype(jp2_image, 0, JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_R));
        jas_image_setcmpttype(jp2_image, 1, JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_G));
        jas_image_setcmpttype(jp2_image, 2, JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_B));
    }

    // -------------------------------------------------------------------
    // Set ICC color profile.

    // FIXME : doesn't work yet!

    jas_cmprof_t  *cm_profile  = 0;
    jas_iccprof_t *icc_profile = 0;

    QByteArray profile_rawdata = m_image->getICCProfil();

    icc_profile = jas_iccprof_createfrombuf((uchar*)profile_rawdata.data(), profile_rawdata.size());
    if (icc_profile != 0)
    {
        cm_profile = jas_cmprof_createfromiccprof(icc_profile);
        if (cm_profile != 0)
        {
            jas_image_setcmprof(jp2_image, cm_profile);
        }
    }

    // -------------------------------------------------------------------
    // Convert to JPEG 2000 pixels.

    for (i = 0 ; i < (long)number_components ; i++)
    {
        pixels[i] = jas_matrix_create(1, (unsigned int)imageWidth());
        if (pixels[i] == 0)
        {
            for (x = 0 ; x < i ; x++)
                jas_matrix_destroy(pixels[x]);

            jas_image_destroy(jp2_image);
            DDebug() << "Error encoding JPEG2000 image data : Memory Allocation Failed" << endl;
            return false;
        }
    }

    unsigned char* data = imageData();
    unsigned char* pixel;
    unsigned short r, g, b, a=0;
    uint           checkpoint = 0;

    for (y = 0 ; y < (long)imageHeight() ; y++)
    {
        if (observer && y == (long)checkpoint)
        {
            checkpoint += granularity(observer, imageHeight(), 0.8);
            if (!observer->continueQuery(m_image))
            {
                jas_image_destroy(jp2_image);
                for (i = 0 ; i < (long)number_components ; i++)
                    jas_matrix_destroy(pixels[i]);

                jas_cleanup();

                return false;
            }
            observer->progressInfo(m_image, 0.1 + (0.8 * ( ((float)y)/((float)imageHeight()) )));
        }

        for (x = 0 ; x < (long)imageWidth() ; x++)
        {
            pixel = &data[((y * imageWidth()) + x) * imageBytesDepth()];

            if ( imageSixteenBit() )        // 16 bits image.
            {
                b = (unsigned short)(pixel[0]+256*pixel[1]);
                g = (unsigned short)(pixel[2]+256*pixel[3]);
                r = (unsigned short)(pixel[4]+256*pixel[5]);
                
                if (imageHasAlpha())
                    a = (unsigned short)(pixel[6]+256*pixel[7]);
            }
            else                            // 8 bits image.
            {
                b = (unsigned short)pixel[0];
                g = (unsigned short)pixel[1];
                r = (unsigned short)pixel[2];

                if (imageHasAlpha())
                    a = (unsigned short)(pixel[3]);
            }

            jas_matrix_setv(pixels[0], x, r);
            jas_matrix_setv(pixels[1], x, g);
            jas_matrix_setv(pixels[2], x, b);

            if (number_components > 3)
                jas_matrix_setv(pixels[3], x, a);
        }

        for (i = 0 ; i < (long)number_components ; i++)
        {
            int ret = jas_image_writecmpt(jp2_image, (short) i, 0, (unsigned int)y, 
                                          (unsigned int)imageWidth(), 1, pixels[i]);
            if (ret != 0)
            {
                DDebug() << "Error encoding JPEG2000 image data" << endl;

                jas_image_destroy(jp2_image);
                for (i = 0 ; i < (long)number_components ; i++)
                    jas_matrix_destroy(pixels[i]);

                jas_cleanup();
                return false;
            }
        }
    }

    QVariant qualityAttr = imageGetAttribute("quality");
    int quality = qualityAttr.isValid() ? qualityAttr.toInt() : 90;
    
    if (quality < 0)
        quality = 90;
    if (quality > 100)
        quality = 100;

    QString     rate;
    QTextStream ts( &rate, IO_WriteOnly );
        
    // NOTE: to have a lossless compression use quality=100.
    // jp2_encode()::optstr:
    // - rate=#B => the resulting file size is about # bytes
    // - rate=0.0 .. 1.0 => the resulting file size is about the factor times
    //                      the uncompressed size
    ts << "rate=" << ( quality / 100.0F );
    
    DDebug() << "JPEG2000 quality: " << quality << endl;
    DDebug() << "JPEG2000 " << rate << endl;

    int ret = jp2_encode(jp2_image, jp2_stream, rate.utf8().data());
    if (ret != 0)
    {
        DDebug() << "Unable to encode JPEG2000 image" << endl;

        jas_image_destroy(jp2_image);
        jas_stream_close(jp2_stream);
        for (i = 0 ; i < (long)number_components ; i++)
            jas_matrix_destroy(pixels[i]);
    
        jas_cleanup();

        return false;
    }

    if (observer)
        observer->progressInfo(m_image, 1.0);

    imageSetAttribute("savedformat", "JP2K");
   
    saveMetadata(filePath);
    
    jas_image_destroy(jp2_image);
    jas_stream_close(jp2_stream);
    for (i = 0 ; i < (long)number_components ; i++)
        jas_matrix_destroy(pixels[i]);

    jas_cleanup();

    return true;
}

bool JP2KLoader::hasAlpha() const
{
    return m_hasAlpha;    
}

bool JP2KLoader::sixteenBit() const
{
    return m_sixteenBit;    
}

}  // NameSpace Digikam
