#ifndef POINTCLOUD_PREPROCESSOR__POINT_ACCESSOR_HPP_
#define POINTCLOUD_PREPROCESSOR__POINT_ACCESSOR_HPP_

#include "PointCloud.h"

#include <cmath>
#include <memory>
#include <stdexcept>
#include <string>

namespace pointcloud_preprocessor
{

struct PointXYZ {
    double x{0.0};
    double y{0.0};
    double z{0.0};
};

class PointAccessorError : public std::runtime_error {
public:
    explicit PointAccessorError(const std::string & msg)
    : std::runtime_error(msg)
    {
    }
};

class PointCloudAccessor {
public:
    virtual ~PointCloudAccessor() = default;
    virtual bool Supports(const PointCloud & pc) const = 0;
    virtual PointXYZ GetPoint(const PointCloud & pc, std::size_t index) const = 0;
};

class XYZPointCloudAccessor : public PointCloudAccessor {
public:
    bool Supports(const PointCloud & pc) const override
    {
        return pc.point_size_ >= 3;
    }

    PointXYZ GetPoint(const PointCloud & pc, std::size_t index) const override
    {
        ValidatePointCloud(pc);

        if (!Supports(pc)) {
            throw PointAccessorError("PointCloud must contain at least XYZ fields");
        }

        if (index >= pc.size_) {
            throw PointAccessorError("Point index is out of range");
        }

        const std::size_t offset = index * pc.point_size_;
        PointXYZ point;
        point.x = pc.points_[offset + 0];
        point.y = pc.points_[offset + 1];
        point.z = pc.points_[offset + 2];
        return point;
    }
};

inline bool IsFinitePoint(const PointXYZ & point)
{
    return std::isfinite(point.x) && std::isfinite(point.y) && std::isfinite(point.z);
}

using PointCloudAccessorPtr = std::shared_ptr<PointCloudAccessor>;

}

#endif
