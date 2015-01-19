cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

project(H264LiveStreamer)

#Find and include OpenCV
find_package(OpenCV REQUIRED)
message(STATUS ${OpenCV_INCLUDE_DIRS})

#Include FindPaths script which searches for the include and link directories
#for the required libraries
include(${CMAKE_SOURCE_DIR}/FindPaths.cmake)

#Set the include directoies for the required libraries
include_directories(
  ${OpenCV_INCLUDE_DIRS}
  ${liveMedia_INCLUDE_DIR}
  ${groupsock_INCLUDE_DIR}
  ${UsageEnvironment_INCLUDE_DIR}
  ${BasicUsageEnvironment_INCLUDE_DIR}
  ${x264_INCLUDE_DIR}
  ${tbb_INCLUDE_DIR}
  ${swscale_INCLUDE_DIR}
  )

#Set the link directoreis for the required libraries
link_directories(
  ${LINK_DIRECTORIES}
  ${liveMedia_LIBRARY_DIR}
  ${groupsock_LIBRARY_DIR}
  ${UsageEnvironment_LIBRARY_DIR}
  ${BasicUsageEnvironment_LIBRARY_DIR}
  ${x264_LIBRARY_DIR}
  ${tbb_LIBRARY_DIR}
  ${swscale_LIBRARY_DIR}
  )

#Create executable
add_executable(H264StreamerExample
  x264Encoder.cxx
  H264LiveServerMediaSession.cxx
  LiveSourceWithx264.cxx
  main.cxx
  )

#Link libraries
target_link_libraries(H264StreamerExample
  swscale
  x264
  liveMedia
  groupsock
  UsageEnvironment
  BasicUsageEnvironment
  tbb
  ${OpenCV_LIBRARIES}
  )