# CMakeLists.txt project file for Raylib projects
cmake_minimum_required(VERSION 3.30)
project(RayLib_Shooter C)

set(CMAKE_C_STANDARD 99)

# Include the command that downloads libraries
include(FetchContent)

# Define a function for adding git dependencies
function(include_dependency libName gitURL gitTag)
    # Setup the declare
    FetchContent_Declare(${libName}
            GIT_REPOSITORY ${gitURL}
            GIT_TAG        ${gitTag}
            GIT_SHALLOW    TRUE
            GIT_PROGRESS   TRUE
    )
    FetchContent_MakeAvailable(${libName})
endfunction()

# Add Raylib support
set(LIB1 raylib)
find_package(${LIB1} QUIET)
if (NOT ${LIB1}_FOUND)
    message(STATUS "Getting ${LIB1} from Github")
    include_dependency(${LIB1} https://github.com/raysan5/raylib.git 5.5)
else()
    message(STATUS "Using local ${LIB1}")
endif()

# Add executable
add_executable(RayLib_Shooter src/main.c
        src/game.c
        src/vector_math.c
        src/file_io.c
        src/draw.c
)

# Set the include directory
target_include_directories(RayLib_Shooter PRIVATE ${raylib_INCLUDE_DIRS})

# Link all libraries to the project (including winmm for Windows)
if (WIN32)
    target_link_libraries(RayLib_Shooter PRIVATE ${LIB1} winmm)
else()
    target_link_libraries(RayLib_Shooter PRIVATE ${LIB1})
endif()

