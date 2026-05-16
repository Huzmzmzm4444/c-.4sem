#ifndef POINTCLOUD_PREPROCESSOR__CROP_BOX_FILTER_HPP_
#define POINTCLOUD_PREPROCESSOR__CROP_BOX_FILTER_HPP_

#include "../filter.hpp"
#include "../point_accessor.hpp"

namespace pointcloud_preprocessor
{

struct CropBoxParam {
    double min_x{0.0};
    double max_x{0.0};
    double min_y{0.0};
    double max_y{0.0};
    double min_z{0.0};
    double max_z{0.0};
    bool negative{false};
};

class CropBoxFilter : public Filter {
public:
    CropBoxFilter(const LoggerPtr & logger, const PointCloudAccessorPtr & point_accessor);
    CropBoxFilter(
        const LoggerPtr & logger,
        const PointCloudAccessorPtr & point_accessor,
        const CropBoxParam & param);

    std::unique_ptr<PointCloud> Apply(const PointCloud & pc) const override;

    void SetParams(const CropBoxParam & param);
    const CropBoxParam & GetParams() const;
    bool IsConfigured() const;

private:
    void ValidateParams(const CropBoxParam & param) const;
    void ValidateInputCloud(const PointCloud & pc) const;
    bool IsPointInside(const PointXYZ & point) const;

    PointCloudAccessorPtr point_accessor_;
    CropBoxParam param_{};
    bool configured_{false};
};

}

#endif
