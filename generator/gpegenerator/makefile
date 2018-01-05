IDIR=./include
CC=g++
CFLAGS=-I$(IDIR) -I../aeslib/include -I../gpelib/include
SRC=./src
ODIR=./obj
BIN=./bin
LIBPATH=-L../aeslib/lib/ -L../qrlib/lib/ -L ../gpelib/lib/ 
LIBS=-lgpe -laes -lqr -lopencv_imgcodecs -lopencv_imgproc -lopencv_core 
#-lIlmImf -ltiff -ljasper -lpng -ljpeg -lwebp -lzlib -llzma -lgomp -lopengl32

_OBJ=gpegenerator.o BMP.o
     
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SRC)/%.cpp
	$(CC) -s -c -fPIC -o $@ $< $(CFLAGS)

$(BIN)/libgpegenerator.so: $(OBJ)
	g++ -fPIC -s -shared -o $@ $^ $(CFLAGS) $(LIBPATH) $(LIBS)
    
.PHONY: clean

clean:
	rm -f $(ODIR)/*.o 



