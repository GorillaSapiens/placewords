// Copyright 2021 Adam Wozniak All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS-IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Author: adam@wozniakconsulting.com (Adam Wozniak)

// S2 from scratch, there may be bugs here where
// things don't line up perfectly w/S2, but it's
// close enough to use for debugging placewords
// without pulling in the full weight of the S2
// library.

// caveat emptor: I did this mostly to learn a little rust, there may be
// glaring newbie errors here.

use std::process::exit;
use std::f64::consts::PI;

const FACE_XPOS : u64 = 0; // africa
const FACE_YPOS : u64 = 1; // asia
const FACE_ZPOS : u64 = 2; // north
const FACE_XNEG : u64 = 3; // pacific
const FACE_YNEG : u64 = 4; // americas
const FACE_ZNEG : u64 = 5; // south

const DEG_TO_RAD : f64 = PI / 180.0_f64;
const RAD_TO_DEG : f64 = 180.0_f64 / PI;

fn double_to_e6(arg : f64) -> i32 {
   // this is stupidly complex
   let mid = (arg * 1000000.0_f64) as i32;
   let up = mid + 1;
   let down = mid - 1;

   let delta_mid : f64 = (arg - (mid as f64 / 1000000_f64)).abs();
   let delta_up : f64 = (arg - (up as f64 / 1000000_f64)).abs();
   let delta_down : f64 = (arg - (down as f64 / 1000000_f64)).abs();
   
   let mut result = mid;

   if delta_up < delta_mid { result = up; }
   if delta_down < delta_mid { result = down; }

   result
}

fn s2_to_ll(arg : u64) -> [i32; 2] {
   let mut s2 = arg;
   let mut result : [i32; 2] = [0 ; 2];

   println!("s2={:018X} {}", s2, s2);

   s2 >>= 1;

   let mut s : f64 =0.0_f64;
   let mut t : f64 =0.0_f64;

   for _i in 0 .. 30 {
      // ... unbending mind is bit this
      match s2 & 3 {
         0 => {
            let tmp = s;
            s = t;
            t = tmp;
         },
         1 => {
            t += 1.0_f64;
         },
         2 => {
            s += 1.0_f64;
            t += 1.0_f64;
         },
         3 => {
            let tmp = s;
            s = 1.0_f64 - t;
            t = 1.0_f64 - tmp;
            s += 1.0_f64;
         },
         _ => {
            println!("hilbert={}, can never happen", s2 & 3);
            exit(-1)
         },
      }

      s /= 2.0_f64;
      t /= 2.0_f64;

      s2 >>= 2;
   }
   println!("s = {}", s);
   println!("t = {}", t);

   // // TANGENT, do not use
   // let u : f64 = (((s * 2.0_f64 - 1.0_f64) * PI) / 4.0_f64).tan();
   // let v : f64 = (((t * 2.0_f64 - 1.0_f64) * PI) / 4.0_f64).tan();

   // QUADRATIC
   let u : f64 =
      if s >= 0.5_f64 { (4.0_f64 * s * s - 1_f64) / 3.0_f64 }
      else { (1.0_f64 - 4.0_f64 * (1.0_f64 - s) * (1.0_f64 - s)) / 3.0_f64 };
   let v : f64 =
      if t >= 0.5_f64 { (4.0_f64 * t * t - 1_f64) / 3.0_f64 }
      else { (1.0_f64 - 4.0_f64 * (1.0_f64 - t) * (1.0_f64 - t)) / 3.0_f64 };

   println!("u = {}", u);
   println!("v = {}", v);

   let mut x : f64;
   let mut y : f64;
   let mut z : f64;

   println!("face = {}", s2);

   match s2 {
      FACE_XPOS => {
         x = 1.0;
         y = u;
         z = v;
      },
      FACE_XNEG => {
         x = -1.0;
         y = -u;
         z = -v;
      },
      FACE_YPOS => {
         x = -v;
         y = 1.0;
         z = u;
      },
      FACE_YNEG => {
         x = v;
         y = -1.0;
         z = -u;
      },
      FACE_ZPOS => {
         x = -u;
         y = -v;
         z = 1.0;
      },
      FACE_ZNEG => {
         x = u;
         y = v;
         z = -1.0;
      },
      _ => {
         println!("face={}, can never happen", s2);
         exit(-1)
      }
   }

   let r : f64 = (x*x + y*y + z*z).sqrt();
   x /= r;
   y /= r;
   z /= r;
   println!("x = {}", x);
   println!("y = {}", y);
   println!("z = {}", z);

   let mut lat : f64;
   let mut lon : f64;

   lat = z.asin();
   lat *= RAD_TO_DEG;
   println!("lat = {}", lat);

   lon = y.atan2(x);
   lon *= RAD_TO_DEG;
   println!("lon = {}", lon);
 
   result[0] = double_to_e6(lat);
   result[1] = double_to_e6(lon);

   result
}

fn ll_to_s2(lat_e6: i32, lon_e6: i32) -> u64 {
   let mut lat : f64 = lat_e6 as f64 / 1000000.0_f64;
   println!("lat = {}", lat);
   lat *= DEG_TO_RAD;

   let mut lon : f64 = lon_e6 as f64 / 1000000.0_f64;
   println!("lon = {}", lon);
   lon *= DEG_TO_RAD;

   let x : f64 = lat.cos() * lon.cos();
   let y : f64 = lat.cos() * lon.sin();
   let z : f64 = lat.sin();
   println!("x = {}", x);
   println!("y = {}", y);
   println!("z = {}", z);

   let ratio : f64;
   let mut u : f64 = 0_f64;
   let mut v : f64 = 0_f64;

   let mut result : u64 = 0_u64;

   if x.abs() >= y.abs() && x.abs() >= z.abs() {
      ratio = x.abs();
      if x >= 0_f64 {
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
   else if y.abs() >= z.abs() && y.abs() >= x.abs() {
      ratio = y.abs();
      if y >= 0_f64 {
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
   else if z.abs() >= x.abs() && z.abs() >= y.abs() {
      ratio = z.abs();
      if z >= 0_f64 {
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
   println!("face = {}", result);
   println!("u = {}", u);
   println!("v = {}", v);

   // u and v now range [-1,1]

   // // TANGENT, do not use
   // let s = (1.0_f64 + 4.0_f64 * (1.0_f64 / PI) * u.atan()) / 2.0_f64;
   // let t = (1.0_f64 + 4.0_f64 * (1.0_f64 / PI) * v.atan()) / 2.0_f64;

   // QUADRATIC
   let mut s : f64 =
      if u >= 0.0_f64 { 0.5_f64 * (1.0_f64 + 3.0_f64 * u).sqrt() }
      else { 1.0_f64 - 0.5_f64 * (1.0_f64 - 3.0_f64 * u).sqrt() };
   let mut t : f64 =
      if v >= 0.0_f64 { 0.5_f64 * (1.0_f64 + 3.0_f64 * v).sqrt() }
      else { 1.0_f64 - 0.5_f64 * (1.0_f64 - 3.0_f64 * v).sqrt() };
   println!("s = {}", s);
   println!("t = {}", t);

   for _i in 0 .. 30 {
      // this bit is mind bending...
      // see https://en.wikipedia.org/wiki/Hilbert_curve#/media/File:Hilbert_curve_production_rules!.svg

      result <<= 2;

      s *= 2.0_f64;
      t *= 2.0_f64;

      let hilbert : u64 = (if s >= 1.0_f64 { 3_u64 } else { 0_u64 }) ^
                          (if t >= 1.0_f64 { 1_u64 } else { 0_u64 });
      result |= hilbert;

      match hilbert {
         0 => {
            let tmp = s;
            s = t;
            t = tmp; },
         1 => {
            t -= 1.0_f64; },
         2 => {
            s -= 1.0_f64;
            t -= 1.0_f64; },
         3 => {
            s -= 1.0_f64;
            let tmp = s;
            s = 1.0_f64 - t;
            t = 1.0_f64 - tmp; },
         _ => {
            println!("hilbert={}, can never happen", hilbert);
            exit(-1) },
      }
   }

   result <<= 1;
   result |= 1_u64;

   println!("s2={:#018X} {}", result, result as i64);

   result
}

struct TestStruct {
   lat_e6 : i32,
   lon_e6 : i32,
   s2 : u64,
}

const TEST_VECTOR: [ TestStruct; 6] = [
   TestStruct {
      lat_e6: 33533333,
      lon_e6: -7583333,
      s2: 983529568086062495 // 0, casablanca
   },
   TestStruct {
      lat_e6: 28613895,
      lon_e6: 77209006,
      s2: 4110909809715404065 // 1, new delhi
   },
   TestStruct {
      lat_e6: 55755833,
      lon_e6: 37617222,
      s2: 5095060306123625331 // 2, moscow
   },
   TestStruct {
      lat_e6: -27467778,
      lon_e6: 153028056,
      s2: 7751075513880745065 // 3, brisbane
   },
   TestStruct {
      lat_e6: 37749000,
      lon_e6: -122419400,
      s2: -9182982296397125021_i64 as u64 // 4, san francisco
   },
   TestStruct {
      lat_e6: -77846323,
      lon_e6: 166668235,
      s2: -5803106435262993021_i64 as u64 // 5, mcmurdo
   },
];

fn main() {
   for i in 0 .. 6 {
      println!("===");
      println!("ll={} {}", TEST_VECTOR[i].lat_e6, TEST_VECTOR[i].lon_e6);
      let s2 : u64 = ll_to_s2(TEST_VECTOR[i].lat_e6, TEST_VECTOR[i].lon_e6);
      println!("s2={:#018X} {}", TEST_VECTOR[i].s2, TEST_VECTOR[i].s2 as i64);
      let ll : [i32; 2] = s2_to_ll(s2);
      println!("ll={} {}", ll[0], ll[1]);
   }

   // test the planet in 1 degree increments, worldwide
   // we skip latitude +/- 90, because of ambiguity
   // ditto for longitude -180
   let mut imperfect : i32 = 0;
   let mut total : i32 = 0;

   const EPSILON : i32 = 6;

   for lat_e6 in (-89_000_000..90_000_000).step_by(1_000_000) {
      for lon_e6 in (-179_000_000..180_000_000).step_by(1_000_000) {
         total += 1;
         let s2 : u64 = ll_to_s2(lat_e6, lon_e6);
         let ll : [ i32; 2] = s2_to_ll(s2);

         let d1 : i32 = (lat_e6 - ll[0]).abs();
         let d2 : i32 = (lon_e6 - ll[1]).abs();

         println!("in ={} {}", lat_e6, lon_e6);
         println!("out={} {}", ll[0], ll[1]);

         if d1 > EPSILON || d2 > EPSILON {
            println!("OOPS");
            exit(-1);
         }

         if d1 > 0 || d2 > 0 {
            imperfect += 1;
         }
      }
   }
   println!("{} imperfect out of {} total", imperfect, total);
}
