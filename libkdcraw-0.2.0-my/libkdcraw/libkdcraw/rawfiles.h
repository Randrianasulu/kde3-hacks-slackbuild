/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2005-11-06
 * Description : list of RAW file extensions supported by libraw
 * 
 * Copyright (C) 2005-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful *.
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

#ifndef RAW_FILES_H
#define RAW_FILES_H

// NOTE: extension list Version 1 and 2 are taken from http://www.cybercom.net/~dcoffin/dcraw/rawphoto.c

//                                           Ext    Descriptions From
//                                                  www.file-extensions.org
//                                                  en.wikipedia.org/wiki/RAW_file_format
//                                                  filext.com

static const char raw_file_extentions[] = 

// NOTE: VERSION 1
                                          "*.bay "  // Casio Digital Camera Raw File Format.
                                          "*.bmq "  // NuCore Raw Image File.
                                          "*.cr2 "  // Canon Digital Camera RAW Image Format version 2.0. These images are based on the TIFF image standard.
                                          "*.crw "  // Canon Digital Camera RAW Image Format version 1.0. 
                                          "*.cs1 "  // Capture Shop Raw Image File.
                                          "*.dc2 "  // Kodak DC25 Digital Camera File.
                                          "*.dcr "  // Kodak Digital Camera Raw Image Format for these models: Kodak DSC Pro SLR/c, Kodak DSC Pro SLR/n, Kodak DSC Pro 14N, Kodak DSC PRO 14nx.
                                          "*.dng "  // Adobe Digital Negative: DNG is publicly available archival format for the raw files generated by digital cameras. By addressing the lack of an open standard for the raw files created by individual camera models, DNG helps ensure that photographers will be able to access their files in the future. 
                                          "*.erf "  // Epson Digital Camera Raw Image Format.
                                          "*.fff "  // Imacon Digital Camera Raw Image Format.
                                          "*.hdr "  // Leaf Raw Image File.
                                          "*.k25 "  // Kodak DC25 Digital Camera Raw Image Format.
                                          "*.kdc "  // Kodak Digital Camera Raw Image Format.
                                          "*.mdc "  // Minolta RD175 Digital Camera Raw Image Format.
                                          "*.mos "  // Mamiya Digital Camera Raw Image Format.
                                          "*.mrw "  // Minolta Dimage Digital Camera Raw Image Format.
                                          "*.nef "  // Nikon Digital Camera Raw Image Format.
                                          "*.orf "  // Olympus Digital Camera Raw Image Format.
                                          "*.pef "  // Pentax Digital Camera Raw Image Format.
                                          "*.pxn "  // Logitech Digital Camera Raw Image Format.
                                          "*.raf "  // Fuji Digital Camera Raw Image Format.
                                          "*.raw "  // Panasonic Digital Camera Image Format.
                                          "*.rdc "  // Digital Foto Maker Raw Image File.
                                          "*.sr2 "  // Sony Digital Camera Raw Image Format.
                                          "*.srf "  // Sony Digital Camera Raw Image Format for DSC-F828 8 megapixel digital camera or Sony DSC-R1
                                          "*.x3f "  // Sigma Digital Camera Raw Image Format for devices based on Foveon X3 direct image sensor.
                                          "*.arw "  // Sony Digital Camera Raw Image Format for Alpha devices.

// NOTE: VERSION 2

                                          "*.3fr "  // Hasselblad Digital Camera Raw Image Format.
                                          "*.cine " // Phantom Software Raw Image File.
                                          "*.ia "   // Sinar Raw Image File.
                                          "*.kc2 "  // Kodak DCS200 Digital Camera Raw Image Format.
                                          "*.mef "  // Mamiya Digital Camera Raw Image Format.
                                          "*.nrw "  // Nikon Digital Camera Raw Image Format.
                                          "*.qtk "  // Apple Quicktake 100/150 Digital Camera Raw Image Format.
                                          "*.rw2 "  // Panasonic LX3 Digital Camera Raw Image Format.
                                          "*.sti"   // Sinar Capture Shop Raw Image File.

// NOTE: VERSION 3

                                          "*.rwl";  // Leica Digital Camera Raw Image Format.

/* TODO: check if these format are supported
                                          "*.drf "  // Kodak Digital Camera Raw Image Format.
                                          "*.dsc "  // Kodak Digital Camera Raw Image Format.
                                          "*.ptx "  // Pentax Digital Camera Raw Image Format.
                                          "*.cap "  // Phase One Digital Camera Raw Image Format.
                                          "*.iiq "  // Phase One Digital Camera Raw Image Format.
                                          "*.rwz "  // Rawzor Digital Camera Raw Image Format.
*/

#endif // RAW_FILES_H
