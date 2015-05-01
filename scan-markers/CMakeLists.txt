FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5Gui REQUIRED)
FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5OpenGL REQUIRED)
FIND_PACKAGE(Qt5Network REQUIRED)
FIND_PACKAGE(Qt5Concurrent REQUIRED)
INCLUDE_DIRECTORIES(${Qt5Core_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Gui_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Widgets_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5OpenGL_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Network_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Concurrent_INCLUDE_DIRS})
IF (NOT Qt5Core_FOUND OR NOT Qt5Gui_FOUND OR NOT Qt5Widgets_FOUND OR NOT Qt5OpenGL_FOUND OR NOT Qt5Network_FOUND OR NOT Qt5Concurrent_FOUND)
  MESSAGE(FATAL_ERROR "Not all modules of Qt5 were found.")
ENDIF()

QT5_WRAP_CPP(moc_sources
  GuiController.h
  ModelAcquisitionController.h
  FiltersWindow.h
  RawImagesWindow.h
  ModelAcquisitionWindow.h
)

QT5_WRAP_UI(uic_sources
  RawImagesWindow.ui
  FiltersWindow.ui
  ModelAcquisitionWindow.ui
)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
SET(scan_markers_sources ${moc_sources} ${uic_sources}
               scan_markers.cpp
               RawImagesWindow.h RawImagesWindow.cpp
               GuiController.cpp GuiController.h
               ModelAcquisitionController.cpp ModelAcquisitionController.h
               FiltersWindow.cpp FiltersWindow.h
               ModelAcquisitionWindow.cpp ModelAcquisitionWindow.h)

NESTK_ADD_EXECUTABLE(rgbd-scan-markers ${scan_markers_sources})
TARGET_LINK_LIBRARIES(rgbd-scan-markers
                      nestk
                      ${Qt5Core_LIBRARIES}
                      ${Qt5Gui_LIBRARIES}
                      ${Qt5Widgets_LIBRARIES}
                      ${Qt5OpenGL_LIBRARIES}
                      ${Qt5Network_LIBRARIES}
                      ${Qt5Concurrent_LIBRARIES})
