#!/bin/bash

# Check if root ang g++ are installed
command -v root-config > /dev/null 2>&1 || { echo >&2 "The command root-config (root utility) could not be found"; exit 1; }
command -v g++ > /dev/null 2>&1 || { echo >&2 "The command g++ (c++ compiler) could not be found"; exit 1; }

OUT_DIR=out

COMPILER_ARGS="$(root-config --cflags --libs) -Wall -Wextra -std=c++17"

SRC_FILES="\
	src/particle_type.cpp \
	src/resonance_type.cpp \
	src/util.cpp \
	src/particle.cpp"
SIMULATION=src/simulation.cpp
ANALISIS=src/analisis.cpp
TEST=src/test.cpp

TEST_BIN=$OUT_DIR/test
SIMULATION_BIN=$OUT_DIR/simulation
ANALISIS_BIN=$OUT_DIR/analisis

function build-simulation() {
	g++ -o $SIMULATION_BIN $SRC_FILES $SIMULATION $COMPILER_ARGS
}

function build-analisis() {
	g++ -o $ANALISIS_BIN $SRC_FILES $ANALISIS $COMPILER_ARGS
}

function build-test() {
	g++ -o $TEST_BIN $SRC_FILES $TEST $COMPILER_ARGS
}

function print-help() {
	echo "Synthax: ./build.sh <analisis|simulation|test|build-analisis|build-simulation|build-test>"
	echo ""
	echo analisis - Build and run analisis
	echo build-analisis - Build analisis
	echo simulation - Build and run simulation
	echo build-simulation - Build main program
	echo test - Build and run tests
	echo build-test - Build tests
}

# Make sure out directory exists
if ! [ -d $OUT_DIR ]; then
	mkdir $OUT_DIR && echo "\"$OUT_DIR\" directory created"
fi

# Executes command
if [ "$1" == "analisis" ] || [ $# -eq 0 ]; then
	$(build-analisis) \
	&& ./${ANALISIS_BIN}
elif [ "$1" == "build-analisis" ]; then
	$(build-analisis)
elif [ "$1" == "simulation" ]; then
	$(build-simulation) \
	&& ./${SIMULATION_BIN}
elif [ "$1" == "build-simulation" ]; then
	$(build-simulation)
elif [ "$1" == "test" ]; then
	$(build-test) \
	&& ./${TEST_BIN}
elif [ "$1" == "build-test" ]; then
	$(build-test)
else
	print-help
fi