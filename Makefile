CC = gcc
CFLAGS = -std=c23 -Wall -Wextra
LDFLAGS = -lm -lSDL2
EXEC_NAME = chip8-emulator 
BUILDDIR = ./build
OBJECTS = main.o chip8.o config.o platform.o

vpath %.h ./include
vpath %.c ./src

all : $(BUILDDIR)/$(EXEC_NAME)

$(BUILDDIR)/$(EXEC_NAME) : $(patsubst %.o, $(BUILDDIR)/%.o, $(OBJECTS)) 
	$(CC) $^ -o $@ $(LDFLAGS)  
	
$(BUILDDIR)/main.o : main.c chip8.h config.h platform.h
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@ 

$(BUILDDIR)/chip8.o : chip8.c chip8.h
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/config.o : config.c config.h 
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/platform.o : platform.c platform.h config.h chip8.h
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@


.PHONY : clean all
clean : 
	rm -rf $(BUILDDIR)
