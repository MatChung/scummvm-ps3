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

.) automatically builds installable package with "make pkg" (may need testing)

.) Virtual keyboard enabled (I will select a better layout later)

.) enable keyboard everywhere with R1 (if its not automatic)


Feedback, patches, or anything like it is welcome, the location I watch is http://psx-scene.com/forums/f149/project-scummvm-ps3-port-69286/ or my github account.
(I am looking at you MagicSeb! If you add something, or are aware of bugs a short line should not be to much.)


99% of the graphics glitches are gone, the worst thing is a broken cursor

Games tested:
Monkey Island 1 & 2 (FMTowns Version)
works, except the cursor is broken after playing
Monkey Island 3
works, sometimes sound skipping (cracks)
Day of the Tentacle:
works 100%
The Legend of Kyrandia (CD Version)
works 100%
Beneath a Steel Sky (CD and Floppy)
crashes at intro, but if skipped it works fine
Discworld
works 100%

