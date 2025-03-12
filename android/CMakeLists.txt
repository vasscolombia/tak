# The Flutter tooling requires that developers have CMake 3.10 or later
# installed. You should not increase this version, as doing so will cause
# the plugin to fail to compile for some customers of the plugin.
cmake_minimum_required(VERSION 3.10.2)

project(tak_flutter_wrapper VERSION 0.0.1 LANGUAGES C CXX)

set(TAK_ROOT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR})
set(TOOLS_DIR ${TAK_ROOT_FOLDER}/tools)
set(TAK_LIBS_DIR ${TAK_ROOT_FOLDER}/libs)
set(TAK_WBC_DIR ${TAK_ROOT_FOLDER}/libs)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
    set(UPDATE_BINARY ${TOOLS_DIR}/UpdateBinary/windows/UpdateBinary.exe)
elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin")
    set(UPDATE_BINARY ${TOOLS_DIR}/UpdateBinary/macos/UpdateBinary)
else()
    set(UPDATE_BINARY ${TOOLS_DIR}/UpdateBinary/linux/UpdateBinary)
endif()

add_definitions( "-DTARGET_ANDROID" )

add_library(tak_flutter_wrapper SHARED
  "../src/native_tak.cpp"
  "../android/src/main/cpp/environmentProvider.cpp"
)

target_include_directories(tak_flutter_wrapper PRIVATE src/main/cpp/)

target_link_libraries( tak_flutter_wrapper
                       android
                       log
                       stdc++
                       ${TAK_LIBS_DIR}/${ANDROID_ABI}/libTAK.a
                       ${TAK_WBC_DIR}/${ANDROID_ABI}/libWBC.a )

target_compile_definitions(tak_flutter_wrapper PUBLIC DART_SHARED_LIB)

# UpdateBinary
set(BINARY_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libtak_flutter_wrapper.so)
set(ATP_FILE ${TAK_LIBS_DIR}/${ANDROID_ABI}/tak.c.atp)
add_custom_command( TARGET tak_flutter_wrapper POST_BUILD
                    COMMAND ${UPDATE_BINARY} --bin=${BINARY_PATH} --file=${ATP_FILE}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    COMMENT "Executing UpdateBinary for ${ANDROID_ABI}" )
