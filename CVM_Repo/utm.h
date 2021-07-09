#include "./util.h"
#include <vector>
#include <opencv2/opencv.hpp>

#ifndef UTM_H
#define UTM_H

// Choose floating point precision:

// 32-bit (for Teensy 3.5/3.6 ARM boards, etc.)
//#define FLOAT_32

// 64-bit (for desktop/server use)
#define FLOAT_64

#ifdef FLOAT_64
#define FLOAT double
#define SIN sin
#define COS cos
#define TAN tan
#define POW pow
#define SQRT sqrt
#define FLOOR floor

#else
#ifdef FLOAT_32
#define FLOAT float
#define SIN sinf
#define COS cosf
#define TAN tanf
#define POW powf
#define SQRT sqrtf
#define FLOOR floorf

#endif
#endif


#include <math.h>

#define pii 3.14159265358979

/* Ellipsoid model constants (actual values here are for WGS84) */
#define sm_a 6378137.0
#define sm_b 6356752.314
#define sm_EccSquared 6.69437999013e-03

#define UTMScaleFactor 0.9996

// DegToRad
// Converts degrees to radians.
FLOAT DegToRad(FLOAT deg);

// RadToDeg
// Converts radians to degrees.
FLOAT RadToDeg(FLOAT rad);

// ArcLengthOfMeridian
// Computes the ellipsoidal distance from the equator to a point at a
// given latitude.
//
// Reference: Hoffmann-Wellenhof, B., Lichtenegger, H., and Collins, J.,
// GPS: Theory and Practice, 3rd ed.  New York: Springer-Verlag Wien, 1994.
//
// Inputs:
//     phi - Latitude of the point, in radians.
//
// Globals:
//     sm_a - Ellipsoid model major axis.
//     sm_b - Ellipsoid model minor axis.
//
// Returns:
//     The ellipsoidal distance of the point from the equator, in meters.
FLOAT ArcLengthOfMeridian (FLOAT phi);

// UTMCentralMeridian
// Determines the central meridian for the given UTM zone.
//
// Inputs:
//     zone - An integer value designating the UTM zone, range [1,60].
//
// Returns:
//   The central meridian for the given UTM zone, in radians
//   Range of the central meridian is the radian equivalent of [-177,+177].
FLOAT UTMCentralMeridian(int zone);

// FootpointLatitude
//
// Computes the footpoint latitude for use in converting transverse
// Mercator coordinates to ellipsoidal coordinates.
//
// Reference: Hoffmann-Wellenhof, B., Lichtenegger, H., and Collins, J.,
//   GPS: Theory and Practice, 3rd ed.  New York: Springer-Verlag Wien, 1994.
//
// Inputs:
//   y - The UTM northing coordinate, in meters.
//
// Returns:
//   The footpoint latitude, in radians.
FLOAT FootpointLatitude(FLOAT y);

// MapLatLonToXY
// Converts a latitude/longitude pair to x and y coordinates in the
// Transverse Mercator projection.  Note that Transverse Mercator is not
// the same as UTM; a scale factor is required to convert between them.
//
// Reference: Hoffmann-Wellenhof, B., Lichtenegger, H., and Collins, J.,
// GPS: Theory and Practice, 3rd ed.  New York: Springer-Verlag Wien, 1994.
//
// Inputs:
//    phi - Latitude of the point, in radians.
//    lambda - Longitude of the point, in radians.
//    lambda0 - Longitude of the central meridian to be used, in radians.
//
// Outputs:
//    x - The x coordinate of the computed point.
//    y - The y coordinate of the computed point.
//
// Returns:
//    The function does not return a value.
void MapLatLonToXY (FLOAT phi, FLOAT lambda, FLOAT lambda0, FLOAT &x, FLOAT &y);

// MapXYToLatLon
// Converts x and y coordinates in the Transverse Mercator projection to
// a latitude/longitude pair.  Note that Transverse Mercator is not
// the same as UTM; a scale factor is required to convert between them.
//
// Reference: Hoffmann-Wellenhof, B., Lichtenegger, H., and Collins, J.,
//   GPS: Theory and Practice, 3rd ed.  New York: Springer-Verlag Wien, 1994.
//
// Inputs:
//   x - The easting of the point, in meters.
//   y - The northing of the point, in meters.
//   lambda0 - Longitude of the central meridian to be used, in radians.
//
// Outputs:
//   phi    - Latitude in radians.
//   lambda - Longitude in radians.
//
// Returns:
//   The function does not return a value.
//
// Remarks:
//   The local variables Nf, nuf2, tf, and tf2 serve the same purpose as
//   N, nu2, t, and t2 in MapLatLonToXY, but they are computed with respect
//   to the footpoint latitude phif.
//
//   x1frac, x2frac, x2poly, x3poly, etc. are to enhance readability and
//   to optimize computations.
void MapXYToLatLon (FLOAT x, FLOAT y, FLOAT lambda0, FLOAT& phi, FLOAT& lambda);

// LatLonToUTMXY
// Converts a latitude/longitude pair to x and y coordinates in the
// Universal Transverse Mercator projection.
//
// Inputs:
//   lat - Latitude of the point, in radians.
//   lon - Longitude of the point, in radians.
//   zone - UTM zone to be used for calculating values for x and y.
//          If zone is less than 1 or greater than 60, the routine
//          will determine the appropriate zone from the value of lon.
//
// Outputs:
//   x - The x coordinate (easting) of the computed point. (in meters)
//   y - The y coordinate (northing) of the computed point. (in meters)
//
// Returns:
//   The UTM zone used for calculating the values of x and y.
int LatLonToUTMXY (FLOAT lat, FLOAT lon, int zone, FLOAT& x, FLOAT& y);

// UTMXYToLatLon
//
// Converts x and y coordinates in the Universal Transverse Mercator//   The UTM zone parameter should be in the range [1,60].

// projection to a latitude/longitude pair.
//
// Inputs:
// x - The easting of the point, in meters.
// y - The northing of the point, in meters.
// zone - The UTM zone in which the point lies.
// southhemi - True if the point is in the southern hemisphere;
//               false otherwise.
//
// Outputs:
// lat - The latitude of the point, in radians.
// lon - The longitude of the point, in radians.
//
// Returns:
// The function does not return a value.
void UTMXYToLatLon (FLOAT x, FLOAT y, int zone, bool southhemi, FLOAT& lat, FLOAT& lon);

// Resolution
//
// Given the UTM cooridnate of leftupper and rightbottum corner of image, calculate
// the resolution along east(x) and north(y) axis of UTM coodinate.
//
// Inputs:
// resolution - The resolution with initial value, in coordinate structure.
// imgRowCol - The rows and cols of image, in coordinate structure.
// UTMLeftUpper - The leftupper corner of image, in UTM coordinate.
// UTMRightBottum - The rightbottum corner of image, in UTM coordinate.
//
// Outputs:
//resolution - The resolution along east(x) and north(y) axis of UTM coodinate.
//
// Returns:
// The function does not return a value.
coordinate Resolution(cv::Mat img, coordinate LatLonLeftUpper, coordinate LatLonRightBottom, int zone, bool isSouth);

// UTMxyToImgUV
//
// Given the resolution, UTM coodinate of pixel and leftupper corner in UTM cooridnate,
// calculate the coordinate of this pixel in image plane
//
// Inputs:
// resolution - The resolution along east(x) and north(y) axis of UTM coodinate.
// UTMLeftUpper - The leftupper corner of image, in UTM coordinate.
// imgUV - The cooridinate of a pixel in image plane.
// UTMxy - The coordinate of the same pixel in UTM coodinate.
//
// Outputs:
// UTMxy - The coordinate of this pixel in image plane
//
// Returns:
// The function does not return a value.
void UTMxyToImgUV(coordinate resolution, coordinate UTMLeftUpper, coordinate UTMxy, coordinate& imgUV);

// ImgUVToUTMxy
//
// Given the resolution, pixel coordinate in image plane and leftupper corner in UTM cooridnate,
// calculate the UTM coordinate of this pixel
//
// Inputs:
// resolution - The resolution along east(x) and north(y) axis of UTM coodinate.
// UTMLeftUpper - The leftupper corner of image, in UTM coordinate.
// imgUV - The cooridinate of a pixel in image plane.
// UTMxy - The coordinate of the same pixel in UTM coodinate.
//
// Outputs:
// UTMxy - The coordinate of the same pixel in UTM coodinate.
//
// Returns:
// The function does not return a value.
void ImgUVToUTMxy(coordinate resolution, coordinate UTMLeftUpper, coordinate imgUV, coordinate& UTMxy);

// LatLonToImgUV
//
// Given the satellite image, LeftUpper and RightBottom corner coordinate in LatLon,
// UTM zone and semiglobe region,
// For a given LatLon,
// output the coordinate of pixel in image plane
coordinate LatLonToImgUV(cv::Mat img, coordinate LatLonLeftUpper, coordinate LatLonRightBottom, int zone, bool isSouth, coordinate LatLon);

// ImgUVToLatLon
//
// Given the satellite image, LeftUpper and RightBottom corner coordinate in LatLon,
// UTM zone and semiglobe region,
// For a given pixel,
// output the coordinate of pixel in LatLon
coordinate ImgUVToLatLon(cv::Mat img, coordinate LatLonLeftUpper, coordinate LatLonRightBottom, int zone, bool isSouth, coordinate imgUV);

#endif // UTM_H
