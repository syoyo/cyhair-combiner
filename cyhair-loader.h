/*
The MIT License (MIT)

Copyright (c) 2016 Light Transport Entertainment, Inc.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef CYHAIR_LODER_H_
#define CYHAIR_LODER_H_

#include <vector>

namespace cyhair_loader {

class real3 {
 public:
  real3() : x(0.0f), y(0.0f), z(0.0f) {}
  real3(float v) : x(v), y(v), z(v) {}
  real3(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {}
  //~real3() {}

  real3 operator+(const real3 &f2) const {
    return real3(x + f2.x, y + f2.y, z + f2.z);
  }
  real3 operator*(const real3 &f2) const {
    return real3(x * f2.x, y * f2.y, z * f2.z);
  }
  real3 operator/(const real3 &f2) const {
    return real3(x / f2.x, y / f2.y, z / f2.z);
  }
  real3 operator/(const float f) const { return real3(x / f, y / f, z / f); }

  float x, y, z;
};

inline real3 operator*(float f, const real3 &v) {
  return real3(v.x * f, v.y * f, v.z * f);
}

struct CyHairHeader {
  char magic[4];
  unsigned int num_strands;
  unsigned int total_points;
  unsigned int flags;
  unsigned int default_segments;
  float default_thickness;
  float default_transparency;
  float default_color[3];
  char infomation[88];
};

class CyHair {
 public:
  CyHair()
      : flags_(0),
        num_strands_(0),
        total_points_(0),
        default_segments_(-1),
        default_thickness_(0.01f),
        default_transparency_(1.0f) {
    default_color_[0] = 0.5f;
    default_color_[1] = 0.5f;
    default_color_[2] = 0.5f;
  }

  ~CyHair() {}

  /// Load CyHair data from a file.
  bool Load(const char *filename);

  /// Convert to cubic bezier curves.
  /// 4(cubic) * 3(xyz) * num_curves = vertices.size()
  /// 4(cubic) * num_curves = radiuss.size()
  /// `max_strands` limits the number of strands to convert. -1 = convert all
  /// strands.
  /// `thickness` overwrites strand thickness if it have positive value.
  /// Apply `vertex_translate` after `vertex_scale`.
  /// TODO(syoyo) return strand/segment information
  bool ToCubicBezierCurves(std::vector<float> *vertices,
                           std::vector<float> *radiuss,
                           const float vertex_scale[3],
                           const float vertex_translate[3],
                           const int max_strands = -1,
                           const float thickness = -1.0f);

  CyHairHeader header_;

  // Raw CyHair values
  std::vector<unsigned short> segments_;
  std::vector<float> points_;  // xyz
  std::vector<float> thicknesses_;
  std::vector<float> transparencies_;
  std::vector<float> colors_;  // rgb
  unsigned int flags_;
  unsigned int num_strands_;
  unsigned int total_points_;
  int default_segments_;
  float default_thickness_;
  float default_transparency_;
  float default_color_[3];
  int pad0;

  // Processed CyHair values
  std::vector<unsigned int> strand_offsets_;
};


} // namespace cyhair_loader

#endif // CYHAIR_LODER_H_
