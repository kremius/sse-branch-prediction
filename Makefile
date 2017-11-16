compiler = g++-7
cpp_flags = -march=native -O3 -std=c++17

variants = serial serial_branchless serial_branch_by_code serial_div_switch

all: $(variants)

build:
	mkdir -p build

$(variants): build
	$(compiler) $(cpp_flags) src/main.cpp src/$@_processing.cpp -o build/$@
	$(compiler) -S $(cpp_flags) src/$@_processing.cpp -o build/$@.s

