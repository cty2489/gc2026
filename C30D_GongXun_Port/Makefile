TARGET := C30D_GongXun_Port
BUILD_DIR := build

TOOLCHAIN_BIN ?= /Applications/arm-gnu-toolchain-15.2.rel1-darwin-arm64-arm-none-eabi/bin
PREFIX ?= $(TOOLCHAIN_BIN)/arm-none-eabi-
CC := $(PREFIX)gcc
AS := $(PREFIX)gcc
OBJCOPY := $(PREFIX)objcopy
SIZE := $(PREFIX)size
PYOCD ?= python3 -m pyocd
PYOCD_TARGET ?= stm32f407zgtx
PYOCD_FREQ ?= 100000
PYOCD_CONNECT ?= under-reset

MCU_FLAGS := -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
DEFS := -DUSE_STDPERIPH_DRIVER -DSTM32F40_41xxx
INCLUDES := -IStart -ILibrary -ISystem -IHardware -IControl -ICarBody -IUser

CFLAGS := $(MCU_FLAGS) $(DEFS) $(INCLUDES) -std=c99 -Os -ffunction-sections -fdata-sections
CFLAGS += -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare -Wno-missing-field-initializers
CFLAGS += -Wno-misleading-indentation
ASFLAGS := $(MCU_FLAGS) $(DEFS) $(INCLUDES) -x assembler-with-cpp
LDFLAGS := $(MCU_FLAGS) -TSTM32F407ZG_FLASH.ld -specs=nano.specs -specs=nosys.specs
LDFLAGS += -Wl,--gc-sections -Wl,--no-warn-rwx-segments -Wl,-Map=$(BUILD_DIR)/$(TARGET).map
LIBS := -lm

ASM_SOURCES := Start/startup_stm32f40_41xxx_gcc.S

C_SOURCES := \
	Start/system_stm32f4xx.c \
	Library/misc.c \
	Library/stm32f4xx_adc.c \
	Library/stm32f4xx_can.c \
	Library/stm32f4xx_crc.c \
	Library/stm32f4xx_cryp.c \
	Library/stm32f4xx_cryp_aes.c \
	Library/stm32f4xx_cryp_des.c \
	Library/stm32f4xx_cryp_tdes.c \
	Library/stm32f4xx_dac.c \
	Library/stm32f4xx_dbgmcu.c \
	Library/stm32f4xx_dcmi.c \
	Library/stm32f4xx_dma.c \
	Library/stm32f4xx_exti.c \
	Library/stm32f4xx_flash.c \
	Library/stm32f4xx_fsmc.c \
	Library/stm32f4xx_gpio.c \
	Library/stm32f4xx_hash.c \
	Library/stm32f4xx_hash_md5.c \
	Library/stm32f4xx_hash_sha1.c \
	Library/stm32f4xx_i2c.c \
	Library/stm32f4xx_iwdg.c \
	Library/stm32f4xx_pwr.c \
	Library/stm32f4xx_rcc.c \
	Library/stm32f4xx_rng.c \
	Library/stm32f4xx_rtc.c \
	Library/stm32f4xx_sdio.c \
	Library/stm32f4xx_spi.c \
	Library/stm32f4xx_syscfg.c \
	Library/stm32f4xx_tim.c \
	Library/stm32f4xx_usart.c \
	Library/stm32f4xx_wwdg.c \
	System/Delay.c \
	System/syscalls.c \
	Hardware/Camera.c \
	Hardware/HWT101.c \
	Hardware/Key.c \
	Hardware/OLED.c \
	Hardware/QR.c \
	Hardware/Steer.c \
	Hardware/StepMotor.c \
	Control/PID.c \
	CarBody/Arm.c \
	CarBody/Gimbal.c \
	CarBody/Chassis_Path.c \
	CarBody/Chassis_PID.c \
	CarBody/Chassis.c \
	CarBody/zpc_zxc.c \
	User/main.c \
	User/zpc_zxc_Typedef.c \
	User/isr.c \
	User/Data.c \
	User/stm32f4xx_it.c

OBJECTS := $(addprefix $(BUILD_DIR)/,$(C_SOURCES:.c=.o))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(ASM_SOURCES:.S=.o))
DEPS := $(OBJECTS:.o=.d)

.PHONY: all clean flash flash-stlink list-probes size

all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) STM32F407ZG_FLASH.ld
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@
	$(SIZE) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf
	$(OBJCOPY) -O ihex $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf
	$(OBJCOPY) -O binary -S $< $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/%.o: %.S
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -MMD -MP -c $< -o $@

flash: $(BUILD_DIR)/$(TARGET).hex
	$(PYOCD) flash -t $(PYOCD_TARGET) --frequency $(PYOCD_FREQ) --connect $(PYOCD_CONNECT) $<

flash-stlink: $(BUILD_DIR)/$(TARGET).bin
	st-flash --connect-under-reset --freq=100k write $< 0x08000000

list-probes:
	$(PYOCD) list

size: $(BUILD_DIR)/$(TARGET).elf
	$(SIZE) $<

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)
