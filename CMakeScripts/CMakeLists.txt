#=== Prepare environment ===#
cmake_minimum_required(VERSION 3.16)

# Include system environment
include(toolchain.cmake)

# Set project name
project(ENCODER C ASM)

# Set project composition method
# Valid values: [COMP_PER_FILES ; COMP_PER_FOLDERS]
set(PROJECT_COMPOSITION COMP_PER_FOLDERS)

# Set C standard
set(CMAKE_C_STANDARD 99)
# Set microcontroller
set(CMAKE_SYSTEM_PROCESSOR cortex-m3)
# Binary file extension
set(CMAKE_EXECUTABLE_SUFFIX ".elf")
# Linker script file
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/../Config/STM32F103_FLASH.ld)
# Set startup file
set(STARTUP_FILE ${CMAKE_SOURCE_DIR}/../StartUp/startup_stm32f10x_md.s)
#set(SRC_RTT_ASM_DIR ${CMAKE_SOURCE_DIR}/../../SEGGER_RTT/SEGGER_RTT_ASM_ARMv7M.S)
set(SPL "${CMAKE_SOURCE_DIR}/../../STM32F10x_StdPeriph_Lib_V3.5.0/Libraries")

# Adding compiler/linker options
include(compiler.cmake)

add_definitions(
        -DSTM32F10X_MD
        -DUSE_STDPERIPH_DRIVER
        )

file(GLOB_RECURSE CIRCULAR_BUFFER "${CMAKE_SOURCE_DIR}/../Circular_Buffer/Latest/circ_buffer.c")
file(GLOB_RECURSE SOFT_TIMER "${CMAKE_SOURCE_DIR}/../Timer/software_timer.c")
file(GLOB_RECURSE TERMINAL "${CMAKE_SOURCE_DIR}/../Terminal/terminal.c")
file(GLOB_RECURSE MODULE_RAK811 "${CMAKE_SOURCE_DIR}/../ModuleRAK811/ModuleRAK811.c")
file(GLOB_RECURSE CHECKSUM "${CMAKE_SOURCE_DIR}/../CheckSum/checksum.c")


file(GLOB_RECURSE MAIN_SOURCES
        "${CMAKE_SOURCE_DIR}/../source/main.c"
        "${CMAKE_SOURCE_DIR}/../source/Init.c"
        "${CMAKE_SOURCE_DIR}/../source/system_stm32f10x.c"
        "${CMAKE_SOURCE_DIR}/../source/Physical_layer.c"
        "${CMAKE_SOURCE_DIR}/../source/System_management.c"
#        "${CMAKE_SOURCE_DIR}/../source/DL_PrecessDataHandler.c"
        "${CMAKE_SOURCE_DIR}/../source/DL_OnRequestDataHandler.c"
        "${CMAKE_SOURCE_DIR}/../source/DL_ModeHandler.c"
        "${CMAKE_SOURCE_DIR}/../source/DL_MessageHandler.c"
        "${CMAKE_SOURCE_DIR}/../source/Data_layer.c"
        )

file(GLOB_RECURSE SPL_SOURCES
        "${SPL}/STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.c"
        "${SPL}/STM32F10x_StdPeriph_Driver/src/stm32f10x_gpio.c"
        "${SPL}/STM32F10x_StdPeriph_Driver/src/stm32f10x_usart.c"
        "${SPL}/STM32F10x_StdPeriph_Driver/src/stm32f10x_tim.c"
        "${SPL}/STM32F10x_StdPeriph_Driver/src/stm32f10x_dma.c"
        "${SPL}/STM32F10x_StdPeriph_Driver/src/misc.c"
)


include_directories("${SPL}/CMSIS/CM3/CoreSupport")
include_directories("${SPL}/CMSIS/CM3/DeviceSupport/ST/STM32F10X")
include_directories("${SPL}/STM32F10x_StdPeriph_Driver/inc")
include_directories(${CMAKE_SOURCE_DIR}/../include)
include_directories(${CMAKE_SOURCE_DIR}/../Terminal)
include_directories(${CMAKE_SOURCE_DIR}/../Timer)
include_directories(${CMAKE_SOURCE_DIR}/../Circular_Buffer/Latest)
include_directories(${CMAKE_SOURCE_DIR}/../ModuleRAK811)
include_directories(${CMAKE_SOURCE_DIR}/../CheckSum)



add_executable( ${PROJECT_NAME}
        ${MAIN_SOURCES}
        ${SPL_SOURCES}
        ${STARTUP_FILE}
        ${SOFT_TIMER}
        ${TERMINAL}
        ${MODULE_RAK811}
        ${CIRCULAR_BUFFER}
        ${CHECKSUM}
)


#=== Post-Build operations ===#

# Generate HEX-file
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY}
        ARGS -Oihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
        COMMENT "Generate ${PROJECT_NAME}.hex file"
        )

# Generate SREC-file
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY}
        ARGS -Osrec ${PROJECT_NAME}.elf ${PROJECT_NAME}.s19
        COMMENT "Generate ${PROJECT_NAME}.s19 file"
        )

# Generate BIN-file

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY}
        ARGS -Obinary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
        COMMENT "Generate ${PROJECT_NAME}.bin file"
        )


# Show memory usage
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${GCC_SIZE}
        ARGS --format=berkeley -t ${PROJECT_NAME}.elf
        COMMENT "Memory usage:"
        )