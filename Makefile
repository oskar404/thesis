#
# FILE:         Makefile
# DESCRIPTION:  Reproduction of "Model Based Visualization
#               of Paper" build system
#

IO_SRC := $(wildcard src/io/*.c)
TIF_SRC := $(filter-out src/tif/tif_msdo.c ,$(wildcard src/tif/*.c))
RENDER_SRC := src/render/mfacet.c src/render/vector.c
PROOF_SRC := $(wildcard src/proof/*.c) $(RENDER_SRC) $(TIF_SRC) $(IO_SRC)
PROOF_OBJ := $(PROOF_SRC:.c=.o)

INCLUDES := -I./src/render -I./src/io -I./src/tif

CFLAGS := -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast
CC := gcc -g -ansi $(CFLAGS) $(INCLUDES)

LDLIBS := -lm
LDFLAGS :=

all: proof

%.o: %.cpp
	$(CC) -c $< -o $@

proof:  $(PROOF_OBJ)
	$(CC) $^ -o $@ $(LDLIBS) $(LDFLAGS)

clean:
	@rm -rf *.o

distclean: clean
	@rm -f proof

wc:
	@echo "Counting lines..."
	@find src -name "*.[ch]" -exec cat {} \; | wc -l

