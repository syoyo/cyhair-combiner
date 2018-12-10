/*
MIT License

Copyright (c) 2018 Syoyo Fujita

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <cassert>
#include <cstdint>  // C++11
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>

#include "cyhair-writer.h"

namespace cyhair_writer {

//
// Save curves as CyHair format.
//
bool SaveAsCyhair(const std::string &filename,
                  const size_t num_points,
                  const std::vector<float> &points,
                  const std::vector<uint32_t> &num_points_per_segments,
                  const std::vector<float> &thicknesses,
                  const std::vector<float> &transparencies,
                  const std::vector<float> &colors) {
  CyhairHeader header;

  if (sizeof(header) != 128) {
    std::cerr << "header size mismatch." << std::endl;
    return false;
  }

  header.signagure[0] = 'H';
  header.signagure[1] = 'A';
  header.signagure[2] = 'I';
  header.signagure[3] = 'R';

  // 0x01 = has_segments
  // 0x02 = has_points
  // 0x04 = has_thickness
  // 0x08 = has_transparency
  // 0x10 = has_color
  header.flags = 0;

  if (num_points_per_segments.size() > 0) {
    header.flags |= 0x01;
  }

  if (points.size() > 0) {
    header.flags |= 0x02;
  }

  if (thicknesses.size() > 0) {
    header.flags |= 0x04;
  }

  if (transparencies.size() > 0) {
    header.flags |= 0x08;
  }

  if (colors.size() > 0) {
    header.flags |= 0x10;
  }

  header.d_segments = 0;         // no default segments.
  header.d_thickness = 1.0f;     // FIXME(syoyo)
  header.d_transparency = 0.5f;  // FIXME(syoyo)
  header.d_color[0] = 1.0f;      // FIXME(syoyo)
  header.d_color[1] = 1.0f;      // FIXME(syoyo)
  header.d_color[2] = 1.0f;      // FIXME(syoyo)

  header.hair_count = uint32_t(num_points_per_segments.size());
  header.point_count = uint32_t(num_points);

  std::string info("Generated by cyhair_writer.");
  assert(info.size() < (CYHAIR_INFO_HEADER_SIZE - 1));
  memcpy(header.info, info.data(), info.size());

  std::ofstream ofs(filename.c_str(), std::ios::binary);
  if (!ofs) {
    std::cerr << "Failed to open [" << filename << "] to write." << std::endl;
    return false;
  }

  // shirink to 16bit ushort.
  // strands = points - 1
  std::vector<uint16_t> num_strands;
  for (size_t i = 0; i < num_points_per_segments.size(); i++) {
    assert(num_points_per_segments[i] > 0);
    assert(num_points_per_segments[i] < std::numeric_limits<uint16_t>::max());
    num_strands.push_back(uint16_t(num_points_per_segments[i] - 1));
  }

  // write header.
  ofs.write(reinterpret_cast<const char *>(&header), sizeof(header));

  // write strands.
  if (num_strands.size() > 0) {
    ofs.write(reinterpret_cast<const char *>(num_strands.data()),
              std::streamsize(num_strands.size() * sizeof(uint16_t)));
  }

  // write points.
  if ((points.size() > 0) && (points.size() <= (num_points * 3))) {
    ofs.write(reinterpret_cast<const char *>(points.data()),
              std::streamsize(points.size() * sizeof(float)));
  }

  if ((thicknesses.size() > 0)  && (thicknesses.size() <= num_points)) {
    ofs.write(reinterpret_cast<const char *>(thicknesses.data()),
              std::streamsize(thicknesses.size() * sizeof(float)));
  }

  if ((transparencies.size() > 0)  && (transparencies.size() <= num_points)) {
    ofs.write(reinterpret_cast<const char *>(transparencies.data()),
              std::streamsize(transparencies.size() * sizeof(float)));
  }

  if ((colors.size() > 0)  && (colors.size() <= (num_points * 3))) {
    ofs.write(reinterpret_cast<const char *>(colors.data()),
              std::streamsize(colors.size() * sizeof(float)));
  }

  ofs.close();

  return true;
}

}  // namespace cyhair_writer
