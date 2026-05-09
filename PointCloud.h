#ifndef POINT_CLOUD_H_
#define POINT_CLOUD_H_

#include <cstddef>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

struct PointCloud {
  std::vector<double> points_;
  std::size_t size_{0};
  std::size_t point_size_{0};
  std::string pointcloud_type_;

  [[nodiscard]] bool empty() const noexcept
  {
    return size_ == 0 || point_size_ == 0 || points_.empty();
  }
};

inline void FillPointCloud(
  PointCloud * pc, std::size_t size, std::string type, std::vector<double> points)
{
  if (pc == nullptr) {
    throw std::invalid_argument("FillPointCloud: null PointCloud pointer");
  }
  if (size == 0) {
    throw std::invalid_argument("FillPointCloud: point count must be positive");
  }
  if (points.empty() || points.size() % size != 0) {
    throw std::invalid_argument("FillPointCloud: invalid points array size");
  }

  pc->size_ = size;
  pc->point_size_ = points.size() / size;
  pc->pointcloud_type_ = std::move(type);
  pc->points_ = std::move(points);
}

#endif
