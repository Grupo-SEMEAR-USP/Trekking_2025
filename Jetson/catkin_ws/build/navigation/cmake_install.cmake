# Install script for directory: /home/gola/Documents/SEMEAR/Trekking_2025/Jetson/catkin_ws/src/navigation

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/gola/Documents/SEMEAR/Trekking_2025/Jetson/catkin_ws/install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/navigation/msg" TYPE FILE FILES "/home/gola/Documents/SEMEAR/Trekking_2025/Jetson/catkin_ws/src/navigation/msg/encoders.msg")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/navigation/cmake" TYPE FILE FILES "/home/gola/Documents/SEMEAR/Trekking_2025/Jetson/catkin_ws/build/navigation/catkin_generated/installspace/navigation-msg-paths.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE DIRECTORY FILES "/home/gola/Documents/SEMEAR/Trekking_2025/Jetson/catkin_ws/devel/include/navigation")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/roseus/ros" TYPE DIRECTORY FILES "/home/gola/Documents/SEMEAR/Trekking_2025/Jetson/catkin_ws/devel/share/roseus/ros/navigation")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/common-lisp/ros" TYPE DIRECTORY FILES "/home/gola/Documents/SEMEAR/Trekking_2025/Jetson/catkin_ws/devel/share/common-lisp/ros/navigation")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/gennodejs/ros" TYPE DIRECTORY FILES "/home/gola/Documents/SEMEAR/Trekking_2025/Jetson/catkin_ws/devel/share/gennodejs/ros/navigation")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  execute_process(COMMAND "/usr/bin/python3" -m compileall "/home/gola/Documents/SEMEAR/Trekking_2025/Jetson/catkin_ws/devel/lib/python3/dist-packages/navigation")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages" TYPE DIRECTORY FILES "/home/gola/Documents/SEMEAR/Trekking_2025/Jetson/catkin_ws/devel/lib/python3/dist-packages/navigation")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/home/gola/Documents/SEMEAR/Trekking_2025/Jetson/catkin_ws/build/navigation/catkin_generated/installspace/navigation.pc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/navigation/cmake" TYPE FILE FILES "/home/gola/Documents/SEMEAR/Trekking_2025/Jetson/catkin_ws/build/navigation/catkin_generated/installspace/navigation-msg-extras.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/navigation/cmake" TYPE FILE FILES
    "/home/gola/Documents/SEMEAR/Trekking_2025/Jetson/catkin_ws/build/navigation/catkin_generated/installspace/navigationConfig.cmake"
    "/home/gola/Documents/SEMEAR/Trekking_2025/Jetson/catkin_ws/build/navigation/catkin_generated/installspace/navigationConfig-version.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/navigation" TYPE FILE FILES "/home/gola/Documents/SEMEAR/Trekking_2025/Jetson/catkin_ws/src/navigation/package.xml")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/navigation" TYPE PROGRAM FILES "/home/gola/Documents/SEMEAR/Trekking_2025/Jetson/catkin_ws/build/navigation/catkin_generated/installspace/odom_pub_simul.py")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/navigation" TYPE PROGRAM FILES "/home/gola/Documents/SEMEAR/Trekking_2025/Jetson/catkin_ws/build/navigation/catkin_generated/installspace/cmd_vel_to_ackermann_drive.py")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/navigation" TYPE PROGRAM FILES "/home/gola/Documents/SEMEAR/Trekking_2025/Jetson/catkin_ws/build/navigation/catkin_generated/installspace/cmd_vel_to_ackermann_drive_rod.py")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/navigation" TYPE PROGRAM FILES "/home/gola/Documents/SEMEAR/Trekking_2025/Jetson/catkin_ws/build/navigation/catkin_generated/installspace/tf_publisher.py")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/navigation" TYPE PROGRAM FILES "/home/gola/Documents/SEMEAR/Trekking_2025/Jetson/catkin_ws/build/navigation/catkin_generated/installspace/tf_publisher_simul.py")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/navigation" TYPE PROGRAM FILES "/home/gola/Documents/SEMEAR/Trekking_2025/Jetson/catkin_ws/build/navigation/catkin_generated/installspace/path_saver.py")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/navigation" TYPE PROGRAM FILES "/home/gola/Documents/SEMEAR/Trekking_2025/Jetson/catkin_ws/build/navigation/catkin_generated/installspace/path_reader.py")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/navigation" TYPE PROGRAM FILES "/home/gola/Documents/SEMEAR/Trekking_2025/Jetson/catkin_ws/build/navigation/catkin_generated/installspace/path_follower.py")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/navigation" TYPE PROGRAM FILES "/home/gola/Documents/SEMEAR/Trekking_2025/Jetson/catkin_ws/build/navigation/catkin_generated/installspace/odom_front.py")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/navigation" TYPE PROGRAM FILES "/home/gola/Documents/SEMEAR/Trekking_2025/Jetson/catkin_ws/build/navigation/catkin_generated/installspace/odom_esp_test.py")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/navigation" TYPE PROGRAM FILES "/home/gola/Documents/SEMEAR/Trekking_2025/Jetson/catkin_ws/build/navigation/catkin_generated/installspace/start_navi.py")
endif()

