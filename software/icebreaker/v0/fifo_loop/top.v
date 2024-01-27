
module top(
    inout  [7:0] D,
    input        RXFn,
    input        TXEn,
    output       RDn,
    output       WRn,
	output reg   adc_clk_out
);

//clock
wire clk_48,clk;
	SB_HFOSC u_hfosc (
		.CLKHFPU(1'b1),
		.CLKHFEN(1'b1),
		.CLKHF(clk_48)
	);
always@(posedge clk_48) adc_clk_out = ~adc_clk_out;//24MHz
//assign adc_clk_out = clk_48;//DCO
//I got this this error on adc_clk_out pin "DCO", when i tried to drive 48MHz clock.
//ERROR: Max frequency for clock 'adc_clk_out$SB_IO_OUT': 38.32 MHz (FAIL at 48.00 MHz)
//assign clk = adc_clk_in;
//assign clk = clk_48;
assign clk = adc_clk_out;

//reset
reg [5:0] reset_cnt = 0;
reg rst = 1;
always @(posedge clk) begin
    if (reset_cnt < 63) begin
        rst       <= 1;
        reset_cnt <= reset_cnt + 1;
    end else begin
        rst       <= 0;
    end
end

//internal signals
reg         rd_en;
wire [7:0]  rd_data;
reg  [7:0]  cmd; //command input from FTDI
wire        rd_empty;
reg         wr_en;
reg  [7:0]  wr_data;
wire        wr_full;

reg  [2:0]  state;

localparam IDLE        = 3'd0;
localparam RD_CMD      = 3'd1;
localparam DECODE      = 3'd2;
localparam LOOPBACK    = 3'd3;

//FSM
always@(posedge clk)begin
	if(rst)begin
		state   <= IDLE;
		rd_en   <= 0;
		cmd     <= 0;
		wr_en   <= 0;
		wr_data <= 0;
	end else begin
		case(state)
			IDLE    : begin
				if(rd_empty==0)begin
					state <= RD_CMD;
					rd_en <= 1;
					wr_en <= 0;
				end else begin
					state <= IDLE;
					rd_en <= 0;
					wr_en <= 0;
				end
			end
			RD_CMD  : begin
					state <= DECODE;
					rd_en <= 0;
					wr_en <= 0;
					cmd   <= rd_data;
			end
			DECODE  : begin
				case(cmd[7:4])
				7 : state <= LOOPBACK;
				default : state <= IDLE;
				endcase
			end
			LOOPBACK: begin
				if(wr_full==0)begin
					wr_en   <= 1;
					wr_data <= cmd;
					state   <= IDLE;
				end
			end
			default : begin
				state     <= IDLE;
			end
		endcase
	end
end

ft2232h_async_fifo ft_fifo(
.D(D),
.RXFn(RXFn),
.TXEn(TXEn),
.RDn(RDn),
.WRn(WRn),
.clk_48mhz(clk),
.reset(rst),
.rw_clk(clk),
.rd_en(rd_en),
.rd_data(rd_data),
.rd_empty(rd_empty),
.wr_en(wr_en),
.wr_data(wr_data),
.wr_full(wr_full)
);

endmodule
