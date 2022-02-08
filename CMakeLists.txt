#THIS FILE IS AUTO GENERATED FROM THE TEMPLATE! DO NOT CHANGE!
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.19)

# specify cross compilers and tools
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER  arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(SIZE arm-none-eabi-size)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# project settings
project(Quadcopter C CXX ASM)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

#Uncomment for hardware floating point
add_compile_definitions(ARM_MATH_CM4;ARM_MATH_MATRIX_CHECK;ARM_MATH_ROUNDING)
add_compile_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)
add_link_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)

#Uncomment for software floating point
#add_compile_options(-mfloat-abi=soft)

add_compile_options(-mcpu=cortex-m4 -mthumb -mthumb-interwork)
add_compile_options(-ffunction-sections -fdata-sections -fno-common -fmessage-length=0)

# uncomment to mitigate c++17 absolute addresses warnings
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-register")

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message(STATUS "Maximum optimization for speed")
    add_compile_options(-Ofast)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    message(STATUS "Maximum optimization for speed, debug info included")
    add_compile_options(-Ofast -g)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    message(STATUS "Maximum optimization for size")
    add_compile_options(-Os)
else ()
    message(STATUS "Minimal optimization, debug info included")
    add_compile_options(-Og -g)
endif ()

include_directories(Core/Inc Drivers/STM32F4xx_HAL_Driver/Inc Drivers/STM32F4xx_HAL_Driver/Inc/Legacy Drivers/CMSIS/Device/ST/STM32F4xx/Include Drivers/CMSIS/Include)

add_definitions(-DUSE_HAL_DRIVER -DSTM32F401xC)

file(GLOB_RECURSE SOURCES "startup/*.*" "Drivers/*.*" "Core/*.*")

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F401CCUx_FLASH.ld)

add_link_options(-Wl,-gc-sections,--print-memory-usage,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)
add_link_options(-mcpu=cortex-m4 -mthumb -mthumb-interwork)
add_link_options(-T ${LINKER_SCRIPT})

################Lib add################
include_directories(Drivers/CMSIS/Lib/DSP/Include Lib/AIR530Z Lib/BATTERY Lib/FILTER Lib/HC-SR04 Lib/CONTROLLER Lib/IMU Lib/MOTO Lib/RETARGET Lib/SERIALWAVE Lib/SPL06 Lib/NRF24L01 Lib/MPU6050 Lib/GL9306)
add_library(LIB_UAV Lib/AIR530Z/air530z.c Lib/BATTERY/battery.c Lib/FILTER/filter.c Lib/HC-SR04/hc-sr04.c Lib/CONTROLLER/controller.c Lib/CONTROLLER/pid.c Lib/IMU/imu.c Lib/MOTO/moto.c Lib/RETARGET/retarget.c Lib/SERIALWAVE/serialwave.c Lib/SERIALWAVE/postureplot.c Lib/SPL06/spl06.c Lib/NRF24L01/nrf24l01.c Lib/MPU6050/mpu6050.c Lib/GL9306/gl9306.c)
link_libraries(LIB_UAV)

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

################link add################
target_link_libraries(${PROJECT_NAME}.elf ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Lib/DSP/libarm_cortexM4lf_math.a)

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}")
