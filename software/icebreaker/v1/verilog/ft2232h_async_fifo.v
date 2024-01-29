module ft2232h_async_fifo (
    inout [7:0] D,
    input RXFn,
    input TXEn,
    output reg RDn,
    output reg WRn,

    input clk_48mhz,reset,
    input rw_clk,
    input rd_en,
    output [7:0] rd_data,
    output rd_empty,
    input wr_en,
    input [7:0] wr_data,
    output wr_full
    );
    wire read_full;
    reg read_wr_en;
        async_fifo read_fifo
                    (.reset(reset),
                     .rd_clk(rw_clk), .rd_en(rd_en), .rd_data(rd_data),
                     .wr_clk(clk_48mhz), .wr_en(read_wr_en), .wr_data(D),
                     .full(read_full), .empty(rd_empty));

    wire write_empty;
    reg write_rd_en;
    wire [7:0] D_out;
        async_fifo write_fifo
                    (.reset(1'b0),
                     .rd_clk(clk_48mhz), .rd_en(write_rd_en), .rd_data(D_out),
                     .wr_clk(rw_clk), .wr_en(wr_en), .wr_data(wr_data),
                     .full(wr_full), .empty(write_empty));

    assign D=(WR_delay || !WRn)?D_out:8'hz;

    reg WR, WR_delay;
    always @(posedge clk_48mhz)
		if(reset) WR_delay<=0;
		else WR_delay<=WR;
    always @(negedge clk_48mhz)
		if(reset) WRn<=0;
		else WRn<=!WR_delay;

    wire RXFn_sync, TXEn_sync;
	reg  [1:0] RXFn_r, TXEn_r;
	always @(posedge clk_48mhz)begin
		if(reset)begin
			RXFn_r <= 2'b11;
			TXEn_r <= 2'b11;
		end else begin
			RXFn_r <= {RXFn_r[0],RXFn};
			TXEn_r <= {TXEn_r[0],TXEn};
		end
	end
	assign RXFn_sync = RXFn_r[1];
	assign TXEn_sync = TXEn_r[1];
	
    reg [1:0] wait_clocks;

    localparam STATE_IDLE=2'h0, STATE_READ=2'h1, STATE_WRITE=2'h2, STATE_COMPLETE=2'h3;
    reg [1:0] state;
    always @(posedge clk_48mhz)
    begin
		if(reset)begin
			state       <= STATE_IDLE;
			RDn         <= 1'b1;
			WR          <= 1'b0;
			write_rd_en <= 1'b0;
			read_wr_en  <= 1'b0;
			wait_clocks <= 2'd0;
		end else case(state)
        STATE_IDLE:
        begin
            if((!read_full)&&(!RXFn_sync))
            begin
                if(WRn)
                begin
                    state       <= STATE_READ;
                    RDn         <= 1'b0;
                    write_rd_en <= 1'b0;
                end
            end
            else if((!write_empty)&&(!TXEn_sync))
            begin
                state       <= STATE_WRITE;
                RDn         <= 1'b1;
                write_rd_en <= 1'b1;
                WR          <= 1'b1;
            end
            else
            begin
                RDn         <= 1'b1;
                write_rd_en <= 1'b0;
            end
        end
        STATE_READ:
        begin
            read_wr_en  <= 1'b1;
            wait_clocks <= 2'd2;
            state       <= STATE_COMPLETE;
        end
        STATE_WRITE:
        begin
            write_rd_en <= 1'b0;
            wait_clocks <= 2'd2;
            state       <= STATE_COMPLETE;
        end
        STATE_COMPLETE:
        begin
            WR          <= 1'b0;
            RDn         <= 1'b1;
            read_wr_en  <= 1'b0;
            if(wait_clocks)
                wait_clocks <= wait_clocks - 1'b1;
            else
                state       <= STATE_IDLE;
        end
        endcase
    end

endmodule

