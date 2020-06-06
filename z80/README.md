# Z80
The Z80 code for the BSX Breadboard Computer including:

- Monitor: A simple machine code monitor to load into ROM
- BBC Basic: The BBC Basic Interpreter for Z80 (Experimental)

#### BBC Basic
This is provided under a zlib license - see the README in the folder for more details

#### Monitor
The monitor program provides the following functionality:

- Mnnnn,llll - Memory Hex Dump: Output llll bytes from memory location nnnn 
- Jnnnn - Jump to location nnnn
- Onn,vv - O(utput) the value vv on Z80 port nn
- Inn,llll - I(nput) llll values from Z80 port nn
- L - Put the monitor into Load mode; it will wait for a binary stream of data on port 0
- B - Jump to address 0x4000 (where BBC Basic can be loaded)
- Dnnnn,llll - Disassemble llll bytes from memory location nnnn

#### Load data format
When in Load mode, send bytes of data to load into RAM to the STM32 down the serial port in the
following format:
0x4C, 0x0D, Address(L), Address(H), Length(L), Length(H), Data1, Data2, ...., DataN

Where:
- Address is the location in RAM to load the data into
- Length is the length of the data stream
- Data is the stream of bytes to load sequentially in memory

There is an example Powershell file in the /utilities folder (send_file.ps1) that will do this
