#include "PointCloud.h"
#include "crop_box_filter/crop_box_filter.hpp"
#include "logging.hpp"
#include "point_accessor.hpp"

#include <iostream>
#include <memory>
#include <ostream>
#include <string>

std::ostream & operator<<(std::ostream & os, const PointCloud & pc)
{
    os << "Point cloud (" << pc.pointcloud_type_ << "):" << std::endl;
    for (std::size_t i = 0; i < pc.size_; ++i) {
        for (std::size_t j = 0; j < pc.point_size_; ++j) {
            os << pc.points_[i * pc.point_size_ + j] << " ";
        }
        os << std::endl;
    }
    return os;
}

int main()
{
    try {
        PointCloud pc_xyzir;
        FillPointCloud(
            pc_xyzir,
            5,
            "XYZIR",
            {
                5.0, 1.2, 2.1, 0.5, 1.0,
                -3.2, 0.2, 1.1, 0.7, 1.0,
                2.2, 2.1, 7.0, 0.1, 2.0,
            });

        std::cout << "We have:" << std::endl;
        std::cout << pc_xyzir << std::endl;

        auto logger = std::make_shared<pointcloud_preprocessor::FilterLogger>("CropBoxFilter");
        auto accessor = std::make_shared<pointcloud_preprocessor::XYZPointCloudAccessor>();

        pointcloud_preprocessor::CropBoxParam param;
        param.min_x = -4.0;
        param.max_x = -2.0;
        param.min_y = 0.1;
        param.max_y = 2.0;
        param.min_z = 0.5;
        param.max_z = 3.0;
        param.negative = false;

        std::unique_ptr<pointcloud_preprocessor::Filter> cbf =
            std::make_unique<pointcloud_preprocessor::CropBoxFilter>(logger, accessor, param);

        std::unique_ptr<PointCloud> out = cbf->Apply(pc_xyzir);
        std::cout << "after CBF:" << std::endl;
        std::cout << *out << std::endl;
        return 0;
    } catch (const pointcloud_preprocessor::FilterConfigurationError & error) {
        std::cerr << "Configuration error: " << error.what() << std::endl;
        return 2;
    } catch (const pointcloud_preprocessor::FilterExecutionError & error) {
        std::cerr << "Execution error: " << error.what() << std::endl;
        return 3;
    } catch (const PointCloudError & error) {
        std::cerr << "PointCloud error: " << error.what() << std::endl;
        return 4;
    } catch (const std::exception & error) {
        std::cerr << "Unhandled error: " << error.what() << std::endl;
        return 1;
    }
}
