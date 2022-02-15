# KALS Team Project
<strong>KALS</strong> is a <strong>recursive reimplementation of the Unix terminal command "ls</strong>," which generates an output equivalent to the "ls -R" command by default.

To build, run the following command:

    make -f kals.mk

## Usage
The available list of flags can be shown by invoking:

    ./kals.out -h

or, depending on your system:

    kals.out -h

and is listed below:

    Flags:
    -a: include above directories
    -f: recognize links/shortcuts
    -d: exclude directory contents
    -l: directory long listing format
    -h: help guide

Additionally, flags can be combined and directories can (although not necessary) be specified amongst those flags, some examples may include:

    ./kals.out -lf directory1 -a directory2
    ./kals.out -laf
    ./kals.out directory1 directory2

## Created by:
- Kathy Tran
- Anthony Makaj
