# BBC Basic
The code, as provided by David Given (dg@colark.com), and originally written by
R.T. Russell has been modified slightly to compile in sjasmplus, and the CPMish
code has been removed as the BSX breadboard computer is not going to run CPM.

- GLOBAL and EXPORT directives have been removed and any global labels prefixed with @
- All source in z80 files is now enclosed in MODULES to prevent label clash
- A handful of '"' values have been converted to 34, and commented with ASCII '"'
- The patch file has been modified to work with the BSX I/O
- A build.z80 file has been added; this includes all other files and is the file to build

Other than that, it is functionally equivalent to the code provided on David Given's
website: 

http://cowlark.com/2019-06-14-bbcbasic-opensource/index.html

Dean Belfield
www.breakintoprogram.co.uk
