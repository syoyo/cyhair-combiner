#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#endif

#include "nlohmann/json.hpp"

#ifdef __clang__
#pragma clang diagnostic pop
#endif


// Simple Cyhair loader.

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include <iostream>
#include <fstream>

#include "cyhair-loader.h"
#include "cyhair-writer.h"

using nlohmann::json;

struct CyhairInput {
  std::string filename;
  float user_thickness = -1.0f; // negative = use thickness in .hair
  float thickness_scale = 1.0f; // thickness scale applied after reading .hair
};

static bool CombineCyhair(
  const std::vector<CyhairInput> &inputs,
  std::vector<float> *points,
  std::vector<uint32_t> *num_points_per_segments,
  std::vector<float> *thicknesses)
{
  points->clear();
  num_points_per_segments->clear();
  thicknesses->clear();
 
  for (size_t i = 0; i < inputs.size(); i++) {
    // TODO(syoyo): Check if all cyhair file has points, segments and thickness attributes.
    cyhair_loader::CyHair hair;
    if (!hair.Load(inputs[i].filename.c_str())) {
      std::cerr << "failed to read a cyhair file : " << inputs[i].filename << std::endl;
      return false;
    }

    std::cout << "hair[" << i << "] points = " << hair.points_.size() << ", thicknesses = " << hair.thicknesses_.size() << ", segments = " << hair.segments_.size() << std::endl;

    const size_t num_points = hair.points_.size() / 3;

    for (size_t k = 0; k < hair.points_.size(); k++) {
      points->push_back(hair.points_[k]);
    }

    // points = segments + 1 
    for (size_t k = 0; k < hair.segments_.size(); k++) {
      num_points_per_segments->push_back(hair.segments_[k] + 1);
    }

    if (inputs[i].user_thickness >= 0.0f) {
      for (size_t k = 0; k < num_points; k++) {
        thicknesses->push_back(inputs[i].user_thickness * inputs[i].thickness_scale);
      }
    } else {
      assert(num_points == hair.thicknesses_.size());

      for (size_t k = 0; k < hair.thicknesses_.size(); k++) {
        thicknesses->push_back(hair.thicknesses_[k] * inputs[i].thickness_scale);
      }
    }
  }

  return true;
}

int main(int argc, char **argv)
{
  if (argc < 3) {
    std::cerr << "Needs input.json output.hair" << std::endl;
    return EXIT_FAILURE;
  }

  std::ifstream ifs(argv[1]);
  if (!ifs) {
    std::cerr << "Failed to open file: " << argv[1] << std::endl;
    return EXIT_FAILURE;
  }


  json j;

  ifs >> j;

  std::vector<CyhairInput> cyhair_inputs;

  for (json::iterator it = j.begin(); it != j.end(); ++it) {
    json o = it.value();

    CyhairInput in;

    if (o.count("filename") == 0) {
      std::cerr << "Invalid JSON. `filename` is required." << std::endl;
      return EXIT_FAILURE;
    } 

    in.filename = o["filename"];

    if (o.count("user_thickness")) {
      in.user_thickness = float(o["user_thickness"]);
    }

    if (o.count("thickness_scale")) {
      in.thickness_scale = float(o["thickness_scale"]);
    }

    std::cout << "filename : " << in.filename << ", user_thickness = " << in.user_thickness << ", thickness_scale = " << in.thickness_scale << std::endl;

    cyhair_inputs.emplace_back(std::move(in));
  }

  std::vector<float> points;
  std::vector<uint32_t> num_points_per_segments;
  std::vector<float> thickness;

  // FIXME(syoyo): Read overwite thickness parameter from args
  // TODO(syoyo): Support transparency and colors.
  if (!CombineCyhair(cyhair_inputs,  &points, &num_points_per_segments, &thickness)) {
    std::cerr << "Failed to combine cyhair" << std::endl;
  }

  std::string output_filename = argv[2];

  const size_t num_points = points.size() / 3;

  // empty attribs
  std::vector<float> transparencies;
  std::vector<float> colors;

  if (!cyhair_writer::SaveAsCyhair(output_filename, num_points, points, num_points_per_segments, thickness, transparencies, colors)) {

    std::cerr << "Failed to save CyHair" << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "Combined Cyhair." << std::endl;
  
  return EXIT_SUCCESS;
}
