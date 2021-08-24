# Tree makefile

STD=-std=c99
WFLAGS=-Wall -Wextra
OPT=-O2
IDIR=-I. -Iinclude
SLIBS=utopia fract imgtool photon mass glee gleex glui ethnic nano
DLIBS=glfw freetype z png jpeg enet
CC=gcc
NAME=Tree2D
SRC=src/*.c src/scripts/*.c src/UI/*.c src/client/*.c src/server/packet.c

CFLAGS=$(STD) $(WFLAGS) $(OPT) $(IDIR)
OS=$(shell uname -s)

ifeq ($(OS),Darwin)
	OSFLAGS=-framework OpenGL -mmacos-version-min=10.9
else
    WLFLAG=-Wl,--whole-archive
    WRFLAG=-Wl,--no-whole-archive
	OSFLAGS=-lm -lGL -lGLEW
endif

LDIR=lib
LSTATIC=$(patsubst %,lib%.a,$(SLIBS))
LPATHS=$(patsubst %,$(LDIR)/%,$(LSTATIC))
LFLAGS=$(patsubst %,-L%,$(LDIR))
LFLAGS += $(WLFLAG)
LFLAGS += $(patsubst %,-l%,$(SLIBS))
LFLAGS += $(WRFLAG)
LFLAGS += $(patsubst %,-l%,$(DLIBS))
LFLAGS += $(OSFLAGS)

$(NAME): $(LPATHS) $(SRC)
	$(CC) -o $@ $(SRC) $(CFLAGS) $(LFLAGS)

$(LDIR)/$(LDIR)%.a: $(LDIR)%.a $(LDIR)
	mv $< $(LDIR)/

$(LDIR): 
	mkdir $@

$(LDIR)%.a: %
	cd $^ && make && mv $@ ../

clean:
	rm -r $(LDIR) && rm $(NAME) && rm imgtool/imgtool
