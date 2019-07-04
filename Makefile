all: genMatrix genLdpcDecoder rsenc bfenc raw2bin ldpcenc scrambler ber hamenc PRNGenc asymenc

clean:
	rm rsenc rsdec bfenc bfdec raw2bin bin2raw ldpcenc ldpcdec genMatrix scrambler descramble ber hamenc hamdec PRNGenc PRNGdec asymenc asymdec genLdpcDecoder

CFLAGS= -Wall -O3 -march=native
#CFLAGS= -Wall -O0 -g -march=native

genMatrix: genMatrix.cpp
	g++ $(CFLAGS) -o genMatrix genMatrix.cpp

rsenc: rsenc.c reedsolomon.c
	gcc $(CFLAGS) -o rsenc rsenc.c reedsolomon.c
	ln -s rsenc rsdec

bfenc: bfenc.c 
	g++ $(CFLAGS) -ansi -o bfenc bfenc.c
	ln -s bfenc bfdec

PRNGenc: PRNGenc.c 
	g++ $(CFLAGS) -o PRNGenc PRNGenc.c
	ln -s PRNGenc PRNGdec

raw2bin: raw2bin.c
	gcc $(CFLAGS) -o raw2bin raw2bin.c
	ln -s raw2bin bin2raw

genLdpcDecode: genLdpcDecoder.cpp
	g++ $(CFLAGS) -o genLdpcDecoder genLdpcDecoder.cpp

ldpcenc: ldpcenc.cpp
	g++ $(CFLAGS) -o ldpcenc ldpcenc.cpp
	ln -s ldpcenc ldpcdec

scrambler: scrambler.c
	g++ $(CFLAGS) -o scrambler scrambler.c
	ln -s scrambler descramble

ber: ber.c
	gcc $(CFLAGS) -o ber ber.c

hamenc: hamenc.cpp
	g++ $(CFLAGS) -o hamenc hamenc.cpp
	ln -s hamenc hamdec

asymenc: asymcode3.cpp
	g++ $(CFLAGS) -o asymenc asymcode3.cpp
	ln -s asymenc asymdec

