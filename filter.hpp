#ifndef POINTCLOUD_PREPROCESSOR__FILTER_HPP_
#define POINTCLOUD_PREPROCESSOR__FILTER_HPP_

#include "PointCloud.h"
#include "logging.hpp"

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

namespace pointcloud_preprocessor
{

class FilterError : public std::runtime_error {
public:
    explicit FilterError(const std::string & msg)
    : std::runtime_error(msg)
    {
    }
};

class FilterConfigurationError : public FilterError {
public:
    explicit FilterConfigurationError(const std::string & msg)
    : FilterError(msg)
    {
    }
};

class FilterExecutionError : public FilterError {
public:
    explicit FilterExecutionError(const std::string & msg)
    : FilterError(msg)
    {
    }
};

class Filter {
public:
    Filter(const std::string & filter_name, const LoggerPtr & logger)
    : filter_name_(filter_name), logger_(logger)
    {
        if (!logger_) {
            throw FilterConfigurationError("Filter requires logger");
        }
    }

    virtual ~Filter() = default;

    const std::string & GetFilterName() const
    {
        return filter_name_;
    }

    virtual std::unique_ptr<PointCloud> Apply(const PointCloud & pc) const = 0;

protected:
    void ValidateInputCloud(const PointCloud & pc) const
    {
        ValidatePointCloud(pc);
    }

    std::string filter_name_;
    LoggerPtr logger_;
};

}

#endif
