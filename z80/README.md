# Z80
The Z80 code for the BSX Breadboard Computer including:

- monitor.z80: A simple machine code monitor to load into ROM
- BBC Basic: Experimental - The BBC Basic Interpreter for Z80

#### BBC Basic
This is provided under a zlib license - see the README in the folder for more details

#### Monitor
The monitor program provides the following functionality:
Mnnnn,llll - Memory Hex Dump: Output llll bytes from memory location nnnn to port 0
Jnnnn - Jump to location nnnn
L - Put the monitor into Load mode; it will wait for a binary stream of data on port 0

#### Load data format
When in Load mode, send bytes of data to load into RAM to the STM32 down the serial port in the
following format:
0x4C, 0x0D, Address(L), Address(H), Length(L), Length(H), Data1, Data2, ...., DataN

Where:
- Address is the location in RAM to load the data into
- Length is the length of the data stream
- Data is the stream of bytes to load sequentially in memory
