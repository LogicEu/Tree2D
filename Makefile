# Tree makefile

STD=-std=c99
WFLAGS=-Wall -Wextra
OPT=-O2
SLIBS=fract utopia glee gleex glui imgtool photon mass nano ethnic
DLIBS=glfw freetype z png jpeg enet
CC=gcc
NAME=Tree2D
SRC=src/*.c src/scripts/*.c src/UI/*.c src/client/*.c src/server/packet.c

SERVERSRC=src/server/*.c
SERVERNAME=T2Dserver

OS=$(shell uname -s)
ifeq ($(OS),Darwin)
	OSFLAGS=-framework OpenGL #-mmacos-version-min=10.9
else
	WLFLAG=-Wl,--whole-archive
	WRFLAG=-Wl,--no-whole-archive
	OSFLAGS=-lm -lGL -lGLEW
endif

LDIR=lib
IDIR=$(patsubst %,-I%/,$(SLIBS))
LSTATIC=$(patsubst %,lib%.a,$(SLIBS))
LPATHS=$(patsubst %,$(LDIR)/%,$(LSTATIC))
LFLAGS=$(patsubst %,-L%,$(LDIR))
LFLAGS += $(WLFLAG)
LFLAGS += $(patsubst %,-l%,$(SLIBS))
LFLAGS += $(WRFLAG)
LFLAGS += $(patsubst %,-l%,$(DLIBS))
LFLAGS += $(OSFLAGS)

CFLAGS=$(STD) $(WFLAGS) $(OPT) $(IDIR)

$(NAME): $(LPATHS) $(SRC)
	$(CC) -o $@ $(SRC) $(CFLAGS) $(LFLAGS)

$(LDIR)/$(LDIR)%.a: $(LDIR)%.a $(LDIR)
	mv $< $(LDIR)/

$(LDIR): 
	mkdir $@

$(LDIR)%.a: %
	cd $^ && make && mv $@ ../

client:
	$(CC) -o $(NAME) $(SRC) $(CFLAGS) $(LFLAGS)

server:
	$(CC) -o $(SERVERNAME) $(SERVERSRC) $(CFLAGS) $(LFLAGS)

app: $(NAME)
	mkdir $(NAME).app
	mkdir $(NAME).app/Contents/
	mkdir $(NAME).app/Contents/Resources/
	mkdir $(NAME).app/Contents/MacOS/
	cp $(NAME) $(NAME).app/Contents/MacOS/
	cp assets/MacOS/Info.plist $(NAME).app/Contents/
	cp assets/MacOS/icon.icns $(NAME).app/Contents/Resources/
	cp -r assets $(NAME).app/Contents/Resources/assets/
	tar -xzvf ../../Assets/lib.tar.gz -C $(NAME).app/Contents/MacOS/

clean:
	rm -r $(LDIR) && rm $(NAME)
