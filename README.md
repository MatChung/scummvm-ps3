PS3 - ScummVM Port
==================


Environment & Building
----------------------

You need the 1.92 PS3 SDK completely set up (Paths, environment variables, MSYS when compiling on windows)

Start MSYS, cd to the checked out path and run

    ./configure --host=ps3 --disable-debug  --enable-release  --disable-scalers --disable-hq-scalers
    make

If all runs well you will get an EBOOT.BIN in dists/ps3. Copy the contents of dists/ps3 to you PS3, and run it with GAIA Manager (Homebrew Mode)

If you want debug information or get crashes add "--ps3-debug=IP" to your configure line and udp debug messages will get sent to this ip on port 3490. ("nc -l -u 3490" will display them)

The rest of the configure switches work too ("./configure --help" to list them), all engines build except the Hugo series.




Status
------

My testsetup:

.) 1080p (it should autoselect the best resolution for your TV)

.) Gaia Manager: git pull bd20548e7fb1d0ee8f4cec2c9bc5f46a5c8878e7

.) psgroove (with PL3): git pull 614cdb6e7d2d5bc400b39b5bd9585cba43ab80d6

.) scummvm installed to "/dev_hdd0/game/GAIA01985/homebrew/scumm/"

.) scummmodern.zip installed to "/dev_hdd0/scummvm/" (not needed if you want the default theme)



What works:

 .) Maniac Mansion is playable (tested for 5 minutes)

 .) Sound seems to work (tested with Day of the Tentacle Talkie)


What doesn't:

I have only tried a handful of games, this list is by far not complete

 .) You will get corrupted palettes in most games

 .) Beneath a Steel Sky crashes during the intro


If you try it and get crashes, please include the log output you got from --ps3-debug=IP
----------------------------------------------------------------------------------------

