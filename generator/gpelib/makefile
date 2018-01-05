IDIR=./include
CC=g++
CFLAGS=-I$(IDIR) -fPIC
SRC=./src
ODIR=./obj
LIB=./lib

_OBJ=FCS_HASH.o       \
     GPE.o            \
     GPE_rnd.o        \
     T3EncodeDecode.o \
     
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SRC)/%.cpp
	$(CC) -fPIC -c -o $@ $< $(CFLAGS)

$(LIB)/libgpe.a: $(OBJ)
	ar rcs $@ $^
    
.PHONY: clean

clean:
	rm -f $(ODIR)/*.o 



