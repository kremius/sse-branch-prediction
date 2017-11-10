all: serial serial_branchless

cpp_flags = -O3 -std=c++1z

build:
	mkdir -p build

serial: build
	g++ $(cpp_flags) main.cpp serial_processing.cpp -o build/serial

serial_branchless: build
	g++ $(cpp_flags) main.cpp serial_branchless_processing.cpp -o build/serial_branchless
