cmake_minimum_required(VERSION 3.10)
project(ahb-wrapper C)

# Set version information
set(AHB_WRAPPER_VERSION_MAJOR 1)
set(AHB_WRAPPER_VERSION_MINOR 0)
set(AHB_WRAPPER_VERSION_PATCH 0)
set(AHB_WRAPPER_VERSION "${AHB_WRAPPER_VERSION_MAJOR}.${AHB_WRAPPER_VERSION_MINOR}.${AHB_WRAPPER_VERSION_PATCH}")

# Configure library
add_library(ahb-wrapper SHARED ahb-wrapper.c)

# Set include directories
target_include_directories(ahb-wrapper PRIVATE
    /usr/include/hybris
)

# Set library dependencies
target_link_libraries(ahb-wrapper
    hybris-common
)

# Set version properties
set_target_properties(ahb-wrapper PROPERTIES
    VERSION ${AHB_WRAPPER_VERSION}
    SOVERSION ${AHB_WRAPPER_VERSION_MAJOR}
)

# Install targets
install(TARGETS ahb-wrapper
    LIBRARY DESTINATION lib
)
