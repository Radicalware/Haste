cmake_minimum_required(VERSION 3.10)

SET(THIS l)

if(UNIX)
	message("This Tool was not designed for Nix Based Systems")
	message("I suggest you use ls bash tool as it is very similar")
    exit()
endif()

# -------------------------- CONFIG -----------------------------------------------------

if(WIN32)
    set(RADICAL_PATH "C:/Source/CMake/Modules")
else()
    set(RADICAL_PATH "/opt/Radicalware/CMake_Modules")
endif()

include("${RADICAL_PATH}/Radical-App-Config.cmake")

# -------------------------- BUILD ------------------------------------------------------

project (${THIS})

add_executable(${THIS}
    ${SRC}/${THIS}.cpp

    ${INC}/core.h
    ${SRC}/core.cpp

    ${INC}/LibLoc.h
    ${SRC}/LibLoc.cpp
)

find_package(Nexus)
find_package(xvector)
find_package(xstring)
find_package(xmap)
find_package(OS)
find_package(cc)

target_link_libraries( ${THIS} PRIVATE		

    radical::Nexus
    radical::xvector
    radical::xstring
    radical::xmap
    radical::OS
	
    radical_mod::cc
)

# -------------------------- INSTALL ------------------------------------------

include("${RADICAL_PATH}/Radical-App-Install.cmake")

# -------------------------- END ----------------------------------------------
