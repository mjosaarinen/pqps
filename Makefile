#	Makefile
#	2019-09-03	Markku-Juhani O. Saarinen <mjos@pqshield.com>

#	Derived from an automagically generated mbed.org Makefile.


# cross-platform directory manipulation
ifeq ($(shell echo $$OS),$$OS)
    MAKEDIR = if not exist "$(1)" mkdir "$(1)"
    RM = rmdir /S /Q "$(1)"
else
    MAKEDIR = '$(SHELL)' -c "mkdir -p \"$(1)\""
    RM = '$(SHELL)' -c "rm -rf \"$(1)\""
endif

OBJDIR := BUILD
# Move to the build directory
ifeq (,$(filter $(OBJDIR),$(notdir $(CURDIR))))
.SUFFIXES:
mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKETARGET = '$(MAKE)' --no-print-directory -C $(OBJDIR) -f '$(mkfile_path)' \
		'SRCDIR=$(CURDIR)' $(MAKECMDGOALS)
.PHONY: $(OBJDIR) clean
all:
	+@$(call MAKEDIR,$(OBJDIR))
	+@$(MAKETARGET)
$(OBJDIR): all
Makefile : ;
% :: $(OBJDIR) ; :
clean :
	$(call RM,$(OBJDIR))

else

# trick rules into thinking we are in the root, when we are in the bulid dir
VPATH = ..

###############################################################################
# Project settings

PROJECT := pqps

#PQALG	= pqm4/crypto_kem/kyber768/m4
#PQALG	= pqm4/crypto_kem/ntruhrss701/m4
#PQALG	= pqm4/crypto_kem/saber/m4
#PQALG	= pqm4/crypto_kem/r5nd-3kemcca-5d/m4
#PQALG	= pqm4/crypto_kem/frodokem640shake/m4
#PQALG	= pqm4/crypto_kem/frodokem640aes/m4
#PQALG	= pqm4/crypto_sign/falcon512/m4-ct


# Project settings
###############################################################################
# Objects and Paths

OBJECTS += main.o randombytes.o
OBJECTS += pqm4/common/aes.o pqm4/common/keccakf1600.o pqm4/mupq/common/aes.o
OBJECTS += pqm4/mupq/common/fips202.o pqm4/mupq/common/sha2.o pqm4/mupq/common/sp800-185.o
OBJECTS += $(patsubst ../%.c,%.o,$(wildcard ../$(PQALG)/*.c))
OBJECTS += $(patsubst ../%.s,%.o,$(wildcard ../$(PQALG)/*.s))
OBJECTS += $(patsubst ../%.S,%.o,$(wildcard ../$(PQALG)/*.S))
INCLUDE_PATHS += -I../pqm4/mupq/common -I../$(PQALG)

SYS_OBJECTS += ../mbed/TARGET_NUCLEO_F411RE/TOOLCHAIN_GCC_ARM/*.o

INCLUDE_PATHS += -I../.
#INCLUDE_PATHS += -I..//usr/src/mbed-sdk
INCLUDE_PATHS += -I../mbed
INCLUDE_PATHS += -I../mbed/TARGET_NUCLEO_F411RE
INCLUDE_PATHS += -I../mbed/TARGET_NUCLEO_F411RE/TARGET_STM
INCLUDE_PATHS += -I../mbed/TARGET_NUCLEO_F411RE/TARGET_STM/TARGET_STM32F4
INCLUDE_PATHS += -I../mbed/TARGET_NUCLEO_F411RE/TARGET_STM/TARGET_STM32F4/TARGET_STM32F411xE
INCLUDE_PATHS += -I../mbed/TARGET_NUCLEO_F411RE/TARGET_STM/TARGET_STM32F4/TARGET_STM32F411xE/TARGET_NUCLEO_F411RE
INCLUDE_PATHS += -I../mbed/TARGET_NUCLEO_F411RE/TARGET_STM/TARGET_STM32F4/TARGET_STM32F411xE/device
INCLUDE_PATHS += -I../mbed/TARGET_NUCLEO_F411RE/TARGET_STM/TARGET_STM32F4/device
INCLUDE_PATHS += -I../mbed/drivers
INCLUDE_PATHS += -I../mbed/hal
INCLUDE_PATHS += -I../mbed/platform

LIBRARY_PATHS := -L../mbed/TARGET_NUCLEO_F411RE/TOOLCHAIN_GCC_ARM 
LIBRARIES := -lmbed 
LINKER_SCRIPT ?= ../mbed/TARGET_NUCLEO_F411RE/TOOLCHAIN_GCC_ARM/STM32F411XE.ld

# Objects and Paths
###############################################################################
# Tools and Flags

TOOLCHAIN = arm-none-eabi-
AS		= $(TOOLCHAIN)gcc
CC		= $(TOOLCHAIN)gcc
AR		= $(TOOLCHAIN)ar
CPP     = $(TOOLCHAIN)g++
LD		= $(TOOLCHAIN)gcc

PREPROC = arm-none-eabi-cpp
ELF2BIN = arm-none-eabi-objcopy

MY_ARCH	= -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=softfp

C_DEFS	+= -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers
C_DEFS	+= -fmessage-length=0 -fno-exceptions -ffunction-sections
C_DEFS	+= -fdata-sections -funsigned-char -fno-delete-null-pointer-checks
C_DEFS	+= -fomit-frame-pointer
C_DEFS	+= -MMD $(INCLUDE_PATHS) $(MY_ARCH) -O3

C_FLAGS	+= -std=c99
C_FLAGS	+= -include mbed_config.h
C_FLAGS	+= $(C_DEFS)

CXX_FLAGS += -include mbed_config.h
CXX_FLAGS += -fno-rtti
CXX_FLAGS += $(C_DEFS)

ASM_FLAGS += -DTRANSACTION_QUEUE_SIZE_SPI=2
ASM_FLAGS += -D__CORTEX_M4 -DUSB_STM_HAL -DARM_MATH_CM4 -D__FPU_PRESENT=1
ASM_FLAGS += -DUSBHOST_OTHER -D__MBED_CMSIS_RTOS_CM -D__CMSIS_RTOS 
ASM_FLAGS += $(MY_ARCH) $(INCLUDE_PATHS)

#ASM_FLAGS += -O3

LD_FLAGS := -Wl,--gc-sections -Wl,--wrap,main -Wl,--wrap,_malloc_r -Wl,--wrap,_free_r -Wl,--wrap,_realloc_r -Wl,--wrap,_memalign_r -Wl,--wrap,_calloc_r -Wl,--wrap,exit -Wl,--wrap,atexit -Wl,-n $(MY_ARCH)
LD_SYS_LIBS := -Wl,--start-group -lstdc++ -lsupc++ -lm -lc -lgcc -lnosys -lmbed -Wl,--end-group

# Tools and Flags
###############################################################################
# Rules

all: $(PROJECT).hex

.c.o:
	+@$(call MAKEDIR,$(dir $@))
	+@echo "[CC] $(notdir $<)"
	@$(CC) -$(C_FLAGS)  -c -o $@ $<

.cpp.o:
	+@$(call MAKEDIR,$(dir $@))
	+@echo "[CPP] $(notdir $<)"
	@$(CPP) $(CXX_FLAGS)  -c -o $@ $<

.s.o:
	+@$(call MAKEDIR,$(dir $@))
	+@echo "[AS] $(notdir $<)"
	@$(AS) -c $(ASM_FLAGS) -o $@ $<
  
.S.o:
	+@$(call MAKEDIR,$(dir $@))
	+@echo "[AS] $(notdir $<)"
	@$(AS) -c $(ASM_FLAGS) -o $@ $<


$(PROJECT).link_script.ld: $(LINKER_SCRIPT)
#	+@echo "preproc: $(notdir $@)"	
#	@$(PREPROC) -E -P $(LD_FLAGS) $< -o $@
	cp $(LINKER_SCRIPT) $(PROJECT).link_script.ld

$(PROJECT).elf: $(OBJECTS)  $(SYS_OBJECTS) $(PROJECT).link_script.ld 
	+@echo "link: $(notdir $@)"
	+@echo "$(filter %.o, $^)" > .link_options.txt
	@$(LD) $(LD_FLAGS) -T $(filter-out %.o, $^) $(LIBRARY_PATHS) \
		--output $@ @.link_options.txt $(LIBRARIES) $(LD_SYS_LIBS)

$(PROJECT).bin: $(PROJECT).elf
	$(ELF2BIN) -O binary $< $@

$(PROJECT).hex: $(PROJECT).elf
	$(ELF2BIN) -O ihex $< $@

# flash it

flash: $(PROJECT).hex
#	st-flash reset
	st-flash --format ihex write $(PROJECT).hex

# Rules
###############################################################################
# Dependencies

DEPS = $(OBJECTS:.o=.d) $(SYS_OBJECTS:.o=.d)
-include $(DEPS)
endif


