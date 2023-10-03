module top(
    input wire logic   CLOCK_50,
    input wire logic [3:0] KEY,
    output logic [7:0] VGA_B,
    output logic       VGA_BLANK_N,
    output logic       VGA_CLK,
    output logic [7:0] VGA_G,
    output logic       VGA_HS,
    output logic [7:0] VGA_R,
    output logic       VGA_SYNC_N,
    output logic       VGA_VS 
);

    GPU gpu(
        .CLOCK_50,
        .HEX0(),
        .HEX1(),
        .KEY(KEY),
        .LEDR(),
        .VGA_B,
        .VGA_BLANK_N,
        .VGA_CLK,
        .VGA_G,
        .VGA_HS,
        .VGA_R,
        .VGA_SYNC_N,
        .VGA_VS
    );

endmodule
