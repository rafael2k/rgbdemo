/**
 * Copyright (C) 2013 ManCTL SARL <contact@manctl.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Nicolas Burrus <nicolas.burrus@manctl.com>
 */


#include "calibration_common.h"

#include <ntk/ntk.h>
#include <ntk/camera/calibration.h>
#include <ntk/geometry/pose_3d.h>
#include <ntk/numeric/levenberg_marquart_minimizer.h>
// #include <opencv/cv.h>
#include <fstream>

#include <QDir>
#include <QDebug>

using namespace ntk;
using namespace cv;

// example command line (for copy-n-paste):
// calibrate_one_camera -w 8 -h 6 -o camera.yml images

namespace global
{
ntk::arg<const char*> opt_ref_image_directory(0, "RGBD images directory for reference camera", 0);
ntk::arg<const char*> opt_image_directory(0, "RGBD images directory for camera to be calibrated", 0);
ntk::arg<const char*> opt_ref_calibration(0, "Calibration file for reference camera YAML", "calibration_ref.yml");
ntk::arg<const char*> opt_input_calibration(0, "Input initial calibration file YAML", "calibration.yml");
ntk::arg<const char*> opt_output_file("--output", "Output YAML filename", "calibration_multikinect.yml");
ntk::arg<const char*> opt_pattern_type("--pattern-type", "Pattern type (chessboard, circles, asymcircles)", "chessboard");
ntk::arg<int> opt_pattern_width("--pattern-width", "Pattern width (number of inner corners)", 10);
ntk::arg<int> opt_pattern_height("--pattern-height", "Pattern height (number of inner corners)", 7);
ntk::arg<float> opt_square_size("--pattern-size", "Square size in used defined scale", 0.025);
ntk::arg<bool> use_infrared_images("--infrared", "Use infrared images.", false);

PatternType pattern_type;

RGBDCalibration ref_calibration;
RGBDCalibration calibration;

QDir ref_images_dir;
QStringList ref_images_list;
QStringList images_list;

QDir images_dir;

OpenniRGBDProcessor rgbd_processor;
}

void writeNestkMatrix()
{
#if 0
    // FIXME: temporary !!!
    global::calibration.updatePoses();
    global::calibration.depth_pose->applyTransformBefore(cv::Vec3f(0,0,0), cv::Vec3f(ntk::deg_to_rad(30.f), 0, 0));
    global::calibration.depth_pose->cvRotationMatrixTranslation(global::calibration.T_extrinsics, global::calibration.R_extrinsics);
    global::calibration.updatePoses();
#endif
    global::calibration.saveToFile(global::opt_output_file());
}

int main(int argc, char** argv)
{
    arg_base::set_help_option("--help");
    arg_parse(argc, argv);
    ntk::ntk_debug_level = 1;   

    namedWindow("corners");

    if      (std::string(global::opt_pattern_type()) == "chessboard") global::pattern_type = PatternChessboard;
    else if (std::string(global::opt_pattern_type()) == "circles") global::pattern_type = PatternCircles;
    else if (std::string(global::opt_pattern_type()) == "asymcircles") global::pattern_type = PatternAsymCircles;
    else fatal_error(format("Invalid pattern type: %s\n", global::opt_pattern_type()).c_str());

    global::ref_calibration.loadFromFile(global::opt_ref_calibration());
    global::calibration.loadFromFile(global::opt_input_calibration());

    global::ref_images_dir = QDir(global::opt_ref_image_directory());
    ntk_ensure(global::ref_images_dir.exists(), (global::ref_images_dir.absolutePath() + " is not a directory.").toLatin1());

    global::images_dir = QDir(global::opt_image_directory());
    ntk_ensure(global::images_dir.exists(), (global::images_dir.absolutePath() + " is not a directory.").toLatin1());

    global::ref_images_list = global::ref_images_dir.entryList(QStringList("view????*"), QDir::Dirs, QDir::Name);
    global::images_list = global::images_dir.entryList(QStringList("view????*"), QDir::Dirs, QDir::Name);

    std::vector<RGBDImage> ref_images;
    loadImageList(global::ref_images_dir,
                   global::ref_images_list,
                   &global::rgbd_processor,
                   &global::ref_calibration,
                   ref_images);

    std::vector<RGBDImage> images;
    loadImageList(global::images_dir,
                   global::images_list,
                   &global::rgbd_processor,
                   &global::calibration,
                   images);

    std::vector< std::vector<Point2f> > ref_corners, ref_good_corners;
    getCalibratedCheckerboardCorners(ref_images,
                                     global::opt_pattern_width(), global::opt_pattern_height(), global::pattern_type,
                                     ref_corners,
                                     ref_good_corners,
                                     true,
                                     global::use_infrared_images());

    std::vector< std::vector<Point2f> > corners, good_corners;
    getCalibratedCheckerboardCorners(images,
                                     global::opt_pattern_width(), global::opt_pattern_height(), global::pattern_type,
                                     corners,
                                     good_corners,
                                     true,
                                     global::use_infrared_images());

    calibrateStereoFromCheckerboard(ref_corners, corners,
                                    global::opt_pattern_width(), global::opt_pattern_height(), global::opt_square_size(),
                                    global::calibration,
                                    global::use_infrared_images());

    writeNestkMatrix();
    return 0;
}
