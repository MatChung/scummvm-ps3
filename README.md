PS3 - ScummVM Port
==================


Environment & Building
----------------------

You need the 1.92 PS3 SDK completely set up (Paths, environment variables, MSYS when compiling on windows)

Start MSYS, cd to the checked out path and run

    ./configure --host=ps3 --disable-debug --enable-release --disable-mt32emu
    make

If all runs well you will get an EBOOT.BIN in dists/ps3. Copy the contents of dists/ps3 to you PS3, and run it with GAIA Manager (Homebrew Mode)

If you want debug information or get crashes add "--ps3-debug=IP" to your configure line and udp debug messages will get sent to this ip on port 3490. ("nc -l -u 3490" will display them)

The rest of the configure switches work too ("./configure --help" to list them), all engines build except the Hugo series.


Keybindings:
------------

 Left Stick - move mouse
 X - left mouse button
 O - right mouse button
 D-Pad - cursor keys
 start - show overlay (save, load, quit, etc)
 select - ESC (to quit videos or other sequences)



Status
------

My testsetup:

.) 1080p (it should autoselect the best resolution for your TV)

.) Gaia Manager: git pull bd20548e7fb1d0ee8f4cec2c9bc5f46a5c8878e7

.) psgroove (with PL3): git pull 614cdb6e7d2d5bc400b39b5bd9585cba43ab80d6

.) scummvm installed to "/dev_hdd0/game/GAIA01985/homebrew/scumm/"

.) scummmodern.zip installed to "/dev_hdd0/scummvm/" (not needed if you want the default theme)



What works:

 .) most of it


What doesn't:

I have only tried a handful of games, this list is by far not complete

 .) Beneath a Steel Sky crashes during the intro

 .) in some games the cursor has no transparent colors

 .) slowdowns may occur

 .) MT32 emu has some performance problems (very bad in MI2 intro)



If you try it and get crashes, please include the log output you got from --ps3-debug=IP and send it to lousy[at]lousy{dot}at
-----------------------------------------------------------------------------------------------------------------------------

