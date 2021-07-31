cmake_minimum_required(VERSION 3.16)

SET(THIS ${ArgProjectName})

# -------------------------- PRE-CONFIG -------------------------------------------------

if(WIN32)
    set(RADICAL_PATH "C:/Source/CMake/Modules")
else()
    set(RADICAL_PATH "/opt/Radicalware/CMake_Modules")
endif()

include("${RADICAL_PATH}/Radical-App-Config.cmake")

# -------------------------- BUILD ------------------------------------------------------

if(UNIX)
    message("This Tool was not designed for Nix Based Systems")
    message("I suggest you use ls bash tool as it is very similar")
    exit()
endif()

project (${THIS})

find_program_files(SOLUTION_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Solution")
add_executable(${THIS} ${SOLUTION_FILES})

find_package(Nexus)
find_package(xvector)
find_package(xstring)
find_package(xmap)
find_package(re2)

find_package(Timer)
find_package(cc)
find_package(OS)

target_include_directories(${THIS} PRIVATE
        ${installed_projects}
)

if(${debug} OR ${build_all})
    target_link_libraries( ${THIS}

        Radical::Nexus
        Radical::xvector
        Radical::xstring
        Radical::xmap
        
        Radical_Mod::re2
    )
endif()


# -------------------------- POST-CONFIG ------------------------------------------------

include("${RADICAL_PATH}/Radical-App-Install.cmake")
INSTALL_VISUAL_STUDIO_SOLUTION(${SOLUTION_FILES})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# -------------------------- END --------------------------------------------------------