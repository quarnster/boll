#
# KallistiGL test program
# (c)2001 Benoit Miller
#   

TARGET = game.elf
OBJS = \
	lib/Point3D.o \
	game.o \
	level.o \
	object.o \
	player.o \
	ntscmenu.o \
	mainmenu.o \
	main.o

all: rm-elf $(TARGET)

include $(KOS_BASE)/Makefile.rules
KOS_CFLAGS += -DBETA -I/home/quarn/code/dreamcast/libq3d/include

clean:
	-rm -f $(TARGET) $(OBJS) *~

rm-elf:
	-rm -f $(TARGET)

$(TARGET): $(OBJS)
	$(KOS_CC) $(KOS_CFLAGS) $(CFLAGS) $(KOS_LDFLAGS) -o $(TARGET) $(KOS_START) \
		$(OBJS) $(OBJEXTRA) -L$(KOS_BASE)/lib -lq3d -lpng -lz -lm -lk++ $(KOS_LIBS)

run: $(TARGET)
	$(KOS_LOADER) $(TARGET)

dist:
	rm -f $(OBJS) romdisk.o romdisk.img
	$(KOS_STRIP)  $(TARGET)

