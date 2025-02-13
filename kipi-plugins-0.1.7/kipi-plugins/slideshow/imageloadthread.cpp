/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-11-14
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2007 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 *
 * Parts of this code are based on smoothslidesaver by Carsten Weinhold 
 * <carsten dot weinhold at gmx dot de>                                           
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Qt includes.

#include <qdir.h>
#include <qvaluevector.h>
#include <qdeepcopy.h>
#include <qwmatrix.h>
#include <qobject.h>
             
// KDE includes.

#include <klocale.h>
             
// Local includes.

#include "slideshowkb.h"
#include "imageloadthread.h"
#include "imageloadthread.moc"

namespace KIPISlideShowPlugin
{

ImageLoadThread::ImageLoadThread(QValueList<QPair<QString, int> >& fileList, 
                                 int width, int height) 
{

    m_initialized   = false;
    m_needImage     = true;
    m_haveImages    = false;
    m_quitRequested = false;

    m_fileIndex = 0;
    m_fileList = fileList;

    m_width = width;
    m_height = height;
}

void ImageLoadThread::quit() {

    QMutexLocker locker(&m_condLock);
    
    m_quitRequested = true;
    m_imageRequest.wakeOne();
}

void ImageLoadThread::requestNewImage() {

    QMutexLocker locker(&m_condLock);
    
    if ( !m_needImage) {
        m_needImage = true;
        m_imageRequest.wakeOne();
    }
}

void ImageLoadThread::run() {

    QMutexLocker locker(&m_condLock);

    // we enter the loop with m_needImage==true, so we will immediatly
    // try to load an image

    while (true) {

        if (m_quitRequested)
            break;

        if (m_needImage) {

          if ( m_fileIndex == (int)m_fileList.count() )
          {
            m_needImage = false;
            emit(endOfShow());
            continue;
          }
          
            m_needImage = false;
            m_condLock.unlock();

            bool ok;
            do {
                ok = loadImage();
                if ( !ok)
                    invalidateCurrentImageName();
            } while ( !ok && m_fileIndex < (int)m_fileList.count());
            
            if ( m_fileIndex == (int)m_fileList.count() )
            {
              
              emit(endOfShow());
              m_condLock.lock();
              continue;
            }
 
            if ( !ok) {
                // generate a black dummy image
                m_texture = QImage(128, 128, 32);
                m_texture.fill(Qt::black.rgb());
            }

            m_condLock.lock();
            
            m_fileIndex++;
            
            if ( !m_initialized) {
                m_haveImages  = ok;
                m_initialized = true;
            }

        } else {
            // wait for new requests from the consumer
            m_imageRequest.wait(&m_condLock);
        }
    }
}

bool ImageLoadThread::loadImage() {

    QPair<QString, int> fileAngle = m_fileList[m_fileIndex];
    QString path(fileAngle.first);
    int     angle(fileAngle.second);
    QImage image(path);
    if (angle != 0)
    {
        QWMatrix wm;
        wm.rotate(angle);
        image = image.xForm(wm);
    }
    
    if (image.isNull()) {
        return false;
    }

    float aspect  = (float)image.width() / (float)image.height();    
    
    image = image.smoothScale(m_width, m_height, QImage::ScaleMin);
    
    m_imageLock.lock();
    
    // this is the critical moment, when we make the new texture and
    // aspect available to the consumer
    m_textureAspect = aspect;
    m_texture       = QGLWidget::convertToGLFormat(image);
    
    m_imageLock.unlock();

    return true;
}

void ImageLoadThread::invalidateCurrentImageName() {
    m_fileList.remove(m_fileList[m_fileIndex]);
    m_fileIndex++;
}

}  // NameSpace KIPISlideShowPlugin
