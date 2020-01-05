cmake_minimum_required(VERSION 3.10)

SET(THIS f)

# -------------------------- PRE-CONFIG -------------------------------------------------

if(WIN32)
    set(RADICAL_PATH "C:/Source/CMake/Modules")
else()
    set(RADICAL_PATH "/opt/Radicalware/CMake_Modules")
endif()

include("${RADICAL_PATH}/Radical-App-Config.cmake")

# -------------------------- BUILD ------------------------------------------------------

project (${THIS})

SUBDIRLIST(PROJECT_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Solution")

add_executable(${THIS} ${PROJECT_FILES})

find_package(Nexus)
find_package(xvector)
find_package(xstring)
find_package(xmap)
find_package(OS)
find_package(SYS)
find_package(Timer)

target_link_libraries( ${THIS} PRIVATE	

    radical::Nexus
    radical::xvector
    radical::xstring
    radical::xmap
    radical::OS
    radical::SYS
    radical::Timer
)

# -------------------------- POST-CONFIG ------------------------------------------------

include("${RADICAL_PATH}/Radical-App-Install.cmake")
CONFIGURE_VISUAL_STUDIO(${PROJECT_FILES})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# -------------------------- END --------------------------------------------------------