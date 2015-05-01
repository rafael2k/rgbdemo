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
  GuiMultiKinectController.h
  View3dWindow.h
  FiltersWindow.h
  RawImagesWindow.h
)

QT5_WRAP_UI(uic_sources
  RawImagesWindow.ui
  View3dWindow.ui
  FiltersWindow.ui
)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

SET(rgbd_multikinect_sources ${moc_sources} ${uic_sources}
               main.cpp
               GuiController.cpp GuiController.h
               View3dWindow.cpp View3dWindow.h
               FiltersWindow.cpp FiltersWindow.h
               RawImagesWindow.cpp RawImagesWindow.h
               MultiKinectScanner.cpp MultiKinectScanner.h
               GuiMultiKinectController.cpp GuiMultiKinectController.h
               ScannerBlock.cpp ScannerBlock.h
)

NESTK_ADD_EXECUTABLE(rgbd-multikinect ${rgbd_multikinect_sources})
TARGET_LINK_LIBRARIES(rgbd-multikinect
                      nestk
                      ${Qt5Core_LIBRARIES}
                      ${Qt5Gui_LIBRARIES}
                      ${Qt5Widgets_LIBRARIES}
                      ${Qt5OpenGL_LIBRARIES}
                      ${Qt5Network_LIBRARIES}
                      ${Qt5Concurrent_LIBRARIES})
