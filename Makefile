#
# KallistiGL test program
# (c)2001 Benoit Miller
#   

TARGET = game.elf
OBJS = \
	mainmenu.o \
	object.o \
	player.o \
	main.o \
	romdisk.o

all: rm-elf $(TARGET)

include $(KOS_BASE)/Makefile.rules
KOS_CFLAGS += -I/home/quarn/code/dreamcast/libq3d/include

clean:
	-rm -f $(TARGET) $(OBJS) romdisk.* *~

rm-elf:
	-rm -f $(TARGET) romdisk.*

$(TARGET): $(OBJS)
	$(KOS_CC) $(KOS_CFLAGS) $(CFLAGS) $(KOS_LDFLAGS) -o $(TARGET) $(KOS_START) \
		$(OBJS) $(OBJEXTRA) -L$(KOS_BASE)/lib -lq3d -lpng -lz -lm $(KOS_LIBS)

romdisk.img:
	$(KOS_GENROMFS) -f romdisk.img -d romdisk -v

romdisk.o: romdisk.img
	$(KOS_BASE)/utils/bin2o/bin2o romdisk.img romdisk romdisk.o

run: $(TARGET)
	$(KOS_LOADER) $(TARGET)

dist:
	rm -f $(OBJS) romdisk.o romdisk.img
	$(KOS_STRIP)  $(TARGET)

