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
ANALYSIS=src/analysis.cpp
TEST=src/test.cpp

TEST_BIN=$OUT_DIR/test
SIMULATION_BIN=$OUT_DIR/simulation
ANALYSIS_BIN=$OUT_DIR/analysis

build_simulation() {
	g++ -o $SIMULATION_BIN $SRC_FILES $SIMULATION $COMPILER_ARGS
}

build_analysis() {
	g++ -o $ANALYSIS_BIN $SRC_FILES $ANALYSIS $COMPILER_ARGS
}

build_test() {
	g++ -o $TEST_BIN $SRC_FILES $TEST $COMPILER_ARGS
}

simulation() {
	$(build_simulation) && ./${SIMULATION_BIN}
}

analysis() {
	$(build_analysis) && ./${ANALYSIS_BIN}
}

test() {
	$(build_test) && ./${TEST_BIN}
}

print_help() {
	echo 'Synthax: ./build.sh [analysis|simulation|test|build_analysis|build_simulation|build_test]'
	echo ''
	echo '*no argumets* - Build and run simulation and analysis'
	echo 'analysis - Build and run analysis'
	echo 'build_analysis - Build analysis'
	echo 'simulation - Build and run simulation'
	echo 'build_simulation - Build main program'
	echo 'test - Build and run tests'
	echo 'build_test - Build tests'
}

# Make sure out directory exists
if ! [ -d $OUT_DIR ]; then
	mkdir $OUT_DIR && echo "\"$OUT_DIR\" directory created"
fi

# Executes command
if [ $# -eq 0 ]; then
	simulation && analysis
elif [ "$1" == "analysis" ] || [ $# -eq 0 ]; then
	analysis
elif [ "$1" == "build_analysis" ]; then
	build_analysis
elif [ "$1" == "simulation" ]; then
	simulation
elif [ "$1" == "build_simulation" ]; then
	build_simulation
elif [ "$1" == "test" ]; then
	test
elif [ "$1" == "build_test" ]; then
	build_test
else
	print_help
fi