#
# KallistiGL test program
# (c)2001 Benoit Miller
#   

TARGET = game.elf
OBJS = \
	lib/primitives.o \
	lib/Point3D.o \
	lib/Keyframe.o \
	lib/Keyframer.o \
	resources.o \
	credits.o \
	game.o \
	level.o \
	object.o \
	player.o \
	ntscmenu.o \
	mainmenu.o \
	main.o

all: rm-elf $(TARGET)

include $(KOS_BASE)/Makefile.rules
ADD_INCS = -I/home/quarn/code/dreamcast/libq3d/include

KOS_CFLAGS += -DBETA $(ADD_INCS)

depend:
	echo "" > Makefile.dep
	makedepend $(KOS_INC_PATHS) $(ADD_INCS) -fMakefile.dep *.cc *.h

clean:
	-rm -f $(TARGET) $(OBJS) *~

rm-elf:
	-rm -f $(TARGET)

$(TARGET): $(OBJS)
	$(KOS_CC) $(KOS_CFLAGS) $(CFLAGS) $(KOS_LDFLAGS) -o $(TARGET) $(KOS_START) \
		$(OBJS) $(OBJEXTRA) -L$(KOS_BASE)/lib -lq3d -lpng -lz -lm -lk++ -lparallax -lkmg -lkosutils -ljpeg $(KOS_LIBS)

run: $(TARGET)
	$(KOS_LOADER) $(TARGET)

dist:
	rm -f $(OBJS) romdisk.o romdisk.img
	$(KOS_STRIP)  $(TARGET)

include Makefile.dep

