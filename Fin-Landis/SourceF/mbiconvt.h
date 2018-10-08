#ifndef MBICONVT_H
#define MBICONVT_H

/* mbiconvt.h - Macros for converting numbers between file & ram format.

       These are macros for converting integer and real numbers
       between their formats in an image file on disk and their
       formats in ram.  They will vary depending on the machine
       and operating system used.

       This version is desgined for converting numbers between
       the format of ERSC or PC ERDAS 7.4 files and the internal
       representation on the RS/6000. This conversion requires
       byte order reversal, because the ERDAS files have low
       order bytes first and the RS/6000 numbers have high order
       bytes first.

       Each macro name consists of 3 parts.

       1) The file type for which it is written.
              ERD indicates ERDAS files.
              ERS indicates ERSC files.

       2) The source type abreviation.

       3) The destination type abreviation.

       The source and destination abreivations depend on whether
       the source and destinations are in the file or in ram.

       File abreviations:
              n1 - first nibble of a byte
              n2 - second nibble of a byte
              i1 - one byte unsigned integer.
              i2 - two byte integer.
              i4 - four byte integer.
              r4 - four byte real number.

       Ram abreviations:
              i - C type int.
              l - C type long.
              f - C type float.
              c - The coordinate type coord.
              p - Pixel value type pixval.

       It is assumed that data coming from or going to the file
       is stored in a buffer of unsigned char. The source or
       destination argument for file data is the address of the
       first byte in this buffer where the data item is taken
       from or placed.

       The source or destination argument corresponding to the
       data in ram is the variable itself, not its address.

       WARNING: Nibble conversions need verification of their
              order. I may have the first and second nibbles
              reversed.

   Copyright 1990, Pete Weiler & Univ. of Wis. Env. Remote Sensing Center

       $Revision:   1.0.1.0  $
       $Modtime:   12 Jul 1991 09:51:42  $

*/

/* This is the macro for reversing the order of 2 bytes.
       The source and destination pointers must be different. */

#define swap2bytes(srcptr, destptr) *((char*)destptr) = *(((char*)srcptr)+1);\
	      *(((char*)destptr)+1) = *((char*)srcptr)

/* This is the macro for reversing the order of 4 bytes.
       The source and destination pointers must be different. */

#define swap4bytes(srcptr, destptr) *((char*)destptr) = *(((char*)srcptr)+3); \
	      *(((char*)destptr)+1) = *(((char*)srcptr)+2); \
	      *(((char*)destptr)+2) = *(((char*)srcptr)+1); \
	      *(((char*)destptr)+3) = *((char*)srcptr)

/* ERDAS file to RS/6000 ram conversions. */

#define ERDi2_i(srcptr, dest)  dest = 0; swap2bytes(srcptr, (((char*)&dest)+2))

#define ERDi4_l(srcptr, dest)  swap4bytes(srcptr, &dest)

#define ERDr4_f(srcptr, dest)  swap4bytes(srcptr, &dest)

#define ERDi4_c(srcptr, dest)  swap4bytes(srcptr, &dest)

#define ERDi2_p(srcptr, dest)  swap2bytes(srcptr, &dest)

#define ERDi1_p(srcptr, dest)  dest = (pixval)(*srcptr)

#define ERDn1_p(srcptr, dest)  dest = (pixval)((*srcptr) & 15)

#define ERDn2_p(srcptr, dest)  dest = (pixval)(((*srcptr)>>4) & 15)


/* RS/6000 ram to ERDAS file conversions. */

#define ERDi_i2(src, destptr)  swap2bytes((((char*)&src)+2), destptr)

#define ERDl_i4(src, destptr)  swap4bytes(&src, destptr)

#define ERDf_r4(src, destptr)  swap4bytes(&src, destptr)

#define ERDc_i4(src, destptr)  swap4bytes(&src, destptr)

#define ERDp_i2(src, destptr)  swap2bytes(&src, destptr)

#define ERDp_i1(src, destptr)  *destptr = (unsigned char)((src) & 255)

#define ERDp_n1(src, destptr)  *destptr = ((unsigned char)src & 15) | (*(destptr) & 240)

#define ERDp_n2(src, destptr)  *destptr = ((unsigned char)((15 & src)<<4)) | (*destptr & 15)



/* ERSC file to ram conversions. */

#define ERSi2_p(srcptr, dest)  swap2bytes(srcptr, &dest)

#define ERSi1_p(srcptr, dest)  dest = (pixval)(*srcptr)


/* RS/6000 ram to ERSC file conversions. */

#define ERSp_i2(src, destptr)  swap2bytes(&src, destptr)

#define ERSp_i1(src, destptr)  *destptr = (unsigned char)((src) & 255)

#endif

