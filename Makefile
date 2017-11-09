all: serial

serial:
	mkdir -p build
	g++ -O3 -std=c++1z main.cpp serial_processing.cpp -o build/serial
