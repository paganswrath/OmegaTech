
CFLAGS = -O3 --std=c++20
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
COMP = g++

OBJS = raygui.o \
  Encoder.o \
  Main.o

COREHEADERS = Source/Core.hpp \
  Encoder.o \
  Main.o


all: OTENGINE

OTENGINE: $(OBJS)

Main.o: Source/Main.cpp Source/*.hpp Source/Parasite/*.hpp
	$(COMP) $(CFLAGS) -c Source/Main.cpp $(LDFLAGS)

Encoder.o: Source/Encoder/Encoder.cpp
	$(COMP) $(CFLAGS) -c Source/Encoder/Encoder.cpp $(LDFLAGS)

raygui.o: Source/raygui/raygui.c
	$(COMP) $(CFLAGS) -c Source/raygui/raygui.c -shared -DRAYGUI_IMPLEMENTATION -DBUILD_LIBTYPE_SHARED -static-libgcc -lopengl32 -lgdi32 -lwinmm 


OTENGINE:
	$(COMP) $(OBJS) -o OmegaTech $(CFLAGS) $(LDFLAGS)