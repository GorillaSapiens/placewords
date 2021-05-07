// not REALLY S2, but you will find this has many similarities...

// TODO FIX: look for "TODO S2" and make corrections

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

// TODO S2
#define FACE_XPOS 0 // africa
#define FACE_YPOS 1 // asia
#define FACE_ZPOS 2 // north
#define FACE_XNEG 3 // pacific
#define FACE_YNEG 4 // americas
#define FACE_ZNEG 5 // south

#define DEG_TO_RAD (M_PI / 180.0L)
#define RAD_TO_DEG (180.0L / M_PI)

#ifdef TEST
#define debug printf
#define EPSILON 6
#else
#define debug(x, ...)
#endif

uint64_t ll_to_s2(int latE6, int lonE6) {
   double lat = ((double) latE6) / 1000000.0L;
   debug("lat = %lf\n", lat);
   lat *= DEG_TO_RAD;

   double lon = ((double) lonE6) / 1000000.0L;
   debug("lon = %lf\n", lon);
   lon *= DEG_TO_RAD;

   double x = cos(lat) * cos(lon);
   double y = cos(lat) * sin(lon);
   double z = sin(lat);
   debug("x = %lf\n", x);
   debug("y = %lf\n", y);
   debug("z = %lf\n", z);

   double ratio;
   double u, v;

   uint64_t result;

   if (fabs(x) >= fabs(y) && fabs(x) >= fabs(z)){
      ratio = fabs(x);
      if (x >= 0.0L) {
         result = FACE_XPOS;
         u = y / ratio;
         v = z / ratio;
      }
      else {
         result = FACE_XNEG;
         u = -y / ratio;
         v = -z / ratio;
      }
   }
   else if (fabs(y) >= fabs(z) && fabs(y) >= fabs(x)){
      ratio = fabs(y);
      if (y >= 0.0L) {
         result = FACE_YPOS;
         u = z / ratio;
         v = -x / ratio;
      }
      else {
         result = FACE_YNEG;
         u = -z / ratio;
         v = x / ratio;
      }
   }
   else if (fabs(z) >= fabs(x) && fabs(z) >= fabs(y)){
      ratio = fabs(z);
      if (z >= 0.0L) {
         result = FACE_ZPOS;
         u = -x / ratio;
         v = -y / ratio;
      }
      else {
         result = FACE_ZNEG;
         u = x / ratio;
         v = y / ratio;
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

   debug("s2=%lX\n", result);

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
         y = -u;
         z = -v;
         break;
      case FACE_YPOS:
         x = -v;
         y = 1.0;
         z = u;
         break;
      case FACE_YNEG:
         x = v;
         y = -1.0;
         z = -u;
         break;
      case FACE_ZPOS:
         x = -u;
         y = -v; 
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
   debug("x = %lf\n", x);
   debug("y = %lf\n", y);
   debug("z = %lf\n", z);

   double lat, lon;

   lat = asin(z);
   lat *= RAD_TO_DEG;
   debug("lat = %lf\n", (double) lat);

   lon = atan2(y, x);
   lon *= RAD_TO_DEG;
   debug("lon = %lf\n", ((double) lon));

   result[0] = lat * 1000000.0;
   result[1] = lon * 1000000.0;

   return result;
}

#ifdef TEST
void main(int argc, char **argv) {
   int latE6 = 37749000;
   int lonE6 = -122419400;

   if (argc > 1) {
      latE6 = atoi(argv[1]);
      lonE6 = atoi(argv[2]);
   }

   uint64_t s2 = ll_to_s2(latE6, lonE6);
   int *ret = s2_to_ll(s2);
   printf("##\n# %d %d\n# %d %d\n", latE6, lonE6, ret[0], ret[1]);

   for (latE6 = -89000000; latE6 < 90000000; latE6 += 1000000) {
      for (lonE6 = -179000000; lonE6 < 180000000; lonE6 += 1000000) {
         s2 = ll_to_s2(latE6, lonE6);
         ret = s2_to_ll(s2);
         printf("##\n# %d %d\n# %d %d\n", latE6, lonE6, ret[0], ret[1]);
         if(abs(latE6-ret[0]) > EPSILON || abs(lonE6-ret[1]) > EPSILON) {
            printf("OOPS\n");
            exit(-1);
         }
      }
   }
}
#endif
