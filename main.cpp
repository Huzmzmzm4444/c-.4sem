#include "PointCloud.h"
#include "crop_box_filter.hpp"

#include <iostream>
#include <memory>
#include <ostream>

std::ostream & operator<<(std::ostream & stream, const PointCloud & pc)
{
  stream << "Point cloud (" << pc.pointcloud_type_ << "):\n";
  for (std::size_t i = 0; i < pc.size_; ++i) {
    const std::size_t offset = i * pc.point_size_;
    for (std::size_t j = 0; j < pc.point_size_; ++j) {
      stream << pc.points_[offset + j] << ' ';
    }
    stream << '\n';
  }
  return stream;
}

int main()
{
  try {
    PointCloud pc_xyzir;
    FillPointCloud(
      &pc_xyzir, 3, "XYZIR",
      {5.0, 1.2, 2.1, 0.5, 1.0, -3.2, 0.2, 1.1, 0.7, 1.0, 2.2, 2.1, 7.0, 0.1, 2.0});

    std::cout << "We have:\n" << pc_xyzir << '\n';

    std::unique_ptr<pointcloud_preprocessor::Filter> filter =
      std::make_unique<pointcloud_preprocessor::CropBoxFilter>();
    filter->SetParams(pointcloud_preprocessor::FilterParametr(
      {{"min_x", -4.0},
       {"max_x", -2.0},
       {"min_y", 0.1},
       {"max_y", 2.0},
       {"min_z", 0.5},
       {"max_z", 3.0},
       {"negative", 0.0}}));

    const std::unique_ptr<PointCloud> out(filter->Apply(&pc_xyzir));
    std::cout << "after CBF:\n" << *out << '\n';
  } catch (const std::exception & e) {
    std::cerr << e.what() << '\n';
    return 1;
  }

  return 0;
}
