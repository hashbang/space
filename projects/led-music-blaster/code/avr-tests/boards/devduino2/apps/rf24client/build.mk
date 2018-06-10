#
# example: NODE
#

## paths

VPATH += $(PRJ_DIR)/boards/$(PLAT)/apps/rf24client
VPATH += $(PRJ_DIR)/boards/$(PLAT)/bsp/clock
VPATH += $(PRJ_DIR)/boards/$(PLAT)/bsp/leds
VPATH += $(PRJ_DIR)/boards/$(PLAT)/bsp/spi
VPATH += $(PRJ_DIR)/boards/$(PLAT)/bsp/vcc
VPATH += $(PRJ_DIR)/boards/$(PLAT)/bsp/adc
VPATH += $(PRJ_DIR)/boards/$(PLAT)/bsp/temp

## sources

NODE_SRCS := \
	main.c	\
	uart.c	\
	leds.c	\
	spi.c	\
	clock.c	\
	radio.c \
	vcc.c	\
	adc.c	\
	temp_mcp9700.c \

NODE_OBJS := $(NODE_SRCS:.c=.o)
NODE_OBJS := $(addprefix $(OBJ_DIR)/,$(NODE_OBJS))

## deps

LIBS = $(LIBNRF24)

## flags

CFLAGS	= -Wall $(PFLAGS)

CFLAGS	+= -I$(PRJ_DIR)/include
CFLAGS	+= -I$(PRJ_DIR)/boards/$(PLAT)/apps/rf24client

CFLAGS	+= $(LIBNRF24_INC)

LDFLAGS	= -Wl,-Map,$(PRG).map

## rules

rf24client: $(OBJ_DIR)/rf24client.hex $(OBJ_DIR)/rf24client.bin
	cp $(OBJ_DIR)/rf24client.hex $(OBJ_DIR)/firmware.hex
	avr-size --mcu=$(CHIP) --format=avr $(OBJ_DIR)/rf24client.elf

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $^ $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@

$(OBJ_DIR)/rf24client.elf: $(NODE_OBJS) $(LIBS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(NODE_OBJS) $(LIBS)

$(OBJ_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^
