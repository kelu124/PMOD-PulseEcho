module async_fifo (
    input reset,
    input rd_clk,
    input rd_en,
    output reg [8-1:0] rd_data,
    input wr_clk,
    input wr_en,
    input [8-1:0] wr_data,
    output full,
    output empty
    );

    reg [8-1:0] memory[8-1:0];

    reg [3:0] rd_head, wr_head;
    wire [3:0] rd_head_gray, wr_head_gray;
    assign rd_head_gray=binary_to_gray(rd_head);
    assign wr_head_gray=binary_to_gray(wr_head);
    reg  [3:0] rd_head_sync_gray, wr_head_sync_gray;
    reg  [3:0] rd_head_gray_r0, wr_head_gray_r0;
    reg  [3:0] rd_head_gray_r1, wr_head_gray_r1;
	always@(posedge wr_clk)begin
		if(reset)begin
			rd_head_gray_r0   <= 4'd0;
			rd_head_gray_r1   <= 4'd0;
			rd_head_sync_gray <= 4'd0;
		end else begin
			rd_head_gray_r0   <= rd_head_gray;
			rd_head_gray_r1   <= rd_head_gray_r0;
			rd_head_sync_gray <= rd_head_gray_r1;
		end
	end
	always@(posedge rd_clk)begin
		if(reset)begin
			wr_head_gray_r0   <= 4'd0;
			wr_head_gray_r1   <= 4'd0;
			wr_head_sync_gray <= 4'd0;
		end else begin
			wr_head_gray_r0   <= wr_head_gray;
			wr_head_gray_r1   <= wr_head_gray_r0;
			wr_head_sync_gray <= wr_head_gray_r1;
		end
	end

    wire [3:0] rd_head_sync, wr_head_sync;
    assign rd_head_sync=gray_to_binary(rd_head_sync_gray);
    assign wr_head_sync=gray_to_binary(wr_head_sync_gray);

    assign empty=(rd_head==wr_head_sync);
    assign full=((rd_head_sync[3-1:0]==wr_head[3-1:0])&&(rd_head_sync[3]!=wr_head[3]));

    always @(posedge wr_clk)
    begin
        if(reset)
            wr_head<=0;
        else if(wr_en && !full)
        begin
            memory[wr_head[3-1:0]]<=wr_data;
            wr_head<=wr_head+1'b1;
        end
    end

    always @(posedge rd_clk)
    begin
        if(reset)
            rd_head<=0;
        else if(rd_en && !empty)
        begin
            rd_data<=memory[rd_head[3-1:0]];
            rd_head<=rd_head+1'b1;
        end
    end

    function [3:0] binary_to_gray(input [3:0] binary);
        integer i;
        begin
            for(i=0; i<3; i=i+1)
                binary_to_gray[i] = binary[i+1] ^ binary[i];
            binary_to_gray[3] = binary[3];
        end
    endfunction

    function [3:0] gray_to_binary(input [3:0] gray);
        integer i;
        begin
            gray_to_binary[3] = gray[3];
            for(i=(3-1); i>=0; i=i-1)
                gray_to_binary[i] = gray_to_binary[i+1] ^ gray[i];
        end
    endfunction

endmodule

