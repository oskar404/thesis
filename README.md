
# Reproducing: Model Based Visualization of Paper

This is my old thesis work done long ago (1994). I stumbled across the source
material and decided to check how my coding was then and if I am able compile
this.

The subject of the thesis was "Model based visualization of paper". I guess the
time has passed and the progress has made this work obsolete. Who knows I
haven't followed the progress in the printing industry.

1. [Summary "Model based visualization of paper"](summary.pdf)
2. [Thesis "Painojäljen mallipohjainen visualisointi näyttöjärjestelmässä" (in Finnish)](thesis.pdf)

## Digital Archealogy

This became a digital archealogy project. Reproducability is an important part
of the scientific work. The result document is available and the source codes
but no README files or anything describing the created software and how to use
it. So this became an adventure in trying to figure what the stuff really did.
So I decided to document what has been done so far.

### 1. Code Cleaning

Originally code is developed in Windows (probably with Windows NT or then
Windows 3.11). All files are uppercase. Converted file names to lowercase
before commiting them to git repo.

Tools:
- Ubuntu 14.04.5 LTS
- GNU bash, version 4.3.11(1)-release (x86_64-pc-linux-gnu)

Get rid of Windows line breaks:

    find src -type f -exec dos2unix {} \;

Removed trailing whitespaces:

    function trim_ws(){
        sed -i 's/[ \t]*$//' $@
    }
    trim_ws $(find src -type f)

Replaced tabs with spaces:

    function trim_tab(){
        while [ "$1" != "" ]; do
            expand -t 4 "$1" | sponge "$1"
            shift
        done
    }
    trim_tab $(find src -type f)

### 2. Visual Inspection

No working build file or README which would directly tell what all the code
does. Some browsing around and file reading is needed. This is c code. No doubt
about that.

    grep -r main src/*

Simple grep revealed 4 main() functions in files:

    src/convert/scale.c
    src/paper/paper.c
    src/proof/proof.c
    src/render/int_test.c

Sources contain some documentation:

    Scale.c - Main program for scaling

    paper.c - Main program for paper structure creation program

    Proof.c - Main program for softproofing

    integrator test - when run this should produce

Some files seemed identical. There are multiple e.g. defs.h files.

    sha1sum $(find src -type f) | sort

The command revealed 3 identical defs.h files in different directories:

    124b14b95da3b8a3be214706b4ff0640c925df18  src/ink/defs.h
    124b14b95da3b8a3be214706b4ff0640c925df18  src/light/defs.h
    124b14b95da3b8a3be214706b4ff0640c925df18  src/proof/defs.h

Then there are paper.h and proof.h which are also identical:

    5b191bd982307a75cec7d46b1f85d3f02a6abc03  src/paper/paper.h
    5b191bd982307a75cec7d46b1f85d3f02a6abc03  src/proof/proof.h

It seems that using include directories properly and making reusable components
have not been the top priority.

The `src/tif` directory contains a third party TIFF library:

    head src/tif/*

    * Copyright (c) 1988, 1989, 1990, 1991, 1992 Sam Leffler
    * Copyright (c) 1991, 1992 Silicon Graphics, Inc.

The library is with permissive license (no license violation there). Some more
digging:

    grep "(c)" $(find src -type f)

`src/io/getopt.c` is thrid party with permissive license also:

    * Copyright (c) 1987 Regents of the University of California.

This file is standard Unix/Linux file, but when the code has been developed it
was practical just to copy the source as part of the other sources.

### 3. Digging Build System

There was an original project file and an executable. The project file is a
binary file and the `less` tool revealed: "Turbo C Project File"

Googling does not reveal tools which could read this file directly. The simple
solution might be digging up some Borland tools, but where is the fun of that.
I wanted a `Makefile`.

What does the project file contain. Extracting strings from the project file:

    strings reference/PROOF.PRJ

This project file had references to directories PROOF, IO, RENDER and TIF. The
second project file

    strings reference/PAPER.PRJ

has references to PAPER and IO directories. So here is some idea how what is
the contents for two executables. Still the two other main functions are a
mystery.
