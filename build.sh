#!/bin/bash
OUT_DIR=out

COMPILER_ARGS="$(root-config --cflags --libs) -Wall -Wextra"

SRC_FILES="\
	src/particle_type.cpp \
	src/resonance_type.cpp \
	src/particle.cpp"
MAIN=src/main.cpp
TEST=src/test.cpp

TEST_BIN=$OUT_DIR/test
TARGET_BIN=$OUT_DIR/main


if [ "$1" == "run" ] || [ $# -eq 0 ]
then
	g++ -o $TARGET_BIN $SRC_FILES $MAIN $COMPILER_ARGS \
	&& ./${TARGET_BIN}
elif [ "$1" == "build" ]
then
	g++ -o $TARGET_BIN $SRC_FILES $MAIN $COMPILER_ARGS
elif [ "$1" == "test" ]
then
	g++ -o $TEST_BIN $SRC_FILES $TEST $COMPILER_ARGS \
	&& ./${TEST_BIN}
elif [ "$1" == "build-test" ]
then
	g++ -o $TEST_BIN $SRC_FILES $TEST $COMPILER_ARGS
else
	echo "Synthax: ./build.sh <run|test|build|build-test>"
	echo ""
	echo run - Build and run main program
	echo build - Build main program
	echo test - Build and run tests
	echo build-test - Build tests
fi