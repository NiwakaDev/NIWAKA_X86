.PHONY:clean build all

EMULATOR = x86

CC = g++
G++_OPTIONS = -Wall -std=c++11 -g -I ${INCLUDE_DIR} -O0
G++_OPTIONS += -lSDL2

SOURCE_DIR      = ./sources/
OUTPUTS_DIR     = ./outputs/
TEST_ASM_DIR        = ./test_asm/
INCLUDE_DIR     = ./include/
vpath %.h ${INCLUDE_DIR}
vpath %.cpp ${SOURCE_DIR}

HEADERS = $(wildcard *.h)
SOURCES = $(wildcard ${SOURCE_DIR}*.cpp)
OBJECTS = ${SOURCES:.cpp=.o}

all:${EMULATOR}

%.o : %.cpp ${HEADERS}
	${CC} ${G++_OPTIONS} -o $@ -c $<

${EMULATOR} : ${OBJECTS} 
	${CC} ${LD_FLGS} ${G++_OPTIONS} -o ${OUTPUTS_DIR}${EMULATOR} $^

#機械語チェック用
helloworld : ${TEST_ASM_DIR}helloworld.asm
	nasm -o ${OUTPUTS_DIR}helloworld ${TEST_ASM_DIR}helloworld.asm

clean :
	rm ${OUTPUTS_DIR}${EMULATOR}
	rm ${SOURCE_DIR}*.o