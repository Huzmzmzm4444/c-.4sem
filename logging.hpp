#ifndef POINTCLOUD_PREPROCESSOR__LOGGING_HPP_
#define POINTCLOUD_PREPROCESSOR__LOGGING_HPP_

#include "Logger.h"

#include <memory>
#include <string>

namespace pointcloud_preprocessor
{

class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const std::string & msg) = 0;
};

class FilterLogger : public ILogger {
public:
    explicit FilterLogger(const std::string & name)
    : logger_(name)
    {
    }

    void log(const std::string & msg) override
    {
        logger_.log(msg);
    }

private:
    Logger logger_;
};

using LoggerPtr = std::shared_ptr<ILogger>;

}

#endif
