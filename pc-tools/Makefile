.SUFFIXES: .cpp .o

CPP = g++
CCFLAGS = -Os -I$(KARMA) -I$(SCRIPTI)
LIBS = -lgcc_s -lGL -L/usr/X11R6/lib `sdl-config --libs` -lGLU 
BIN = intro

OBJS = \
	main.o


all: $(BIN)

$(BIN): $(OBJS)
	g++ $(CCFLAGS) -o $(BIN) $(OBJS) $(LIBS)

clean:
	rm -rf *~ $(OBJS) $(BIN)
	find -name "*~" -exec rm {} \;

.cpp.o:
	$(CPP) $(CCFLAGS) -c $< -o $@

