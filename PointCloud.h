#ifndef POINTCLOUD_H_
#define POINTCLOUD_H_

#include <cmath>
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

struct PointCloud {
public:
    std::string pointcloud_type_;
    std::size_t size_{0};
    std::size_t point_size_{0};
    std::vector<double> points_;
};

class PointCloudError : public std::runtime_error {
public:
    explicit PointCloudError(const std::string & msg)
    : std::runtime_error(msg)
    {
    }
};

class PointCloudValidationError : public PointCloudError {
public:
    explicit PointCloudValidationError(const std::string & msg)
    : PointCloudError(msg)
    {
    }
};

inline void ValidatePointCloud(const PointCloud & pc)
{
    if (pc.point_size_ == 0 && pc.size_ != 0) {
        throw PointCloudValidationError("PointCloud point_size_ must be positive for non-empty cloud");
    }

    if (pc.points_.size() != pc.size_ * pc.point_size_) {
        throw PointCloudValidationError("PointCloud storage size does not match point count");
    }
}

inline void FillPointCloud(
    PointCloud & pc,
    std::size_t point_size,
    std::string pointcloud_type,
    std::initializer_list<double> points)
{
    if (point_size == 0) {
        throw PointCloudValidationError("Point size must be positive");
    }

    if (points.size() % point_size != 0) {
        throw PointCloudValidationError("Point buffer size must be divisible by point size");
    }

    pc.point_size_ = point_size;
    pc.pointcloud_type_ = std::move(pointcloud_type);
    pc.points_.assign(points.begin(), points.end());
    pc.size_ = pc.points_.size() / pc.point_size_;
    ValidatePointCloud(pc);
}

#endif
