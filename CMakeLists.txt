cmake_minimum_required(VERSION 3.10)
project(OPENCV_PROJECT)

# Use C++17
set(CMAKE_CXX_STANDARD 17)

# Find OpenCV
find_package(OpenCV REQUIRED)

# Add executable
add_executable(yolov8 yolov8.cpp)

# Link OpenCV libraries
target_link_libraries(yolov8 ${OpenCV_LIBS})

# Path to model file
set(MODEL_FILE ${CMAKE_SOURCE_DIR}/yolov8n.onnx)

# Copy yolov8n.onnx to build directory after build
add_custom_command(TARGET yolov8 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${MODEL_FILE}
    $<TARGET_FILE_DIR:yolov8>)
