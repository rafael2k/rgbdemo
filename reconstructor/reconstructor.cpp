/**
 * This file is part of the rgbdemo project.
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
 * Author: Nicolas Burrus <nicolas@burrus.name>, (C) 2010, 2011
 */

#include <ntk/ntk.h>
#include <ntk/camera/calibration.h>
#if defined(USE_PMDSDK) && defined(NESTK_PRIVATE)
# include "pmdsdk2.h"
# include <ntk/private/camera/pmd_grabber.h>
# include <ntk/private/camera/pmd_rgb_grabber.h>
#endif

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <sstream>
#include <iomanip>

#include <ntk/image/sift_gpu.h>
#include <ntk/camera/opencv_grabber.h>
#include <ntk/camera/file_grabber.h>
#include <ntk/camera/rgbd_frame_recorder.h>
#include <ntk/camera/kinect_grabber.h>
#include <ntk/mesh/mesh_generator.h>
#include <ntk/mesh/surfels_rgbd_modeler.h>
#include "GuiController.h"
#include "ModelAcquisitionController.h"

#include <QApplication>
#include <QMetaType>

using namespace ntk;
using namespace cv;

namespace opt
{
  ntk::arg<const char*> dir_prefix("--prefix", "Directory prefix for output", "grab1");
  ntk::arg<const char*> calibration_file("--calibration", "Calibration file (yml)", 0);
  ntk::arg<const char*> image("--image", "Fake mode, use given still image", 0);
  ntk::arg<const char*> directory("--directory", "Fake mode, use all view???? images in dir.", 0);
  ntk::arg<int> camera_id("--camera-id", "Camera id for opencv", 0);
  ntk::arg<bool> sync("--sync", "Synchronization mode", 0);
}

int main (int argc, char** argv)
{
  arg_base::set_help_option("-h");
  arg_parse(argc, argv);
  ntk_debug_level = 1;
  cv::setBreakOnError(true);

  QApplication::setGraphicsSystem("raster");
  QApplication app (argc, argv);

  const char* fake_dir = opt::image();
  bool is_directory = opt::directory() != 0;
  if (opt::directory())
    fake_dir = opt::directory();

  ntk::KinectProcessor kinect_processor;
  RGBDGrabber* grabber = 0;

  if (opt::image() || opt::directory())
  {
    std::string path = opt::image() ? opt::image() : opt::directory();
    FileGrabber* file_grabber = new FileGrabber(path, opt::directory() != 0);
    grabber = file_grabber;
  }
  else
  {
    KinectGrabber* k_grabber = new KinectGrabber();
    k_grabber->initialize();
    k_grabber->setIRMode(false);
    grabber = k_grabber;
  }

  if (opt::sync())
    grabber->setSynchronous(true);

  RGBDFrameRecorder frame_recorder (opt::dir_prefix());
  frame_recorder.setSaveOnlyRaw(false);

  ntk::RGBDCalibration calib_data;
  ntk_ensure(opt::calibration_file(), "You must specify a calibration file (--calibration)");
  calib_data.loadFromFile(opt::calibration_file());
  grabber->setCalibrationData(calib_data);

  GuiController gui_controller (*grabber, kinect_processor);
  grabber->addEventListener(&gui_controller);
  gui_controller.setFrameRecorder(frame_recorder);

  if (opt::sync())
    gui_controller.setPaused(true);

  SurfelsRGBDModeler modeler;
  modeler.setMinViewsPerSurfel(1);
  kinect_processor.setFilterFlag(RGBDProcessor::ComputeNormals, 1);
  kinect_processor.setMaxNormalAngle(90);

  ModelAcquisitionController* acq_controller = 0;
  acq_controller = new ModelAcquisitionController (gui_controller, modeler);

  RelativePoseEstimator* pose_estimator = 0;
  FeatureSetParams params ("FAST", "BRIEF64", true);
  pose_estimator = new RelativePoseEstimatorFromImage(params);

  acq_controller->setPoseEstimator(pose_estimator);
  gui_controller.setModelAcquisitionController(*acq_controller);

  grabber->start();

  app.exec();
  delete acq_controller;
}
