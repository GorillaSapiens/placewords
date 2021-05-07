// S2 from scratch, there may be bugs here where
// things don't line up perfectly w/S2, but it's
// close enough to use for debugging placewords
// without pulling in the full weight of the S2
// library.

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

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
#define EPSILON 3
#else
#define debug(x, ...)
#endif

#define QUADRATIC

// forward convert from latE6, lonE6 to 64 bit S2
uint64_t ll_to_s2(int latE6, int lonE6) {
   double lat = ((double) latE6) / 1000000.0L;
   debug("lat = %.16lf\n", lat);
   lat *= DEG_TO_RAD;

   double lon = ((double) lonE6) / 1000000.0L;
   debug("lon = %.16lf\n", lon);
   lon *= DEG_TO_RAD;

   double x = cos(lat) * cos(lon);
   double y = cos(lat) * sin(lon);
   double z = sin(lat);
   debug("x = %.16lf\n", x);
   debug("y = %.16lf\n", y);
   debug("z = %.16lf\n", z);

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
   debug("u = %.16lf\n", u);
   debug("v = %.16lf\n", v);

   // u and v now range [-1,1]

#if TANGENT
   double s = (1.0L + 4.0L * M_1_PI * atan(u)) / 2.0L;
   double t = (1.0L + 4.0L * M_1_PI * atan(v)) / 2.0L;
#endif
#ifdef QUADRATIC
   double s = (u >= 0.0L) ? (0.5L * sqrt(1.0L + 3.0L * u)) :
         (1.0L - 0.5L * sqrt(1.0L - 3.0L * u));
   double t = (v >= 0.0L) ? (0.5L * sqrt(1.0L + 3.0L * v)) :
         (1.0L - 0.5L * sqrt(1.0L - 3.0L * v));
#endif
   debug("s = %.16lf\n", s);
   debug("t = %.16lf\n", t);

   for (int i = 0; i < 30; i++) {
      // this bit is mind bending...
      // see https://en.wikipedia.org/wiki/Hilbert_curve#/media/File:Hilbert_curve_production_rules!.svg

      result <<= 2;

      s *= 2.0L;
      t *= 2.0L;

      int hilbert = (s >= 1.0L ? 3 : 0) ^ (t >= 1.0L ? 1 : 0);
      result |= (uint64_t) hilbert;

      double tmp;

      switch (hilbert) {
         case 0:
            tmp = s;
            s = t;
            t = tmp;
            break;
         case 1:
            t -= 1.0L;
            break;
         case 2:
            s -= 1.0L;
            t -= 1.0L;
            break;
         case 3:
            s -= 1.0L;
            tmp = s;
            s = 1.0L - t;
            t = 1.0L - tmp;
            break;
      }
   }

   result <<= 1;
   result |= 1LL;

   debug("s2=%lX\n", result);

   return result;
}

int double_to_E6(double in) {
   long double integer;
   long double extra = modfl(in * 1000000.0L, &integer);
   if (extra > .5L) {
      integer += 1.0L;
   }
   else if (extra < -.5L) {
      integer -= 1.0L;
   }

   return (int) integer;
}

// backward convert from 64 bit S2 to latE6, lonE6
// caution, returns static pointer to 2 ints, overwritten per call
int *s2_to_ll(uint64_t s2) {
   static int result[2] = { 0, 0 }; // lat, lon

   s2 >>= 1;

   // i had s=t=0 here, but that led to more errors than
   // using 0.5L as a starting value
   double s = 0.5L, t = 0.5L;
   double tmp;

   for (int i = 0; i < 30; i++) {
      // ... unbending mind is bit this
      switch (s2 & 3) {
         case 0:
            tmp = s;
            s = t;
            t = tmp;
            break;
         case 1:
            t += 1.0L;
            break;
         case 2:
            s += 1.0L;
            t += 1.0L;
            break;
         case 3:
            tmp = s;
            s = 1.0L - t;
            t = 1.0L - tmp;
            s += 1.0L;
            break;
      }

      s /= 2.0L;
      t /= 2.0L;

      s2 >>= 2;
   }
   debug("s = %.16lf\n", s);
   debug("t = %.16lf\n", t);

#ifdef TANGENT
   double u = tan(((s * 2.0L - 1.0L) * M_PI) / 4.0L);
   double v = tan(((t * 2.0L - 1.0L) * M_PI) / 4.0L);
#endif
#ifdef QUADRATIC
   double u = (s >= 0.5L) ? ((4.0L * s * s - 1) / 3.0L) :
      ((1.0L - 4.0L * (1.0L - s) * (1.0L - s)) / 3.0L);
   double v = (t >= 0.5L) ? ((4.0L * t * t - 1) / 3.0L) :
      ((1.0L - 4.0L * (1.0L - t) * (1.0L - t)) / 3.0L);
#endif
   debug("u = %.16lf\n", u);
   debug("v = %.16lf\n", v);

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
   debug("x = %.16lf\n", x);
   debug("y = %.16lf\n", y);
   debug("z = %.16lf\n", z);

   double lat, lon;

   lat = asin(z);
   lat *= RAD_TO_DEG;
   debug("lat = %.16lf\n", (double) lat);

   lon = atan2(y, x);
   lon *= RAD_TO_DEG;
   debug("lon = %.16lf\n", ((double) lon));

   result[0] = double_to_E6(lat);
   result[1] = double_to_E6(lon);

   return result;
}

#ifdef TEST
struct {
   int latE6, lonE6;
   uint64_t s2;
} vector[] = {
   { 33533333, -7583333, 983529568086062495LL }, // 0, casablanca
   { 28613895, 77209006, 4110909809715404065LL }, // 1, new delhi
   { 55755833, 37617222, 5095060306123625331LL }, // 2, moscow
   { -27467778, 153028056, 7751075513880745065LL }, // 3, brisbane
   { 37749000, -122419400, -9182982296397125021LL }, // 4, san francisco
   { -77846323,166668235, -5803106435262993021LL } // 5, mcmurdo
};

void main(int argc, char **argv) {
   for (int i = 0; i < 6; i++) {
      uint64_t s2 = ll_to_s2(vector[i].latE6, vector[i].lonE6);
      int *ret = s2_to_ll(s2);
      printf("####\n# %d %d\n# %d %d\n", vector[i].latE6, vector[i].lonE6, ret[0], ret[1]);
      printf("# %016lx %016lx\n", vector[i].s2, s2);
      printf("# %016lx %016lx\n", vector[i].s2>>1, s2>>1);
   }

   // test the planet in 1 degree increments, worldwide
   // we skip latitude +/- 90, because of ambiguity
   // ditto for longitude -180
   int imperfect = 0;
   for (int latE6 = -89000000; latE6 < 90000000; latE6 += 1000000) {
      for (int lonE6 = -179000000; lonE6 < 180000000; lonE6 += 1000000) {
         uint64_t s2 = ll_to_s2(latE6, lonE6);
         int *ret = s2_to_ll(s2);
         printf("##\n# %d %d\n# %d %d\n", latE6, lonE6, ret[0], ret[1]);
         if(abs(latE6-ret[0]) > EPSILON || abs(lonE6-ret[1]) > EPSILON) {
            printf("OOPS\n");
            exit(-1);
         }
         if(abs(latE6-ret[0]) > 0 || abs(lonE6-ret[1]) > 0) {
            printf("CLOSE %d %d %d %d\n", latE6, lonE6, ret[0], ret[1]);
            imperfect++;
         }
      }
   }
   printf("%d imperfect\n", imperfect);
}
#endif
