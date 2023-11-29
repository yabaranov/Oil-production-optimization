cmake_minimum_required(VERSION 3.27 FATAL_ERROR)

set(PROJECT_NAME Oil-production-optimization)
project(${PROJECT_NAME})

add_executable(${PROJECT_NAME} 
src/main.cpp
src/functions.h
src/HDPoM.h

src/GridData.h
src/GridData.cpp

src/MatrixVector.h
src/MatrixVector.cpp

src/DenseSLAE.h
src/DenseSLAE.cpp

src/GNM.h
src/GNM.cpp

)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC /MP)
endif()