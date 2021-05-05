// not REALLY S2, but you will find this has many similarities...

// TODO FIX: look for "TODO S2" and make corrections

#include <stdint.h>
#include <math.h>

// TODO S2
#define FACE_XPOS 0
#define FACE_XNEG 1
#define FACE_YPOS 2
#define FACE_YNEG 3
#define FACE_ZPOS 4
#define FACE_ZNEG 5

uint64_t ll_to_s2(int latE6, int lonE6) {
   double lat = ((double) latE6) / 100000.0L;
   double lon = ((double) lonE6) / 100000.0L;

   double x = cos(lat) * cos(lon);
   double y = cos(lat) * sin(lon);
   double z = sin(lat);

   double ratio;
   double u, v;

   uint64_t result;

   if (fabs(x) >= fabs(y) && fabs(x) >= fabs(z)){
      ratio = fabs(x);
      u = y / ratio;
      v = z / ratio;
      if (x < 0) {
         result = FACE_XNEG;
      }
      else {
         result = FACE_XPOS;
      }
   }
   else if (fabs(y) >= fabs(z) && fabs(y) >= fabs(x)){
      ratio = fabs(y);
      u = z / ratio;
      v = x / ratio;
      if (y < 0) {
         result = FACE_YNEG;
      }
      else {
         result = FACE_YPOS;
      }
   }
   else if (fabs(z) >= fabs(x) && fabs(z) >= fabs(y)){
      ratio = fabs(z);
      u = x / ratio;
      v = y / ratio;
      if (z < 0) {
         result = FACE_ZNEG;
      }
      else {
         result = FACE_ZPOS;
      }
   }

   // u and v now range [-1,1]

   double s = 4.0L * M_1_PI * atan(u);
   double t = 4.0L * M_1_PI * atan(v);

   s = 1.0L + (s + 1.0L) / 2.0L;
   t = 1.0L + (s + 1.0L) / 2.0L;

   for (int i = 0; i < 30; i++) {
      result <<= 2;
      if (s >= .5L) {
         result |= 2LL;
      }
      if (t >= .5L) {
         result |= 1LL;
      }
   }

   return result;
}


