cmake_minimum_required(VERSION 3.22)
project(libnp C)

include(GNUInstallDirs)

set(CMAKE_C_STANDARD 11)

add_library(libnp SHARED)
target_sources(libnp PRIVATE np_util.c)
target_include_directories(libnp PUBLIC include)
set_target_properties(libnp PROPERTIES PUBLIC_HEADER include/np.h)
set_target_properties(libnp PROPERTIES PREFIX "")

IF(WIN32)
    target_sources(libnp PRIVATE np_windows.c np_windows.def)
    target_link_libraries(libnp PUBLIC runtimeobject)
ELSEIF(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_search_module(DBUS REQUIRED IMPORTED_TARGET libsystemd libelogind basu)
    string(TOUPPER "${DBUS_MODULE_NAME}" DBUS_MODULE_NAME_UPPER)

    target_sources(libnp PRIVATE np_linux.c)
    target_compile_definitions(libnp PRIVATE _XOPEN_SOURCE=500 "HAVE_${DBUS_MODULE_NAME_UPPER}")
    target_link_libraries(libnp PUBLIC PkgConfig::DBUS)
ELSE()
    target_sources(libnp PRIVATE np_stub.c)
ENDIF()

add_executable(np np/main.c)
target_link_libraries(np PUBLIC libnp)

install(TARGETS libnp LIBRARY PUBLIC_HEADER)
install(TARGETS np RUNTIME)
