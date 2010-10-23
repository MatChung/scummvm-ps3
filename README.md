PS3 - ScummVM Port
==================


Environment & Building
----------------------

You need the 1.92 PS3 SDK completely set up (Paths, environment variables, MSYS when compiling on windows)

Start MSYS, cd to the checked out path and run

    ./configure --host=ps3 --enable-all-engines --disable-hugo --disable-debug  --enable-release  --disable-scalers --disable-hq-scalers
    make

If all runs well you will get an EBOOT.BIN in dists/ps3. Copy the contents of dists/ps3 to you PS3, and run it with GAIA Manager (Homebrew Mode)



Status
------

What works:

 .) Maniac Mansion is playable (tested for 5 minutes)

 .) Sound seems to work (tested with Day of the Tentacle)


What doesn't:

I have only tried a handful of games, this list is by far not complete

 .) You will get corrupted palettes in most games

 .) Many games will have an invisible cursor

 .) Curse of Monkey Island does not play videos

 .) Beneath a Steel Sky crashes during the intro
