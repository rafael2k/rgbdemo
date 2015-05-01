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
  RawImagesWindow.h
)

QT5_WRAP_UI(uic_sources
  RawImagesWindow.ui
)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

SET(rgbd_skeletor_sources ${moc_sources} ${uic_sources}
               skeletor.cpp
               RawImagesWindow.h RawImagesWindow.cpp
               GuiController.cpp GuiController.h)

NESTK_ADD_EXECUTABLE(rgbd-skeletor ${rgbd_skeletor_sources})
TARGET_LINK_LIBRARIES(rgbd-skeletor
                      nestk
                      ${Qt5Core_LIBRARIES}
                      ${Qt5Gui_LIBRARIES}
                      ${Qt5Widgets_LIBRARIES}
                      ${Qt5OpenGL_LIBRARIES}
                      ${Qt5Network_LIBRARIES}
                      ${Qt5Concurrent_LIBRARIES})
