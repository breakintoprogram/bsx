# GAL
The GALasm code and output files for the ATF22V10 PLD
- address_decode.pld: The source file
- address_decode.pin: Pinout type file
- address_decode.chp: Pinout chip file
- address_decode.fus: The fuse map
- address_decode.jed: The file to burn to the ATF22V10

#### NB
Programming GALs can be a bit fiddly:
- The ATF22V10C I use can be successfully programmed by a MiniPro TL866II+ EPROM programmer
- Make sure that when burning the option to encrypt the chip is turned off
- Mileage with other 22V10 GALs and EPROM burners may vary

Link to my webpage with tips on using GALasm [here](http://www.breakintoprogram.co.uk/projects/homebrew-z80/homebrew-z80-computer-part-5)
