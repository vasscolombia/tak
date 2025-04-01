# The Flutter tooling requires that developers have CMake 3.10 or later
# installed. You should not increase this version, as doing so will cause
# the plugin to fail to compile for some customers of the plugin.
cmake_minimum_required(VERSION 3.19)

project(tak_flutter_wrapper VERSION 0.0.1 LANGUAGES C CXX)

function(decode_url_path INPUT_PATH OUTPUT_VARIABLE)
    execute_process(
        COMMAND python3 -c "import urllib.parse; print(urllib.parse.unquote('${INPUT_PATH}'))"
        OUTPUT_VARIABLE DECODED_PATH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(${OUTPUT_VARIABLE} ${DECODED_PATH} PARENT_SCOPE)
endfunction()

decode_url_path("${CMAKE_CURRENT_SOURCE_DIR}" DECODED_SOURCE_DIR)

set(TAK_ROOT_FOLDER "${DECODED_SOURCE_DIR}")
set(TOOLS_DIR "${TAK_ROOT_FOLDER}/tools")
set(TAK_LIBS_DIR "${TAK_ROOT_FOLDER}/libs")
set(TAK_WBC_DIR "${TAK_ROOT_FOLDER}/libs")
set(SOURCE_DIR "${DECODED_SOURCE_DIR}")

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin")
    set(UPDATE_BINARY "${TOOLS_DIR}/UpdateBinary/macos/UpdateBinary")
    execute_process(
        COMMAND chmod +x "${UPDATE_BINARY}"
        RESULT_VARIABLE CHMOD_RESULT
    )
    if(NOT CHMOD_RESULT EQUAL 0)
        message(WARNING "Failed to set executable permissions on UpdateBinary")
    endif()

elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
    set(UPDATE_BINARY "${TOOLS_DIR}/UpdateBinary/windows/UpdateBinary.exe")
    if(NOT EXISTS "${UPDATE_BINARY}")
        message(WARNING "UpdateBinary.exe not found at ${UPDATE_BINARY}")
    endif()

elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    set(UPDATE_BINARY "${TOOLS_DIR}/UpdateBinary/linux/UpdateBinary")
    if(NOT EXISTS "${UPDATE_BINARY}")
        message(WARNING "UpdateBinary not found at ${UPDATE_BINARY}")
    endif()
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
                       "${TAK_LIBS_DIR}/${ANDROID_ABI}/libTAK.a"
                       "${TAK_WBC_DIR}/${ANDROID_ABI}/libWBC.a")

target_compile_definitions(tak_flutter_wrapper PUBLIC DART_SHARED_LIB)

# UpdateBinary
decode_url_path("${CMAKE_LIBRARY_OUTPUT_DIRECTORY}" DECODED_OUTPUT_DIR)
set(BINARY_PATH "${DECODED_OUTPUT_DIR}/libtak_flutter_wrapper.so")
set(ATP_FILE "${TAK_LIBS_DIR}/${ANDROID_ABI}/tak.c.atp")

if(EXISTS "${UPDATE_BINARY}")
    add_custom_command(
        TARGET tak_flutter_wrapper 
        POST_BUILD
        COMMAND "${UPDATE_BINARY}" --bin=${BINARY_PATH} --file=${ATP_FILE}
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Executing UpdateBinary for ${ANDROID_ABI}"
    )
else()
    message(WARNING "UpdateBinary not found at ${UPDATE_BINARY}")
endif()