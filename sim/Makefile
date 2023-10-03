VERILATOR = verilator

LDFLAGS := -LDFLAGS "$(shell sdl2-config --libs)"
CFLAGS := -CFLAGS "$(shell sdl2-config --cflags) -g"

RTLDIR := ../src
SRC := sim_main.cpp

RTL_SRC :=	\
	bbox_iterator.v \
	clk_divider.v \
	clut_ram.v \
	div_uu.v \
	edge_function_evaluator.v \
	edge_function.v \
	GPU.v \
	huedeon_ctrl.v \
	lfsr5.v \
	raster_pipeline_ex1.v \
	raster_pipeline_ex2.v \
	raster_pipeline_ex3.v \
	texture_ram.v \
	texture_unit.v \
	tri_raster_engine.v \
	vga_encoder.v \
	vga_framebuffer.v \
	vram.v

all: sim

clean:
	rm -rf obj_dir data

sim: top.sv $(SRC)
	$(VERILATOR) --trace -cc --exe $(CFLAGS) $(LDFLAGS) --top-module top --no-timing -Wno-CASEINCOMPLETE -Wno-UNSIGNED -Wno-WIDTHEXPAND -Wno-WIDTHTRUNC -Wno-PINMISSING -Wno-ASCRANGE -Wno-WIDTHCONCAT -Wno-MULTIDRIVEN -Wno-INITIALDLY top.sv $(SRC) $(RTL_SRC) -I$(RTLDIR) -I$(RTLDIR)/riscado-v
	$(MAKE) -j 4 -C obj_dir -f Vtop.mk

run: sim
	cp -r ../src/data .
	obj_dir/Vtop

.PHONY: all clean
