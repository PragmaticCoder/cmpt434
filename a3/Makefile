
SRCDIR = ./src
INCDIR = ./include
BUILDDIR = ./build

SRC = $(wildcard $(SRCDIR)/*.c)
INC = $(wildcard $(INCDIR)/*.h)

CC  = gcc
AS  = as
LD  = ld

CFLAGS  =  -g -O2 -Wall -Wextra -Werror -Wno-unused-function\
              -Wno-unused-but-set-parameter -Wno-unused-parameter -Wno-address\
              -Wno-unused-variable -Wno-return-local-addr -Wno-sign-compare -Wno-pointer-sign -rdynamic\
              -DNDEBUG
LDFLAGS	= 

all: build Server

Server: $(SRC) $(INC)
	$(CC) $(CFLAGS) -o $(BUILDDIR)/$@ $(SRC) -I $(INCDIR) -g

build:
	mkdir -p $(BUILDDIR)

clean:
	rm -fr $(BUILDDIR) 
