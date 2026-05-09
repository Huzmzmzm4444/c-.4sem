#include "crop_box_filter.hpp"

#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

namespace pointcloud_preprocessor
{

CropBoxFilter::CropBoxFilter()
: Filter("CropBoxFilter")
{
}

bool CropBoxFilter::IsInside(double x, double y, double z) const noexcept
{
  return z > param_.min_z && z < param_.max_z && y > param_.min_y && y < param_.max_y &&
         x > param_.min_x && x < param_.max_x;
}

PointCloud * CropBoxFilter::Apply(PointCloud * pc)
{
  if (pc == nullptr) {
    throw std::invalid_argument("CropBoxFilter::Apply: null PointCloud pointer");
  }
  if (pc->point_size_ < 3) {
    throw std::invalid_argument("CropBoxFilter::Apply: point size must be at least 3");
  }

  std::vector<double> output;
  output.reserve(pc->points_.size());

  std::size_t output_points_count = 0;
  for (std::size_t i = 0; i < pc->size_; ++i) {
    const std::size_t offset = i * pc->point_size_;
    const double x = pc->points_[offset];
    const double y = pc->points_[offset + 1];
    const double z = pc->points_[offset + 2];

    if (!std::isfinite(x) || !std::isfinite(y) || !std::isfinite(z)) {
      logger_.log("Ignoring point containing non-finite values");
      continue;
    }

    const bool point_is_inside = IsInside(x, y, z);
    const bool should_keep = (!param_.negative && point_is_inside) || (param_.negative && !point_is_inside);
    if (!should_keep) {
      continue;
    }

    output.insert(
      output.end(), pc->points_.begin() + static_cast<std::ptrdiff_t>(offset),
      pc->points_.begin() + static_cast<std::ptrdiff_t>(offset + pc->point_size_));
    ++output_points_count;
  }

  auto * output_pc = new PointCloud;
  output_pc->points_ = std::move(output);
  output_pc->pointcloud_type_ = pc->pointcloud_type_;
  output_pc->size_ = output_points_count;
  output_pc->point_size_ = pc->point_size_;
  return output_pc;
}

void CropBoxFilter::SetParams(const FilterParametr & param)
{
  CropBoxParam new_param = param_;

  new_param.min_x = param.GetParam("min_x", new_param.min_x);
  new_param.max_x = param.GetParam("max_x", new_param.max_x);
  new_param.min_y = param.GetParam("min_y", new_param.min_y);
  new_param.max_y = param.GetParam("max_y", new_param.max_y);
  new_param.min_z = param.GetParam("min_z", new_param.min_z);
  new_param.max_z = param.GetParam("max_z", new_param.max_z);
  new_param.negative = param.GetParam("negative", new_param.negative ? 1.0 : 0.0) != 0.0;

  if (new_param.min_x >= new_param.max_x) {
    throw std::invalid_argument("CropBoxFilter::SetParams: min_x must be less than max_x");
  }
  if (new_param.min_y >= new_param.max_y) {
    throw std::invalid_argument("CropBoxFilter::SetParams: min_y must be less than max_y");
  }
  if (new_param.min_z >= new_param.max_z) {
    throw std::invalid_argument("CropBoxFilter::SetParams: min_z must be less than max_z");
  }

  const bool changed = param_.min_x != new_param.min_x || param_.max_x != new_param.max_x ||
                       param_.min_y != new_param.min_y || param_.max_y != new_param.max_y ||
                       param_.min_z != new_param.min_z || param_.max_z != new_param.max_z ||
                       param_.negative != new_param.negative;

  if (changed) {
    logger_.log(
      "[paramCallback] Setting the minimum point to: " + std::to_string(new_param.min_x) +
      " " + std::to_string(new_param.min_y) + " " + std::to_string(new_param.min_z));
    logger_.log(
      "[paramCallback] Setting the maximum point to: " + std::to_string(new_param.max_x) +
      " " + std::to_string(new_param.max_y) + " " + std::to_string(new_param.max_z));
    logger_.log(
      std::string("[paramCallback] Setting the filter negative flag to: ") +
      (new_param.negative ? "true" : "false"));
    param_ = new_param;
  }

  Filter::SetParams(param);
}

}
