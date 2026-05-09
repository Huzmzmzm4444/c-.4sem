#ifndef POINTCLOUD_PREPROCESSOR__FILTER_HPP_
#define POINTCLOUD_PREPROCESSOR__FILTER_HPP_

#include "Logger.h"
#include "PointCloud.h"

#include <cmath>
#include <string>
#include <unordered_map>
#include <utility>

namespace pointcloud_preprocessor
{

class FilterParametr
{
public:
  FilterParametr() = default;

  explicit FilterParametr(std::unordered_map<std::string, double> params)
  : params_(std::move(params))
  {
  }

  void ChangeParam(const std::string & param_name, double param_val)
  {
    params_[param_name] = param_val;
  }

  [[nodiscard]] bool HasParam(const std::string & param_name) const noexcept
  {
    return params_.find(param_name) != params_.end();
  }

  [[nodiscard]] double GetParam(const std::string & param_name) const
  {
    return params_.at(param_name);
  }

  [[nodiscard]] double GetParam(const std::string & param_name, double default_val) const noexcept
  {
    const auto it = params_.find(param_name);
    return it == params_.end() ? default_val : it->second;
  }

private:
  std::unordered_map<std::string, double> params_;
};

class Filter
{
public:
  explicit Filter(const std::string & filter_name = "pointcloud_preprocessor_filter")
  : filter_name_(filter_name), logger_(filter_name_)
  {
  }

  virtual ~Filter() = default;

  [[nodiscard]] const std::string & GetFilterName() const noexcept
  {
    return filter_name_;
  }

  [[nodiscard]] double GetDistance(const PointCloud * pc, std::size_t index) const
  {
    if (pc->pointcloud_type_ == "XYZIR") {
      const auto base = index * pc->point_size_;
      return std::hypot(pc->points_[base], pc->points_[base + 1], pc->points_[base + 2]);
    }

    return pc->points_[index * pc->point_size_ + 5];
  }

  [[nodiscard]] double GetAzimuth(const PointCloud * pc, std::size_t index) const
  {
    if (pc->pointcloud_type_ == "XYZIR") {
      const auto base = index * pc->point_size_;
      return std::atan2(pc->points_[base + 1], pc->points_[base]);
    }

    return pc->points_[index * pc->point_size_ + 6];
  }

  virtual PointCloud * Apply(PointCloud * pc) = 0;

  virtual void SetParams(const FilterParametr & param)
  {
    params_ = param;
  }

  [[nodiscard]] const FilterParametr & GetFilterParam() const noexcept
  {
    return params_;
  }

protected:
  std::string filter_name_{"pointcloud_preprocessor_filter"};
  Logger logger_;
  FilterParametr params_;
};

} 

#endif
