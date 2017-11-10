all: serial serial_branchless

compiler = g++-7
cpp_flags = -march=native -O3 -std=c++17

build:
	mkdir -p build

serial: build
	$(compiler) $(cpp_flags) main.cpp serial_processing.cpp -o build/serial
	$(compiler) -S $(cpp_flags) serial_processing.cpp -o build/serial.s

serial_branchless: build
	$(compiler) $(cpp_flags) main.cpp serial_branchless_processing.cpp -o build/serial_branchless
	$(compiler) -S $(cpp_flags) serial_branchless_processing.cpp -o build/serial_branchless.s
