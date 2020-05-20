# Utilities

- send_file.ps1: A Powershell script to send a file to the BSX monitor program when in load (L) mode

#### send_file.ps1
Modify the following values:
- filePath: Path to the binary file to upload to the BSX
- startAddress: The start address (in RAM) to upload to, between 0x8000 and 0xFFFF
- comPort: The com port that your STM32 appears as when connected to your PC

Make sure that the Z80 monitor program is running - suggest a Z80 reset before you start upload
You will need to close any other applications using the COM port, i.e. Putty
- Start the file upload by running the script
- Once the upload is complete, the BSX will display Ready on the LCD
- Open Putty, then enter J<startAddress> (i.e. J9000) to run the uploaded file
