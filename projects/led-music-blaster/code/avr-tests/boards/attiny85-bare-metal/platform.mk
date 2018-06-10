#
# arch: avr
# chip: attiny85
# platform: bare metal attiny85
# description:
#   http://www.atmel.com/ru/ru/devices/ATTINY85.aspx
#   http://www.engbedded.com/fusecalc
#   http://www.jdunman.com/ww/AmateurRadio/SDR/helix_air_net_au%20%20AVRDUDE%20and%20FTDI%20232H.htm
#
# Schematics of attiny85 programmer based on UM232H is shown on docs/avr-isp-using-um232h.png
#

## toolchain

CROSS_COMPILE = avr

CC	= $(CROSS_COMPILE)-gcc
LD	= $(CROSS_COMPILE)-ld
OBJCOPY = $(CROSS_COMPILE)-objcopy
OBJDUMP	= $(CROSS_COMPILE)-objdump

## dependencies

NRF24_CFG_FLAGS = -DNRF24_LOG_LEVEL=1 -DCONFIG_LIB_RF24_SIZEOPT

deps: libnrf24 libsoftuart nanopb

## platform compile flags

CHIP		= attiny85
CLK_FREQ    = 1000000L

PFLAGS = -mmcu=$(CHIP) -DF_CPU=$(CLK_FREQ) \
	-Os -fshort-enums -ffunction-sections -Wl,--gc-sections

## platform flash flags

# UM232H programmer

SERIAL_PORT = /dev/ttyUSB0
PROGRAMMER_UM232H = UM232H
SPI_BIT_CLK = 100000

DUDE_OPTIONS_UM232H =		\
	-p $(CHIP)		\
	-P $(SERIAL_PORT)	\
	-c $(PROGRAMMER_UM232H)	\
	-b $(SPI_BIT_CLK)

# FT232RL programmer

FTDI_PORT = ft0
PROGRAMMER_FT232RL  = arduino-ft232r
FTDI_BBSYNC_RATE = 2400

DUDE_OPTIONS_FT232RL =		\
	-p $(CHIP)		\
	-P $(FTDI_PORT)		\
	-c $(PROGRAMMER_FT232RL)\
	-B $(FTDI_BBSYNC_RATE)

## projects for arduino-mini

TARGETS = "led-gpio led-pwm softuart-test adc-test gas-sensor rf24client rf24node w1-test"

ifeq ($(MAKECMDGOALS), led-gpio)
include $(PRJ_DIR)/boards/$(PLAT)/apps/led-gpio/build.mk
endif

ifeq ($(MAKECMDGOALS), led-pwm)
include $(PRJ_DIR)/boards/$(PLAT)/apps/led-pwm/build.mk
endif

ifeq ($(MAKECMDGOALS), softuart-test)
include $(PRJ_DIR)/boards/$(PLAT)/apps/softuart-test/build.mk
endif

ifeq ($(MAKECMDGOALS), adc-test)
include $(PRJ_DIR)/boards/$(PLAT)/apps/adc-test/build.mk
endif

ifeq ($(MAKECMDGOALS), gas-sensor)
include $(PRJ_DIR)/boards/$(PLAT)/apps/gas-sensor/build.mk
endif

ifeq ($(MAKECMDGOALS), rf24client)
include $(PRJ_DIR)/boards/$(PLAT)/apps/rf24client/build.mk
endif

ifeq ($(MAKECMDGOALS), rf24node)
include $(PRJ_DIR)/boards/$(PLAT)/apps/rf24node/build.mk
endif

ifeq ($(MAKECMDGOALS), w1-test)
include $(PRJ_DIR)/boards/$(PLAT)/apps/w1-test/build.mk
endif

## upload rules

upload: upload-ft232rl

upload-ft232rl:
	avrdude $(DUDE_OPTIONS_FT232RL) -U flash:w:out/firmware.hex -U lfuse:w:0x62:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m

upload-um232h:
	avrdude $(DUDE_OPTIONS_UM232H) -U flash:w:out/firmware.hex -U lfuse:w:0x62:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m

# Fuse bits:
#	- to set 8MHz:  -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m
#	- to set 1MHz:	-U lfuse:w:0x62:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m
