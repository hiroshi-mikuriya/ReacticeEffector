cmake_minimum_required(VERSION 3.6)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_SYSTEM_NAME Generic)

##########
# tool chain
##########
set(CMAKE_C_COMPILER   arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_EXE_LINKER   arm-none-eabi-g++)
set(OBJCOPY            arm-none-eabi-objcopy)
set(SIZE               arm-none-eabi-size)

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
# compiler options
##########
add_compile_options(-Os)
add_compile_options(-Wall)
add_compile_options(-ffunction-sections)
add_compile_options(-fdata-sections)
add_compile_options(-fstack-usage)

set(CMAKE_C_FLAGS "-mcpu=${CPU} -std=gnu11 ${FPU} ${FLOAT_ABI} -mthumb")
set(CMAKE_CXX_FLAGS "-mcpu=${CPU} -std=gnu++14 -fno-exceptions -fno-rtti -fno-use-cxa-atexit ${FPU} ${FLOAT_ABI} -mthumb")
set(CMAKE_AS_FLAGS "-mcpu=${CPU} -x assembler-with-cpp ${FPU} ${FLOAT_ABI} -mthumb")
set(CMAKE_EXE_LINKER_FLAGS "-mcpu=${CPU} -T${CMAKE_CURRENT_SOURCE_DIR}/${LDSCRIPT} --specs=nosys.specs -Wl,-Map=${MAP_FILE} -Wl,--gc-sections -static --specs=nano.specs ${FPU} ${FLOAT_ABI} -mthumb -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group")

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
	${CORE}/Src/*.c
	${CORE}/Startup/*.s
	${USER}/*.cpp
	${USER}/**/*.cpp
	${USER}/**/**/*.cpp
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
