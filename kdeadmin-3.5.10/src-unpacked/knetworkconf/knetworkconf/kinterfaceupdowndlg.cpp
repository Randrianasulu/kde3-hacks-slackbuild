#include <kdialog.h>
#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './kinterfaceupdowndlg.ui'
**
** Created: Вс фев 1 00:56:14 2015
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "kinterfaceupdowndlg.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "./kinterfaceupdowndlg.ui.h"
static const unsigned char img0_kinterfaceupdowndlg[] = { 
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x03,
    0xbb, 0x49, 0x44, 0x41, 0x54, 0x18, 0x95, 0x95, 0x95, 0x4f, 0x68, 0x1d,
    0x45, 0x1c, 0xc7, 0x3f, 0x9b, 0x6c, 0x9a, 0xdf, 0x93, 0x97, 0x3a, 0x0b,
    0x0f, 0x79, 0x4f, 0x3c, 0xf4, 0x95, 0x88, 0x6e, 0xeb, 0x9f, 0x1a, 0x5a,
    0x68, 0x5a, 0x7a, 0x48, 0xf0, 0xd2, 0x5c, 0x84, 0xde, 0xbd, 0x14, 0x41,
    0x5a, 0x44, 0x90, 0x20, 0x9e, 0x8a, 0x78, 0x12, 0x15, 0x05, 0x21, 0x20,
    0x85, 0x5c, 0x04, 0xcf, 0x39, 0xa8, 0x3c, 0x7a, 0x90, 0xf4, 0x50, 0x5a,
    0xc5, 0x5a, 0x15, 0x25, 0xdd, 0xd2, 0x04, 0xb7, 0xd0, 0x92, 0x59, 0x9b,
    0x67, 0x67, 0x24, 0xeb, 0xdb, 0x9f, 0x75, 0xdb, 0xf5, 0x90, 0xe6, 0x35,
    0x2d, 0x49, 0x20, 0x5f, 0x18, 0x66, 0xe6, 0x30, 0xdf, 0xf9, 0xf1, 0xfd,
    0x0c, 0xbf, 0x09, 0xaa, 0xaa, 0x62, 0x33, 0x05, 0x41, 0xc0, 0xcc, 0xe7,
    0x33, 0x07, 0x4f, 0x1c, 0x3f, 0x71, 0x96, 0x50, 0xf6, 0x13, 0x02, 0x25,
    0x88, 0x80, 0xea, 0xc6, 0x59, 0xa0, 0x54, 0x10, 0xa9, 0xd2, 0xa5, 0xe4,
    0xfb, 0xce, 0x37, 0x9d, 0x37, 0xcf, 0x7c, 0x78, 0x66, 0x31, 0xdc, 0xd4,
    0xf5, 0x81, 0x9a, 0x8d, 0xe6, 0x28, 0x75, 0x69, 0xcf, 0x9d, 0xf7, 0xc3,
    0x6e, 0x08, 0x5a, 0x0d, 0x03, 0x80, 0xa9, 0x0b, 0x5e, 0x15, 0x00, 0xe7,
    0x33, 0x58, 0x15, 0xa2, 0x11, 0xcf, 0xd4, 0x81, 0xf6, 0x3e, 0x45, 0xf7,
    0x00, 0xdb, 0x1b, 0x03, 0x03, 0x82, 0x04, 0x9d, 0x6b, 0x16, 0xea, 0x6d,
    0xc4, 0x2a, 0xcd, 0x86, 0x41, 0x01, 0x19, 0x12, 0xbc, 0x53, 0x54, 0x05,
    0x7b, 0xcb, 0x72, 0x64, 0x34, 0x62, 0x72, 0x54, 0x03, 0x09, 0x65, 0x10,
    0x60, 0x5b, 0x63, 0xa9, 0x09, 0xbe, 0xf4, 0x14, 0xb9, 0xe0, 0x73, 0x8b,
    0x69, 0xb4, 0x50, 0xcd, 0x10, 0x11, 0xb4, 0x54, 0x28, 0x05, 0xe7, 0x2d,
    0x3e, 0x07, 0x9b, 0x39, 0x28, 0x23, 0xbc, 0xfa, 0xb5, 0x8a, 0xb6, 0x33,
    0xd6, 0x52, 0x31, 0xa1, 0xc1, 0x7b, 0x8b, 0xed, 0x2a, 0xbe, 0x6b, 0x29,
    0x14, 0x6c, 0x57, 0x29, 0x72, 0xb0, 0x5d, 0x8b, 0xe6, 0x82, 0xbd, 0x69,
    0xa1, 0x04, 0xa7, 0x0e, 0x23, 0x6b, 0x71, 0x6d, 0x5b, 0x71, 0x24, 0x11,
    0x5a, 0x2a, 0x22, 0x11, 0xda, 0x75, 0x38, 0xdf, 0x42, 0xd5, 0x22, 0x62,
    0xf0, 0xa5, 0x07, 0x15, 0x5c, 0x9e, 0x41, 0x59, 0xc3, 0xab, 0x47, 0xc2,
    0x26, 0xfa, 0x20, 0xfb, 0x70, 0x2b, 0xfa, 0x77, 0x96, 0xef, 0xa0, 0xca,
    0x80, 0x84, 0x0c, 0xcd, 0xbe, 0x13, 0x13, 0x3d, 0xa0, 0x2f, 0x22, 0x74,
    0x7e, 0x4d, 0x98, 0xbb, 0x6c, 0x48, 0x73, 0x88, 0xa4, 0x89, 0x2b, 0x33,
    0x9a, 0xf5, 0xe6, 0xc3, 0x57, 0xb2, 0x5e, 0xf1, 0x4e, 0xe9, 0x4f, 0xbe,
    0xd0, 0xa6, 0xf3, 0x7b, 0x42, 0x4b, 0xda, 0x38, 0x6f, 0xa1, 0x61, 0xd0,
    0x50, 0x21, 0x94, 0x7e, 0xc6, 0xeb, 0x51, 0xec, 0x88, 0xfe, 0xd4, 0xf3,
    0xca, 0xf4, 0x31, 0x41, 0x0c, 0xd4, 0x68, 0xdd, 0x2b, 0xd6, 0x2e, 0x0f,
    0xb2, 0x2c, 0xfd, 0x47, 0x90, 0x5e, 0xdf, 0x78, 0xa7, 0xf4, 0xb5, 0x8c,
    0xb0, 0xde, 0xd2, 0xae, 0x4b, 0x75, 0xf5, 0x46, 0xda, 0xf5, 0xd6, 0x5f,
    0xf4, 0xa5, 0xb7, 0xce, 0xbb, 0x24, 0x59, 0x4c, 0x92, 0xfe, 0xab, 0xd8,
    0x39, 0xfd, 0x82, 0x76, 0xa3, 0x4d, 0x24, 0x51, 0x30, 0x79, 0x78, 0xf2,
    0xc9, 0xf8, 0x95, 0x78, 0x98, 0x9c, 0x2f, 0xe7, 0xcf, 0xcf, 0xcf, 0x4e,
    0xbf, 0x3d, 0xdd, 0xed, 0x1b, 0x3f, 0x42, 0x5f, 0x1d, 0xce, 0x43, 0x96,
    0x59, 0x34, 0x2f, 0xc8, 0xba, 0x16, 0xf5, 0x60, 0xbb, 0x1b, 0xe8, 0xc3,
    0xda, 0x2c, 0x42, 0x96, 0x67, 0x83, 0x94, 0x44, 0x36, 0xb7, 0xbb, 0xe2,
    0x67, 0xe3, 0xbb, 0x47, 0x27, 0x8f, 0x56, 0xfd, 0x28, 0x0a, 0x0a, 0x44,
    0x22, 0x44, 0xa0, 0xdd, 0x68, 0x81, 0x08, 0xc6, 0x44, 0x50, 0x16, 0xd4,
    0xc2, 0x16, 0x36, 0x2f, 0x1e, 0xa7, 0x5f, 0xd9, 0xd4, 0xfe, 0x9d, 0x2c,
    0x26, 0xbf, 0xa0, 0x2c, 0x24, 0xd7, 0x92, 0xaf, 0xd3, 0xc5, 0xf4, 0xb7,
    0xb9, 0x73, 0x73, 0xfd, 0xc6, 0x13, 0x02, 0xd4, 0xc2, 0x5a, 0xa0, 0xa5,
    0x23, 0x32, 0x06, 0x9f, 0x2b, 0x2d, 0x13, 0x21, 0x75, 0x45, 0xc2, 0x16,
    0x5a, 0x7a, 0x5a, 0x12, 0x3d, 0x4e, 0xff, 0x5e, 0x9a, 0xa5, 0xb7, 0xb2,
    0xcc, 0x7f, 0x61, 0x1a, 0xe6, 0xea, 0xf8, 0xc1, 0xa9, 0xc2, 0x34, 0x13,
    0x19, 0x9f, 0x98, 0xca, 0xdf, 0x7d, 0xef, 0x8d, 0x87, 0x15, 0x5b, 0x67,
    0xdd, 0x78, 0x38, 0xbe, 0xfa, 0xd5, 0xa9, 0xf6, 0x6e, 0x85, 0x8a, 0x92,
    0x80, 0x90, 0x01, 0xcd, 0x15, 0x42, 0x83, 0xcb, 0x95, 0x1a, 0xad, 0xaa,
    0x4f, 0xbf, 0x9b, 0x0e, 0x8e, 0xbd, 0x38, 0xf6, 0x9c, 0x6d, 0xba, 0xcf,
    0xcc, 0x48, 0xb4, 0x82, 0x70, 0x3f, 0xea, 0xca, 0x77, 0x97, 0x7e, 0xba,
    0xf4, 0x01, 0x50, 0xf4, 0x8d, 0xd3, 0xa5, 0xf4, 0x72, 0x67, 0xa8, 0xf3,
    0x71, 0x24, 0x51, 0xec, 0xd4, 0xed, 0x89, 0xf7, 0xc4, 0xc7, 0x6a, 0xf5,
    0xda, 0x88, 0x84, 0x82, 0xcb, 0x1d, 0x40, 0xf5, 0xf3, 0x52, 0xf2, 0xaf,
    0xbf, 0xe9, 0xff, 0x72, 0xa5, 0x13, 0x4a, 0xee, 0xab, 0x6a, 0x4e, 0xce,
    0x8a, 0x2f, 0xfd, 0x1f, 0x52, 0x37, 0xf7, 0xd3, 0xa5, 0xe4, 0x4f, 0xbf,
    0xe2, 0x1f, 0xed, 0xc1, 0x0b, 0x97, 0x16, 0x82, 0x93, 0xaf, 0x9f, 0x1c,
    0x9e, 0x3d, 0x3b, 0x3b, 0xb1, 0x7c, 0x7d, 0xf9, 0x42, 0xef, 0x76, 0xef,
    0x6e, 0xef, 0x76, 0xaf, 0x5a, 0x1f, 0xcb, 0xd7, 0x97, 0xab, 0xde, 0xed,
    0x5e, 0x71, 0xe5, 0xc7, 0x2b, 0xdf, 0xce, 0x7c, 0x34, 0x73, 0x68, 0xe2,
    0xd5, 0x89, 0xe1, 0x8b, 0xf3, 0x17, 0x03, 0x80, 0xaa, 0xaa, 0x36, 0x1d,
    0x03, 0x00, 0xfb, 0x8f, 0xec, 0xaf, 0xe2, 0x03, 0xf1, 0x7f, 0xa6, 0x6e,
    0xb0, 0xb9, 0x1d, 0xd6, 0x52, 0x03, 0xaf, 0x1e, 0x2d, 0x95, 0x2c, 0xcf,
    0xb6, 0xa5, 0xbf, 0x95, 0x82, 0x8d, 0x9b, 0xd3, 0xa7, 0x4e, 0x8f, 0xec,
    0x1b, 0xdd, 0x77, 0x58, 0xea, 0xf2, 0x5a, 0x33, 0x6a, 0xbe, 0x1c, 0x35,
    0xa2, 0x97, 0x34, 0x57, 0x4d, 0x57, 0xd2, 0x85, 0x0d, 0xf4, 0x7f, 0x98,
    0x3b, 0x37, 0xb7, 0xba, 0x7e, 0x66, 0xcb, 0x1f, 0x68, 0x7d, 0xf1, 0xc9,
    0xa7, 0xb3, 0x03, 0x11, 0xf2, 0x94, 0x34, 0xe5, 0x09, 0xcd, 0x74, 0xc4,
    0x95, 0xfe, 0xe8, 0xd4, 0xf1, 0xa9, 0xb7, 0xce, 0x5f, 0x98, 0x5f, 0x49,
    0x97, 0xd2, 0x19, 0xd3, 0x30, 0x57, 0xe3, 0x67, 0xc6, 0x8a, 0xe4, 0x56,
    0xb2, 0x2a, 0xa1, 0x74, 0xd7, 0xe9, 0x6f, 0x65, 0xdc, 0x6f, 0x9b, 0xea,
    0x5d, 0x4d, 0x9b, 0xd1, 0xfb, 0x71, 0x2b, 0x3e, 0xe4, 0x46, 0x74, 0x28,
    0x16, 0x9e, 0xa6, 0x2c, 0xcc, 0xc4, 0xa1, 0xf1, 0xbd, 0xa6, 0x6e, 0xf6,
    0x6e, 0x45, 0x7f, 0x2b, 0xf5, 0x8d, 0x93, 0x1b, 0xc9, 0x3d, 0xe3, 0xa3,
    0xd4, 0x66, 0x76, 0xb7, 0x20, 0xbb, 0x08, 0x15, 0x11, 0xf1, 0x94, 0xa0,
    0x5e, 0xef, 0x6c, 0x4b, 0x7f, 0x13, 0xfd, 0x0f, 0x22, 0x8d, 0x8a, 0x49,
    0x7c, 0x52, 0x4c, 0xa5, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44,
    0xae, 0x42, 0x60, 0x82
};

static const unsigned char img1_kinterfaceupdowndlg[] = { 
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x02,
    0xc1, 0x49, 0x44, 0x41, 0x54, 0x18, 0x95, 0xed, 0x95, 0x31, 0x88, 0x5c,
    0x45, 0x18, 0x80, 0xbf, 0xbb, 0x1b, 0xe5, 0x3f, 0xd8, 0x0b, 0xf3, 0xe0,
    0x8a, 0x37, 0x70, 0xc5, 0x3d, 0x30, 0x92, 0x0d, 0x41, 0x5d, 0x63, 0xc0,
    0x8d, 0xa4, 0xb8, 0x05, 0x21, 0xac, 0x08, 0xe6, 0xc4, 0xc2, 0x15, 0x9b,
    0x9c, 0x9d, 0xa2, 0x85, 0x56, 0xc6, 0xd2, 0x80, 0xbd, 0x90, 0xee, 0x62,
    0x21, 0x27, 0x98, 0xe8, 0x13, 0x53, 0x6c, 0x0a, 0xe1, 0xb6, 0xbb, 0x80,
    0x45, 0x16, 0x3d, 0xe2, 0x8b, 0xe4, 0xe0, 0xad, 0x26, 0x30, 0x4f, 0x38,
    0x7c, 0x53, 0x2c, 0xec, 0x5f, 0x3c, 0x58, 0x8b, 0xdb, 0x88, 0x98, 0x7b,
    0x5a, 0x1c, 0x58, 0xf9, 0x37, 0x3f, 0x33, 0xff, 0xcc, 0xc7, 0x0f, 0xdf,
    0xfc, 0x0c, 0xfc, 0x1f, 0xb3, 0x98, 0xab, 0x2b, 0x7c, 0x75, 0xed, 0xce,
    0x6f, 0xe1, 0x5e, 0x98, 0xbe, 0x75, 0xe9, 0x85, 0x57, 0xe4, 0x3c, 0x77,
    0xec, 0x6b, 0x80, 0xce, 0x8a, 0xe6, 0x20, 0x09, 0x40, 0x35, 0x5b, 0x57,
    0xb3, 0x8d, 0x31, 0xd3, 0xd1, 0x7b, 0x8c, 0x4d, 0x1d, 0x38, 0xdc, 0xcf,
    0xef, 0xd3, 0x88, 0x4e, 0xc7, 0xe7, 0xe5, 0x93, 0xce, 0x87, 0xf1, 0x93,
    0xce, 0x46, 0xf8, 0xa0, 0x58, 0x89, 0xd1, 0xb0, 0x88, 0x84, 0x09, 0xb7,
    0x64, 0x80, 0x15, 0x50, 0xb5, 0x48, 0xc3, 0xa2, 0x63, 0xc5, 0x57, 0x41,
    0x31, 0x7a, 0x61, 0xbe, 0x0e, 0x9c, 0x0e, 0xd2, 0x1b, 0x16, 0x07, 0x22,
    0x4b, 0x6f, 0x9f, 0xdb, 0x70, 0xdd, 0xe5, 0x96, 0xdb, 0xca, 0x9b, 0xee,
    0x83, 0x22, 0x72, 0x1b, 0x2b, 0x17, 0x1d, 0xc3, 0xc4, 0xc9, 0x2a, 0x2e,
    0x58, 0xdc, 0xfa, 0x73, 0x5d, 0xe7, 0x6c, 0xe2, 0x3a, 0xcf, 0xac, 0x3b,
    0x0d, 0x92, 0x50, 0xf1, 0x74, 0x6d, 0xc7, 0xbd, 0x97, 0xdf, 0x37, 0x45,
    0x28, 0x11, 0xa4, 0xf2, 0x04, 0xdc, 0x72, 0x93, 0xaf, 0xcf, 0x0c, 0x11,
    0x11, 0x32, 0xfa, 0x9c, 0x7c, 0x33, 0x22, 0xd0, 0xe6, 0x6c, 0xa3, 0x83,
    0x7c, 0x97, 0x10, 0x74, 0x13, 0x7f, 0xa6, 0x20, 0x6a, 0x58, 0x0a, 0xc2,
    0x5c, 0x2d, 0x58, 0x51, 0xec, 0x2c, 0x47, 0x08, 0x5a, 0x45, 0x14, 0xab,
    0x50, 0x8e, 0x15, 0x45, 0xc9, 0xaa, 0x8c, 0xa6, 0x71, 0xf8, 0x07, 0x25,
    0xe5, 0xc8, 0x93, 0x57, 0x9e, 0xf8, 0x5c, 0x42, 0x19, 0x14, 0x0c, 0xd3,
    0x5a, 0xb0, 0x95, 0x88, 0x82, 0x1c, 0x96, 0x04, 0x7b, 0xcf, 0x23, 0x2b,
    0x2d, 0xb6, 0x43, 0xa0, 0x35, 0xee, 0x92, 0x6b, 0x8e, 0x5b, 0x8e, 0xc9,
    0x43, 0x41, 0x3b, 0x7e, 0x96, 0xa2, 0xa5, 0x9c, 0x94, 0x26, 0xc8, 0x22,
    0x7d, 0xe9, 0x43, 0x05, 0xa6, 0xce, 0x7e, 0xae, 0x3f, 0x3d, 0x0e, 0x11,
    0x8a, 0x2e, 0x44, 0xa3, 0x82, 0x6d, 0x4d, 0x91, 0x38, 0x26, 0xbb, 0x59,
    0x92, 0xd9, 0x0c, 0x7b, 0xbc, 0xcd, 0xa2, 0xe9, 0x90, 0x8d, 0x6f, 0xe1,
    0x07, 0x13, 0x32, 0xc9, 0xe8, 0x9e, 0xe8, 0xa2, 0x7a, 0xf0, 0x4a, 0x4c,
    0x9d, 0xfd, 0x42, 0xae, 0x2c, 0x4d, 0x0a, 0xa1, 0xb7, 0xd2, 0x7a, 0xea,
    0x8d, 0x6a, 0x9b, 0x68, 0x5f, 0x98, 0x3c, 0x10, 0x5c, 0xcb, 0x91, 0x8f,
    0x3d, 0xec, 0xde, 0x46, 0xf7, 0x36, 0xd0, 0xb5, 0x2d, 0x26, 0xa7, 0x40,
    0x0d, 0x44, 0x77, 0x23, 0xa4, 0x21, 0x00, 0x0b, 0x26, 0x1d, 0xa4, 0x37,
    0x2e, 0xbe, 0xf8, 0xd1, 0xe9, 0x87, 0xf6, 0xd5, 0xe7, 0x74, 0x86, 0x81,
    0x3c, 0x02, 0xff, 0x44, 0x8f, 0xad, 0xcf, 0xfb, 0x8f, 0xf1, 0xfa, 0x00,
    0xaf, 0x4a, 0xef, 0x44, 0x8f, 0x61, 0xe1, 0xe9, 0xac, 0xb6, 0xd9, 0xdc,
    0xfd, 0x82, 0xe2, 0xb3, 0xcb, 0x3f, 0xf2, 0x0d, 0xbb, 0x18, 0xe6, 0x01,
    0x52, 0x52, 0xa8, 0x98, 0x07, 0xf6, 0xcd, 0x91, 0xec, 0xef, 0x87, 0xef,
    0xd9, 0xe7, 0x5d, 0xfe, 0x1c, 0x19, 0xe0, 0x60, 0xe8, 0xa6, 0xe6, 0x88,
    0xf6, 0x7f, 0xa1, 0x62, 0x72, 0x98, 0x7c, 0x73, 0x44, 0xfb, 0xbf, 0x3e,
    0xec, 0xf0, 0x11, 0x70, 0xd0, 0x12, 0x8b, 0x45, 0x2b, 0xc5, 0x8d, 0x02,
    0xdb, 0x55, 0x8a, 0x8d, 0x57, 0xc9, 0xaf, 0x4f, 0xc8, 0x6c, 0x46, 0xd4,
    0x5c, 0x03, 0xea, 0xed, 0x53, 0x3d, 0x0a, 0xfd, 0x4b, 0xc7, 0x25, 0x62,
    0x60, 0xe7, 0x54, 0x82, 0x95, 0x84, 0x3c, 0xdc, 0xe6, 0xec, 0x85, 0x84,
    0x74, 0xef, 0x32, 0xcd, 0x51, 0x8f, 0xcc, 0x07, 0xc2, 0xf1, 0xc3, 0xed,
    0xd7, 0xcd, 0xc1, 0xac, 0x63, 0x40, 0x59, 0x78, 0xd5, 0xc5, 0x50, 0x7a,
    0x5e, 0x5a, 0x69, 0x42, 0x18, 0xb2, 0xb3, 0x76, 0x89, 0x8f, 0xaf, 0x6f,
    0x72, 0xf5, 0xd3, 0xc1, 0x4d, 0xe0, 0xf7, 0xc3, 0xec, 0xd7, 0x82, 0xe3,
    0x28, 0x91, 0x51, 0xc8, 0x50, 0xcf, 0x78, 0xe3, 0xca, 0xd5, 0x71, 0x02,
    0xc6, 0xab, 0x4c, 0x9d, 0x11, 0x82, 0x08, 0x3b, 0xa3, 0x0c, 0xf6, 0xb8,
    0x06, 0x7c, 0x79, 0x98, 0xfd, 0x3a, 0xf0, 0x5c, 0xfb, 0xf9, 0xf5, 0xb5,
    0x10, 0xf4, 0x9d, 0xec, 0x6e, 0xff, 0x5b, 0xa0, 0x00, 0x8e, 0xfd, 0xed,
    0xc2, 0x02, 0xf0, 0x33, 0xf0, 0x43, 0x1d, 0xe4, 0x9f, 0xe2, 0x18, 0x10,
    0xfd, 0xcb, 0x99, 0xda, 0x4f, 0xe1, 0x3f, 0x8d, 0x3f, 0x00, 0x86, 0x6a,
    0x86, 0x1c, 0x3a, 0xd7, 0xf9, 0xad, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45,
    0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};


/*
 *  Constructs a KInterfaceUpDownDlg which is a child of 'parent', with the
 *  name 'name'.' 
 */
KInterfaceUpDownDlg::KInterfaceUpDownDlg( QWidget* parent,  const char* name )
    : KDialog( parent, name )
{
    QImage img;
    img.loadFromData( img0_kinterfaceupdowndlg, sizeof( img0_kinterfaceupdowndlg ), "PNG" );
    image0 = img;
    img.loadFromData( img1_kinterfaceupdowndlg, sizeof( img1_kinterfaceupdowndlg ), "PNG" );
    image1 = img;
    if ( !name )
	setName( "KInterfaceUpDownDlg" );
    setIcon( image0 );
    KInterfaceUpDownDlgLayout = new QHBoxLayout( this, 11, 6, "KInterfaceUpDownDlgLayout"); 

    pixmapLabel1 = new QLabel( this, "pixmapLabel1" );
    pixmapLabel1->setPixmap( image1 );
    pixmapLabel1->setScaledContents( TRUE );
    KInterfaceUpDownDlgLayout->addWidget( pixmapLabel1 );
    spacer12 = new QSpacerItem( 21, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    KInterfaceUpDownDlgLayout->addItem( spacer12 );

    label = new QLabel( this, "label" );
    label->setMinimumSize( QSize( 150, 0 ) );
    KInterfaceUpDownDlgLayout->addWidget( label );
    spacer14 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    KInterfaceUpDownDlgLayout->addItem( spacer14 );
    languageChange();
    resize( QSize(253, 44).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
KInterfaceUpDownDlg::~KInterfaceUpDownDlg()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void KInterfaceUpDownDlg::languageChange()
{
    setCaption( tr2i18n( "Changing Interface State" ) );
    label->setText( tr2i18n( "Bringing up interface <b>eth0</b>..." ) );
}

#include "kinterfaceupdowndlg.moc"
