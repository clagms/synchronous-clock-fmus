cmake_minimum_required (VERSION 3.17)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project (Synchronous-Clock-FMUs)

set(FMI_ARCHITECTURE "" CACHE STRING "FMI Architecture")
set_property(CACHE FMI_ARCHITECTURE PROPERTY STRINGS "" "aarch64" "x86" "x86_64")

if (NOT FMI_ARCHITECTURE)
  if (${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "AMD64|x86_64")
    set(FMI_ARCHITECTURE "x86_64")
  elseif (${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "aarch64")
    set(FMI_ARCHITECTURE "aarch64")
  else ()
    message(FATAL_ERROR "Unknown System Architecture: ${CMAKE_SYSTEM_PROCESSOR}")
  endif ()
endif ()

if (WIN32)
  set(FMI_PLATFORM "${FMI_ARCHITECTURE}-windows")
elseif (APPLE)
  set(FMI_PLATFORM "${FMI_ARCHITECTURE}-darwin")
else ()
  set(FMI_PLATFORM "${FMI_ARCHITECTURE}-linux")
endif ()

message("FMI_PLATFORM: " ${FMI_PLATFORM})

set (MODEL_NAMES Controller Plant Supervisor)

foreach (MODEL_NAME ${MODEL_NAMES})

set(MODEL_NAME ${MODEL_NAME})
set(TARGET_NAME ${MODEL_NAME})

set(FMU_BUILD_DIR temp/${MODEL_NAME})

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/fmus)

add_library(${MODEL_NAME} SHARED
  include/fmi3Functions.h
  include/fmi3FunctionTypes.h
  include/fmi3PlatformTypes.h
  src/fmi3Functions.c
  ${MODEL_NAME}/${MODEL_NAME}.c
  ${MODEL_NAME}/modelDescription.xml
)

target_include_directories(${TARGET_NAME} PRIVATE
  include
)

set_target_properties(${TARGET_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY         "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
  LIBRARY_OUTPUT_DIRECTORY         "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
  LIBRARY_OUTPUT_DIRECTORY_DEBUG   "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
  LIBRARY_OUTPUT_DIRECTORY_RELEASE "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
  ARCHIVE_OUTPUT_DIRECTORY         "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
  ARCHIVE_OUTPUT_DIRECTORY_DEBUG   "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
  ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
)

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
  ${CMAKE_CURRENT_SOURCE_DIR}/${MODEL_NAME}/modelDescription.xml
  "${FMU_BUILD_DIR}/modelDescription.xml"
)

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E tar "cfv" ${CMAKE_CURRENT_BINARY_DIR}/fmus/${MODEL_NAME}.fmu --format=zip
  modelDescription.xml binaries
  WORKING_DIRECTORY ${FMU_BUILD_DIR} COMMENT "Creating ZIP archive"
)

endforeach(MODEL_NAME)
