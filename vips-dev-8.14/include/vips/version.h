/* Macros for the header version.
 */

#ifndef VIPS_VERSION_H
#define VIPS_VERSION_H

#define VIPS_VERSION		"8.14.2"
#define VIPS_VERSION_STRING	"8.14.2"
#define VIPS_MAJOR_VERSION	(8)
#define VIPS_MINOR_VERSION	(14)
#define VIPS_MICRO_VERSION	(2)

/* The ABI version, as used for library versioning.
 */
#define VIPS_LIBRARY_CURRENT	(58)
#define VIPS_LIBRARY_REVISION	(2)
#define VIPS_LIBRARY_AGE	(16)

#define VIPS_CONFIG		"enable debug: false\nenable deprecated: false\nenable modules: true\nenable cplusplus: true\nenable RAD load/save: true\nenable Analyze7 load/save: true\nenable PPM load/save: true\nenable GIF load: true\nuse fftw for FFTs: true\naccelerate loops with ORC: true\nICC profile support with lcms: true\nzlib: true\ntext rendering with pangocairo: true\nfont file support with fontconfig: true\nEXIF metadata support with libexif: true\nJPEG load/save with libjpeg: true\nJXL load/save with libjxl: true (dynamic module: true)\nJPEG2000 load/save with OpenJPEG: true\nPNG load/save with libspng: true\nPNG load/save with libpng: false\nselected quantisation package: imagequant\nTIFF load/save with libtiff: true\nimage pyramid save with libgsf: false\nHEIC/AVIF load/save with libheif: true (dynamic module: false)\nWebP load/save with libwebp: true\nPDF load with PDFium: false\nPDF load with poppler-glib: true (dynamic module: true)\nSVG load with librsvg: true\nEXR load with OpenEXR: true\nOpenSlide load: true (dynamic module: true)\nMatlab load with libmatio: true\nNIfTI load/save with niftiio: true\nFITS load/save with cfitsio: true\nGIF save with cgif: true\nselected Magick package: MagickCore (dynamic module: true)\nMagick API version: magick6\nMagick load: true\nMagick save: true"

/* Not really anything to do with versions, but this is a handy place to put
 * it.
 */
#define VIPS_ENABLE_DEPRECATED 0

#endif /*VIPS_VERSION_H*/
