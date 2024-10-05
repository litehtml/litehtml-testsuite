cmake_minimum_required(VERSION 3.29)
project(litehtml_testsuite)

set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message(STATUS "No build type selected, default to Release")
	set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -DDEBUG -g")
set(CMAKE_C_FLAGS_DEBUG "-O0 -DDEBUG -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_C_FLAGS_RELEASE "-O3")

pkg_check_modules(LB_LIBS REQUIRED gdkmm-3.0 gtkmm-3.0 libcurl cairo pango pangocairo fontconfig webkit2gtk-4.1)

add_subdirectory(litehtml)

# additional warnings
add_compile_options(-Wall -Wextra -Wpedantic)

add_executable(litehtml_testsuite
		src/main.cpp
		src/main_window.cpp
		src/main_window.h
		src/html_host.h
		src/html_widget.cpp
		src/html_widget.h
		src/http_request.cpp
		src/http_request.h
		src/http_requests_pool.cpp
		src/http_requests_pool.h
		src/web_history.cpp
		src/web_history.h
		src/web_page.cpp
		src/web_page.h
		litehtml/containers/cairo/cairo_borders.cpp
		litehtml/containers/cairo/cairo_borders.h
		litehtml/containers/cairo/container_cairo.cpp
		litehtml/containers/cairo/container_cairo.h
		litehtml/containers/cairo/container_cairo_pango.cpp
		litehtml/containers/cairo/container_cairo_pango.h
		litehtml/containers/cairo/cairo_images_cache.h
		litehtml-tests/fonts.cpp
		litehtml-tests/fonts.h
		src/source_viewer.cpp
		src/source_viewer.h
)

include_directories(${PROJECT_NAME} litehtml/include ${LB_LIBS_INCLUDE_DIRS} litehtml/containers/cairo litehtml-tests)
target_link_options(${PROJECT_NAME} PRIVATE ${LB_LIBS_LDFLAGS} ${FONTCONFIG_LDFLAGS})
target_link_libraries(${PROJECT_NAME} litehtml ${LB_LIBS_LIBRARIES} ${FONTCONFIG_LIBRARIES})

set_target_properties(${PROJECT_NAME} PROPERTIES
		CXX_STANDARD 17
		C_STANDARD 99
)

set(LITEHTML_PATH ${CMAKE_CURRENT_SOURCE_DIR}/litehtml)
set(LITEHTML_BUILD_RENDER2PNG ON)
set(LITEHTML_BUILD_MANAGE_FAILED ON)
add_subdirectory(litehtml-tests)
