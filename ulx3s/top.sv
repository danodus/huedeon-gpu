module top(
    input  wire logic       clk_25mhz,

    output      logic [3:0] gpdi_dp,
    output      logic [3:0] gpdi_dn,

    input  wire logic [6:0] btn,
    output      logic [7:0] led
);

    logic clk_pix, clk_pix_x5, clk_main;

    pll_main pll_main(
        .clkin(clk_25mhz),
        .locked(),
        .clkout0(clk_main),
    );

    pll_video pll_video(
        .clkin(clk_25mhz),
        .locked(),
        .clkout0(clk_pix_x5),
        .clkout1(clk_pix)
    );

    logic [7:0] vga_r;                      // vga red (8-bit)
    logic [7:0] vga_g;                      // vga green (8-bits)
    logic [7:0] vga_b;                      // vga blue (8-bits)
    logic       vga_hsync;                  // vga hsync
    logic       vga_vsync;                  // vga vsync
    logic       vga_de;                     // vga data enable


    hdmi_encoder hdmi(
        .pixel_clk(clk_pix),
        .pixel_clk_x5(clk_pix_x5),

        .red(vga_r),
        .green(vga_g),
        .blue(vga_b),

        .vde(vga_de),
        .hsync(vga_hsync),
        .vsync(vga_vsync),

        .gpdi_dp(gpdi_dp),
        .gpdi_dn(gpdi_dn)
    );

    RISCV_SOC riscv_soc(
        .CLOCK_50(clk_main),
        .CLOCK_PIX(clk_pix),
        .KEY(btn),
        .SW(),
        .LEDR(led),
        .HEX0(),
        .HEX1(),
        .VGA_B(vga_b),
        .VGA_BLANK_N(vga_de),
        .VGA_G(vga_g),
        .VGA_HS(vga_hsync),
        .VGA_R(vga_r),
        .VGA_SYNC_N(),
        .VGA_VS(vga_vsync)
    );

endmodule