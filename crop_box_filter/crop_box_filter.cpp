#include "crop_box_filter.hpp"

#include <sstream>
#include <utility>

namespace pointcloud_preprocessor
{

CropBoxFilter::CropBoxFilter(const LoggerPtr & logger, const PointCloudAccessorPtr & point_accessor)
: Filter("CropBoxFilter", logger), point_accessor_(point_accessor)
{
    if (!point_accessor_) {
        throw FilterConfigurationError("CropBoxFilter requires point accessor");
    }
}

CropBoxFilter::CropBoxFilter(
    const LoggerPtr & logger,
    const PointCloudAccessorPtr & point_accessor,
    const CropBoxParam & param)
: CropBoxFilter(logger, point_accessor)
{
    SetParams(param);
}

void CropBoxFilter::ValidateParams(const CropBoxParam & param) const
{
    const bool finite =
        std::isfinite(param.min_x) && std::isfinite(param.max_x) &&
        std::isfinite(param.min_y) && std::isfinite(param.max_y) &&
        std::isfinite(param.min_z) && std::isfinite(param.max_z);

    if (!finite) {
        throw FilterConfigurationError("CropBoxFilter params must be finite numbers");
    }

    if (param.min_x > param.max_x ||
        param.min_y > param.max_y ||
        param.min_z > param.max_z)
    {
        throw FilterConfigurationError("CropBoxFilter min bounds must be less than or equal to max bounds");
    }
}

void CropBoxFilter::ValidateInputCloud(const PointCloud & pc) const
{
    Filter::ValidateInputCloud(pc);

    if (!point_accessor_->Supports(pc)) {
        throw FilterExecutionError("PointCloud format is not supported by CropBoxFilter");
    }
}

bool CropBoxFilter::IsPointInside(const PointXYZ & point) const
{
    return point.z > param_.min_z && point.z < param_.max_z &&
           point.y > param_.min_y && point.y < param_.max_y &&
           point.x > param_.min_x && point.x < param_.max_x;
}

std::unique_ptr<PointCloud> CropBoxFilter::Apply(const PointCloud & pc) const
{
    if (!configured_) {
        throw FilterExecutionError("CropBoxFilter is not configured");
    }

    try {
        ValidateInputCloud(pc);

        auto output_pc = std::make_unique<PointCloud>();
        output_pc->pointcloud_type_ = pc.pointcloud_type_;
        output_pc->point_size_ = pc.point_size_;
        output_pc->points_.reserve(pc.points_.size());

        for (std::size_t i = 0; i < pc.size_; ++i) {
            PointXYZ point = point_accessor_->GetPoint(pc, i);

            if (!IsFinitePoint(point)) {
                logger_->log("Ignoring point containing NaN values");
                continue;
            }

            const bool point_is_inside = IsPointInside(point);
            if ((!param_.negative && point_is_inside) || (param_.negative && !point_is_inside)) {
                const std::size_t global_offset = i * pc.point_size_;
                output_pc->points_.insert(
                    output_pc->points_.end(),
                    pc.points_.begin() + static_cast<std::ptrdiff_t>(global_offset),
                    pc.points_.begin() + static_cast<std::ptrdiff_t>(global_offset + pc.point_size_));
            }
        }

        output_pc->size_ = output_pc->point_size_ == 0 ? 0 : output_pc->points_.size() / output_pc->point_size_;
        ValidatePointCloud(*output_pc);
        return output_pc;
    } catch (const PointCloudError & error) {
        throw FilterExecutionError(std::string("CropBoxFilter PointCloud error: ") + error.what());
    } catch (const PointAccessorError & error) {
        throw FilterExecutionError(std::string("CropBoxFilter point access error: ") + error.what());
    }
}

void CropBoxFilter::SetParams(const CropBoxParam & param)
{
    ValidateParams(param);

    if (
        param_.min_x != param.min_x || param_.max_x != param.max_x ||
        param_.min_y != param.min_y || param_.max_y != param.max_y ||
        param_.min_z != param.min_z || param_.max_z != param.max_z ||
        param_.negative != param.negative)
    {
        std::ostringstream min_stream;
        min_stream << "[paramCallback] Setting the minimum point to: "
                   << param.min_x << " " << param.min_y << " " << param.min_z;
        logger_->log(min_stream.str());

        std::ostringstream max_stream;
        max_stream << "[paramCallback] Setting the maximum point to: "
                   << param.max_x << " " << param.max_y << " " << param.max_z;
        logger_->log(max_stream.str());

        logger_->log(std::string("[paramCallback] Setting the filter negative flag to: ") +
            (param.negative ? "true" : "false"));

        param_ = param;
        configured_ = true;
    }
}

const CropBoxParam & CropBoxFilter::GetParams() const
{
    return param_;
}

bool CropBoxFilter::IsConfigured() const
{
    return configured_;
}

}  // namespace pointcloud_preprocessor
