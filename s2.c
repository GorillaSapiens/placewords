// not REALLY S2, but you will find this has many similarities...

// TODO FIX: look for "TODO S2" and make corrections

#include <stdio.h>
#include <stdint.h>
#include <math.h>

// TODO S2
#define FACE_XPOS 0
#define FACE_XNEG 1
#define FACE_YPOS 2
#define FACE_YNEG 3
#define FACE_ZPOS 4
#define FACE_ZNEG 5

#define debug printf

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
   debug("face = %ld\n", result);
   debug("u = %lf\n", u);
   debug("v = %lf\n", v);

   // u and v now range [-1,1]

   double s = (1.0L + 4.0L * M_1_PI * atan(u)) / 2.0L;
   debug("s = %lf\n", s);

   double t = (1.0L + 4.0L * M_1_PI * atan(v)) / 2.0L;
   debug("t = %lf\n", t);

   for (int i = 0; i < 30; i++) {
      result <<= 2;
      if (s >= .5L) {
         result |= 2LL;
         s -= 0.5L;
      }
      s *= 2.0L;
      if (t >= .5L) {
         result |= 1LL;
         t -= 0.5L;
      }
      t *= 2.0L;
   }

   debug("s2=%llX\n", result);

   return result;
}

// caution, returns static pointer to 2 ints, overwritten per call
int *s2_to_ll(uint64_t s2) {
   static int result[2] = { 0, 0 }; // lat, lon

   double s = 0.0L, t = 0.0L;

   for (int i = 0; i < 30; i++) {
      s /= 2.0L;
      t /= 2.0L;
      if (s2 & 2LL) {
         s = s + 0.5L;
      }
      if (s2 & 1LL) {
         t = t + 0.5L;
      }
      s2 >>= 2;
   }
   debug("s = %lf\n", s);
   debug("t = %lf\n", t);

   double u = tan(((s * 2.0L - 1.0L) * M_PI) / 4.0L);
   debug("u = %lf\n", u);

   double v = tan(((t * 2.0L - 1.0L) * M_PI) / 4.0L);
   debug("v = %lf\n", v);

   double x, y, z;

   debug("face = %ld\n", s2);
   switch(s2) {
      case FACE_XPOS:
         x = 1.0;
         y = u;
         z = v;
         break;
      case FACE_XNEG:
         x = -1.0;
         y = u;
         z = v;
         break;
      case FACE_YPOS:
         x = v;
         y = 1.0;
         z = u;
         break;
      case FACE_YNEG:
         x = v;
         y = -1.0;
         z = u;
         break;
      case FACE_ZPOS:
         x = u;
         y = v; 
         z = 1.0;
         break;
      case FACE_ZNEG:
         x = u;
         y = v; 
         z = -1.0;
         break;
   }

   double r = sqrt(x*x + y*y + z*z);
   x /= r;
   y /= r;
   z /= r;

   double lat, lon;

   lon = atan2(y, x);
   lat = atan(sqrt(x * x + y * y) / z);

   result[0] = lat * 1000000.0;
   result[1] = lon * 1000000.0;

   return result;
}

void main(void) {
   int latE6 = 37749000;
   int lonE6 = -122419400;

   uint64_t s2 = ll_to_s2(latE6, lonE6);

   int *ret = s2_to_ll(s2);

   printf("%d %d\n%d %d\n", latE6, lonE6, ret[0], ret[1]);

}
