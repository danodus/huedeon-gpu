YOSYS := yosys
ECPPACK := ecppack
NEXTPNR := nextpnr-ecp5
FUJPROG := fujprog
OPENFPGALOADER := openFPGALoader

DEVICE := 85k
PACKAGE := CABGA381

PIN_DEF := ulx3s.lpf
SRC :=	\
	../src/bbox_iterator.v \
	../src/clut_ram.v \
	../src/div_uu.v \
	../src/edge_function_evaluator.v \
	../src/edge_function.v \
	../src/GPU.v \
	../src/huedeon_ctrl.v \
	../src/lfsr5.v \
	../src/pipeline_ctrl.v \
	../src/raster_pipeline_ex1.v \
	../src/raster_pipeline_ex2.v \
	../src/raster_pipeline_ex3.v \
	../src/texture_ram.v \
	../src/texture_unit.v \
	../src/tri_raster_engine.v \
	../src/vga_encoder.v \
	../src/vga_framebuffer.v \
	../src/vram.v \
	../src/RISCV_SOC.v \
	../src/riscado-v/alu.v \
	../src/riscado-v/control_unit.v \
	../src/riscado-v/load_store.v \
	../src/riscado-v/program_counter.v \
	../src/riscado-v/ram.v \
	../src/riscado-v/register_file.v \
	../src/riscado-v/riscv.v \
	../src/riscado-v/rom.v \
	hdmi_encoder.v \
	pll_main.v \
	pll_video.v \
	top.sv

all: top.bin

clean:
	rm -rf data
	rm -f *.hex *.asc *.json *.bin *.log

top.json: $(SRC)
	cp -r ../src/data .
	cp ../src/riscado-v/gcc/rom.mem .
	$(YOSYS) -ql top.log -p 'synth_ecp5 -top top -json top.json' $(SRC)

top.asc: top.json $(PIN_DEF)
	$(NEXTPNR) -l top_nextpnr.log --$(DEVICE) --package $(PACKAGE) --json top.json --lpf $(PIN_DEF) --textcfg top.asc --randomize-seed

top.bin: top.asc
	$(ECPPACK) --compress --input top.asc --bit top.bin

prog: top.bin
	$(FUJPROG) -Tbit top.bin

.PHONY: all prog
