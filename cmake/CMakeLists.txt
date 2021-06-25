cmake_minimum_required(VERSION 3.6)
 
set(CMAKE_SYSTEM_NAME Generic)

##########
# architecture
##########
set(CPU       cortex-m7)
set(CHIP      STM32F745)
set(LDSCRIPT  STM32F745VGTX_FLASH.ld)
set(FPU       -mfpu=fpv5-sp-d16)
set(FLOAT_ABI -mfloat-abi=hard)

##########
# project name
##########
project(ReacticeEffector)

set(ELF_FILE ${PROJECT_NAME}.elf)
set(HEX_FILE ${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_NAME}.bin)
set(MAP_FILE ${PROJECT_NAME}.map)

##########
# compiler
##########
execute_process(COMMAND which arm-none-eabi-gcc     OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE CMAKE_C_COMPILER)
execute_process(COMMAND which arm-none-eabi-gcc     OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE CMAKE_ASM_COMPILER)
execute_process(COMMAND which arm-none-eabi-g++     OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE CMAKE_CXX_COMPILER)
execute_process(COMMAND which arm-none-eabi-g++     OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE CMAKE_EXE_LINKER)
execute_process(COMMAND which arm-none-eabi-objcopy OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE OBJCOPY)
execute_process(COMMAND which arm-none-eabi-size    OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE SIZE)

##########
# compiler options
##########
add_compile_options(-mcpu=${CPU})
add_compile_options(-mthumb)
add_compile_options(${FPU})
add_compile_options(${FLOAT_ABI})
add_compile_options(-O2)
add_compile_options(-Wall)
add_compile_options(-fdata-sections)
add_compile_options(-ffunction-sections)
add_compile_options(-MMD)
add_compile_options(-MP)

set(CMAKE_C_FLAGS -std=gnu99)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS -fno-use-cxa-atexit)
set(CMAKE_AS_FLAGS "-x assembler-with-cpp")

add_link_options(-mcpu=${CPU})
add_link_options(-mthumb)
add_link_options(${FPU})
add_link_options(${FLOAT_ABI})
add_link_options(--specs=nosys.specs)
add_link_options(-T${CMAKE_CURRENT_SOURCE_DIR}/${LDSCRIPT})
add_link_options(-lc)
add_link_options(-lm)
add_link_options(-lnosys)
add_link_options(-Wl,-Map=${MAP_FILE},--cref)
add_link_options(-Wl,--gc-sections)

enable_language(ASM)

##########
# defines
##########
add_definitions(-D${CHIP}xx)
add_definitions(-DUSE_FULL_LL_DRIVER)
add_definitions(-DUSE_HAL_DRIVER)

##########
# directory name
##########
set(ROOT ..)
set(CORE ${ROOT}/Core)
set(USER ${ROOT}/User)
set(DRIVER ${ROOT}/Drivers/STM32F7xx_HAL_Driver)
set(CMSIS ${ROOT}/Drivers/CMSIS)
set(RTOS ${ROOT}/Middlewares/Third_Party/FreeRTOS/Source)
set(USB ${ROOT}/USB_DEVICE)
set(USBD ${ROOT}/Middlewares/ST/STM32_USB_Device_Library)

##########
# header include path
##########
include_directories(
	${CORE}/Inc
	${USER}
	${DRIVER}/Inc
	${CMSIS}/Device/ST/STM32F7xx/Include
	${CMSIS}/Include
	${RTOS}/include
	${RTOS}/CMSIS_RTOS
	${RTOS}/portable/GCC/ARM_CM7/r0p1
	${USB}/App
	${USB}/Target
	${USBD}/Class/CDC/Inc
	${USBD}/Core/Inc
)

##########
# source files
##########
file(GLOB SRCS
	${CORE}/Src/main.c
	${CORE}/Src/freertos.c
	${CORE}/Src/stm32f7xx_it.c
	${CORE}/Src/stm32f7xx_hal_msp.c
	${CORE}/Src/stm32f7xx_hal_timebase_tim.c
	${CORE}/Src/system_stm32f7xx.c
	${CORE}/Startup/*.s
	${USER}/*.cpp
	${USER}/device/*.cpp
	${USER}/task/*.cpp
	${DRIVER}/Src/*.c
	${RTOS}/*.c
	${RTOS}/CMSIS_RTOS/*.c
	${RTOS}/portable/MemMang/*.c
	${RTOS}/portable/GCC/ARM_CM7/r0p1/*.c
	${USB}/App/*.c
	${USB}/Target/*.c
	${USBD}/Class/CDC/Src/*.c
	${USBD}/Core/Src/*.c
)

##########
# products
##########
add_executable(${ELF_FILE} ${SRCS})

add_custom_command(TARGET ${ELF_FILE} POST_BUILD
	COMMAND ${OBJCOPY} -Obinary ${ELF_FILE} ${BIN_FILE}
	COMMAND ${OBJCOPY} -Oihex   ${ELF_FILE} ${HEX_FILE}
	COMMAND ${SIZE} --format=berkeley ${ELF_FILE} ${HEX_FILE}
	COMMENT "Invoking: Cross ARM GNU Print Size"
)
