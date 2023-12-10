#!/bin/bash
OUT_DIR=out

COMPILER_ARGS="$(root-config --cflags --libs) -Wall -Wextra"

SRC_FILES="\
	src/particle_type.cpp \
	src/resonance_type.cpp \
	src/particle.cpp"
SIMULATION=src/simulation.cpp
ANALISIS=src/analisis.cpp
TEST=src/test.cpp

TEST_BIN=$OUT_DIR/test
SIMULATION_BIN=$OUT_DIR/simulation
ANALISIS_BIN=$OUT_DIR/analisis

function build-simulation() {
	mkdir $OUT_DIR
	g++ -o $SIMULATION_BIN $SRC_FILES $SIMULATION $COMPILER_ARGS
}

function build-analisis() {
	mkdir $OUT_DIR
	g++ -o $ANALISIS_BIN $SRC_FILES $ANALISIS $COMPILER_ARGS
}

function build-test() {
	mkdir $OUT_DIR
	g++ -o $TEST_BIN $SRC_FILES $TEST $COMPILER_ARGS
}

function help() {
	echo "Synthax: ./build.sh <analisis|simulation|test|build-analisis|build-simulation|build-test>"
	echo ""
	echo analisis - Build and run analisis
	echo build-analisis - Build analisis
	echo simulation - Build and run simulation
	echo build-simulation - Build main program
	echo test - Build and run tests
	echo build-test - Build tests
}

if [ "$1" == "analisis" ] || [ $# -eq 0 ]
then
	$(build-analisis) \
	&& ./${ANALISIS_BIN}
elif [ "$1" == "build-analisis" ]
then
	$(build-analisis)
elif [ "$1" == "simulation" ]
then
	$(build-simulation) \
	&& ./${SIMULATION_BIN}
elif [ "$1" == "build-simulation" ]
then
	$(build-simulation)
elif [ "$1" == "test" ]
then
	$(build-test) \
	&& ./${TEST_BIN}
elif [ "$1" == "build-test" ]
then
	$(build-test)
else
	$(help)
fi