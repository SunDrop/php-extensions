IDIR=./include
CC=g++
CFLAGS=-I$(IDIR) -fPIC
SRC=./src
ODIR=./obj
LIB=./lib

_OBJ=BCH_Reed_Solomon.o CV_QRCode.o QRCode_Bitmatrix.o
     
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SRC)/%.cpp
	$(CC) -fPIC -c -o $@ $< $(CFLAGS)

$(LIB)/libqr.a: $(OBJ)
	ar rcs $@ $^
    
.PHONY: clean

clean:
	rm -f $(ODIR)/*.o 



