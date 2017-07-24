all : libLzma.so libLzma.a

SRC = 7zAlloc.c 7zArcIn.c 7zBuf2.c 7zBuf.c 7zCrc.c 7zCrcOpt.c 7zDec.c 7zFile.c 7zStream.c Aes.c AesOpt.c Alloc.c Bcj2.c Blake2s.c Bra86.c Bra.c BraIA64.c BwtSort.c CpuArch.c Delta.c DllSecur.c \
      HuffEnc.c LzFind.c Lzma2Dec.c Lzma2Enc.c Lzma86Dec.c Lzma86Enc.c LzmaDec.c LzmaEnc.c LzmaLib.c Ppmd7.c Ppmd7Dec.c Ppmd7Enc.c Ppmd8.c Ppmd8Dec.c Ppmd8Enc.c Sha1.c \
      Sha256.c Sort.c Xz.c XzCrc64.c XzCrc64Opt.c XzDec.c XzEnc.c XzIn.c

TGT = $(SRC:.c=.o)

CL++ = g++

CPP_FLAGS = -fPIC -O2 -Wall -D_7ZIP_ST

LINK_FLAGS = -shared

libLzma.so : $(TGT)
	$(CL++) $(LINK_FLAGS) -o libLzma.so $(TGT)

libLzma.a : $(TGT)
	ar -r libLzma.a $(TGT)

%.o:%.c
	$(CL++) $(CPP_FLAGS) -c $< -o $@

clean:
	-rm *.o libLzma.so libLzma.a
