# KALS
**KALS** is a **recursive reimplementation of the Unix terminal command "ls,"** which generates an output equivalent to the "ls -R" command by default.

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

## Disclaimer:
We (Kathy and Anthony) take **NO responsibility and/or liability** for how you choose to use any of the source code available here. By using any of the files available in this repository, you understand that you are **AGREEING TO USE AT YOUR OWN RISK**.
