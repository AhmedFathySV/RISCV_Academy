RISCV_TOOLCHAIN_PREFIX ?= riscv64-linux-gnu-
RISCV_CXX ?= $(RISCV_TOOLCHAIN_PREFIX)g++
RISCV_MARCH ?= rv64gcv_zba_zbb_zbc_zbs_zbkb_zbkc_zbkx_zknd_zkne_zknh_zkr_zksed_zksh_zkt
RISCV_MABI ?= lp64d
RISCV_OPT_FLAGS ?= -O3 -DNDEBUG

HISTOGRAM_BINARY := build/histogram-riscv
GEM5_OUTDIR := build/m5out
RUNNER := $(CURDIR)/run_histogram.py
PROJECT_ROOT := $(abspath ../..)
GEM5_HOME ?= $(XIANG_GEM5)
export GEM5_HOME

RISCV_FLAGS := -std=c++20 -march=$(RISCV_MARCH) -mabi=$(RISCV_MABI) \
	-mcmodel=medany -fno-pic -static -fdata-sections -ffunction-sections

.PHONY: all build check-toolchain run run-xs clean help

all: build

build: check-toolchain $(HISTOGRAM_BINARY)

check-toolchain:
	@command -v "$(RISCV_CXX)" >/dev/null 2>&1 || { \
		printf 'RISC-V compiler not found: %s\n' "$(RISCV_CXX)"; \
		exit 1; \
	}

$(HISTOGRAM_BINARY): histogram.cpp | build-dir
	$(RISCV_CXX) $(RISCV_FLAGS) $(RISCV_OPT_FLAGS) $< -o $@

build-dir:
	@mkdir -p build

run: build
	python3 "$(RUNNER)" \
		--binary "$(abspath $(HISTOGRAM_BINARY))" \
		--outdir "$(abspath $(GEM5_OUTDIR))"

run-xs:
	cmake -S "$(PROJECT_ROOT)" -B "$(PROJECT_ROOT)/build" \
		-DCMAKE_TOOLCHAIN_FILE="$(PROJECT_ROOT)/cmake/riscv64_xs_toolchain.cmake"
	cmake --build "$(PROJECT_ROOT)/build" --target run_Histogram_benchmark_gem5

clean:
	rm -rf build

help:
	@printf '%s\n' \
		'build: compile histogram.cpp to a static RISC-V Linux ELF' \
		'run:   build, then run the ELF with the Xiangshan gem5 SE config' \
		'run-xs: build and run the RVV bare-metal benchmark with XS-GEM5' \
		'clean: remove this benchmark build directory and gem5 output files' \
		'' \
		'Set GEM5_HOME or XIANG_GEM5 to the XiangShan gem5 checkout when the' \
		'RISCV_Academy CMake build cache is not available.'
