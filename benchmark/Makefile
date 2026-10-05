NAME = rgb_benchmark

SRCS = main_benchmark.cpp \
       ../test/reference/source/rgb_to_gray.cpp \
       ../lib/source/rgb_to_gray_vector.cpp \
       ../../../gem5/util/m5/src/abi/riscv/m5op.S

INCLUDES = -I$(abspath ../test/reference/include) \
           -I$(abspath ../lib/include) \
           -I$(abspath ../../../gem5/include)

CXXFLAGS += -DUSE_GEM5_M5 -DBENCHMARK -O3 $(INCLUDES)
CFLAGS   += -DUSE_GEM5_M5 -DBENCHMARK -O3 $(INCLUDES)

include $(AM_HOME)/Makefile.app

run_gem5:
	@echo "=== Building Project ==="
	$(MAKE) clean
	$(MAKE) ARCH=riscv64-xs LINUX_GNU_TOOLCHAIN=1 MARCH=rv64gcv
	@echo "=== Running gem5 ==="
	cd ~/gem5 && ./build/RISCV/gem5.opt \
		-d "m5out_rgb_benchmark" \
		./configs/example/kmhv3.py \
		--mem-size=2GB \
		--enable-riscv-vector \
		--disable-difftest \
		--raw-cpt \
		--generic-rv-cpt=$(HOME)/projects/RISCV_Academy/benchmark/build/rgb_benchmark-riscv64-xs.bin