cmake_minimum_required(VERSION 3.20)

###################### CONSTANTS ######################################
set (PROJECT_TYPE_EXECUTABLE          "exe")
set (MCPU_CORTEX_M0				      "-mcpu=cortex-m0")
set (MCPU_CORTEX_M0PLUS				  "-mcpu=cortex-m0plus")
set (MCPU_CORTEX_M3				      "-mcpu=cortex-m3")
set (MCPU_CORTEX_M4				      "-mcpu=cortex-m4")
set (MCPU_CORTEX_M7				      "-mcpu=cortex-m7")
set (MCPU_CORTEX_M33				  "-mcpu=cortex-m33")
set (MFPU_FPV4_SP_D16                 "-mfpu=fpv4-sp-d16")
set (MFPU_FPV5_D16                    "-mfpu=fpv5-d16")
set (RUNTIME_LIBRARY_REDUCED_C        "--specs=nano.specs")
set (RUNTIME_LIBRARY_STD_C            "")
set (RUNTIME_LIBRARY_SYSCALLS_MINIMAL "--specs=nosys.specs")
set (RUNTIME_LIBRARY_SYSCALLS_NONE    "")
set (MFLOAT_ABI_SOFTWARE              "-mfloat-abi=soft")
set (MFLOAT_ABI_HARDWARE              "-mfloat-abi=hard")
set (MFLOAT_ABI_MIX                   "-mfloat-abi=softfp")
#######################################################################

###################### VARIABLES ######################################
set (PROJECT_NAME             "cmake-f405")
set (PROJECT_TYPE             "exe")
set (LINKER_SCRIPT            "${CMAKE_SOURCE_DIR}/STM32F405RGTX_FLASH.ld")
set (MCPU                     "-mcpu=cortex-m4")
set (MFPU                 "-mfpu=fpv4-sp-d16")
set (MFLOAT_ABI               ${MFLOAT_ABI_HARDWARE})
set (RUNTIME_LIBRARY          "--specs=nano.specs")
set (RUNTIME_LIBRARY_SYSCALLS "--specs=nosys.specs")

# Path to the STM32CubeF4 repository
set(STM32CUBEF4_PATH ${CMAKE_SOURCE_DIR}/src/stm32f4-lib)

# STM32 Chip
set(STM32_CHIP "STM32F405xx")

set (PROJECT_SOURCES
	# LIST SOURCE FILES HERE
	startup/startup_stm32f405rgtx.s
	src/main.c
	src/syscalls.c
	src/sysmem.c
	src/system_stm32f4xx.c
	src/stm32f4xx_it.c
	src/stm32f4xx_hal_msp.c

	
	)

set (STM32_SOURCES
	# HAL
	${STM32CUBEF4_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
	${STM32CUBEF4_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
	${STM32CUBEF4_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
	${STM32CUBEF4_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
)

set (PROJECT_INCLUDES
	# LIST INCLUDE DIRECTORIES HERE
	${STM32CUBEF4_PATH}/Drivers/STM32F4xx_HAL_Driver/Inc
	${STM32CUBEF4_PATH}/Drivers/CMSIS/Include
	${STM32CUBEF4_PATH}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
	${CMAKE_SOURCE_DIR}/src

    )

############ MODIFY ACCORDING TO REQUIREMENTS) ########################

#######################################################################

################## PROJECT SETUP ######################################
project(${PROJECT_NAME} C ASM)

set (CMAKE_SYSTEM_PROCESSOR "arm" CACHE STRING "")
set (CMAKE_SYSTEM_NAME "Generic" CACHE STRING "")

# Specify toolchain.   
set(TOOLCHAIN_PREFIX   "arm-none-eabi-")
set(CMAKE_C_COMPILER   "${TOOLCHAIN_PREFIX}gcc")
set(CMAKE_ASM_COMPILER "${TOOLCHAIN_PREFIX}gcc")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_PREFIX}g++")
set(CMAKE_AR           "${TOOLCHAIN_PREFIX}ar")
set(CMAKE_LINKER       "${TOOLCHAIN_PREFIX}ld")
set(CMAKE_OBJCOPY      "${TOOLCHAIN_PREFIX}objcopy")
set(CMAKE_RANLIB       "${TOOLCHAIN_PREFIX}ranlib")
set(CMAKE_SIZE         "${TOOLCHAIN_PREFIX}size")
set(CMAKE_STRIP        "${TOOLCHAIN_PREFIX}ld")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Remove Windows-specific linker flags
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_FLAGS "")
# for vscode intellisense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# elf
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${CMAKE_PROJECT_NAME}>)
# bin
add_custom_command(TARGET cmake-f405 POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary
    "${CMAKE_BINARY_DIR}/cmake-f405.elf"
    "${CMAKE_BINARY_DIR}/cmake-f405.bin"
    COMMENT "Converting ELF to BIN"
    VERBATIM
)

target_include_directories (${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDES})
# Add HAL source files
target_sources(${PROJECT_NAME} PRIVATE ${STM32_SOURCES})
# Define STM32F405xx preprocessor macro
target_compile_definitions(${PROJECT_NAME} PRIVATE ${STM32_CHIP})
target_compile_definitions(${PROJECT_NAME} PRIVATE "USE_HAL_DRIVER")

set (CMAKE_EXECUTABLE_SUFFIX ".elf")
set (CMAKE_C_FLAGS "${MCPU} -std=gnu11 ${MFPU} ${MFLOAT_ABI} ${RUNTIME_LIBRARY} -mthumb -Wall -Werror")
set (CMAKE_EXE_LINKER_FLAGS "-T${LINKER_SCRIPT} ${RUNTIME_LIBRARY_SYSCALLS} -Wl,-Map=test.map -Wl,--gc-sections -static -Wl,--start-group -lc -lm -Wl,--end-group")
set (CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp")
