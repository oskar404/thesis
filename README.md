
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

So let's start and sprinkle some bash magic:

    strings reference/PROOF.PRJ | grep -E "^(PROOF|IO|RENDER|TIF).*\.C$" | sort | uniq | tr '[A-Z]' '[a-z]' | sed 's/\\/\//'

And we get a list of files:

    io/buffer.c
    io/fileio.c
    io/getopt.c
    io/message.c
    proof/color.c
    proof/ink.c
    proof/light.c
    proof/paper.c
    proof/picture.c
    proof/proof.c
    proof/render.c
    render/mfacet.c
    render/vector.c
    tif/access.c
    tif/tif_aux.c
    tif/tif_clos.c
    tif/tif_comp.c
    tif/tif_dir.c
    tif/tif_diri.c
    tif/tif_dirr.c
    tif/tif_dirw.c
    tif/tif_dump.c
    tif/tif_erro.c
    tif/tif_flus.c
    tif/tif_mach.c
    tif/tif_msdo.c
    tif/tif_open.c
    tif/tif_read.c
    tif/tif_stri.c
    tif/tif_swab.c
    tif/tif_tile.c
    tif/tif_vers.c
    tif/tif_warn.c
    tif/tif_writ.c

In shoort the scommand line searched all string starting with recognized path
names, sort the string and remove duplicates, translate the strings to lower
case and replace back slash with slash. A similar list can be generated for the
PAPER project file:

    strings reference/PAPER.PRJ | grep -E "^(PAPER|IO).*\.C$" | sort | uniq | tr '[A-Z]' '[a-z]' | sed 's/\\/\//'

    io/buffer.c
    io/fileio.c
    io/getopt.c
    io/message.c
    paper/create_c.c
    paper/create_r.c
    paper/paper.c

Based on these the first `Makefile` version can be tried.

### 4. Compiling

The real fun part starts now. The compiler is "gcc 4.8.4".

The first step is to try to compile the `proof` executable. Getting the include
paths correct is bit tedious. The `Makefile` needs to list all the components
in the include path.

The second thing noticed. The `-Wall` option is not good:

    src/proof/color.c:166:12: warning: unused variable ‘len’ [-Wunused-variable]

And need to remove `-pedantic` also:

    src/proof/proof.c:79:5: warning: string length ‘580’ is greater than the
    length ‘509’ ISO C90 compilers are required to support [-Woverlength-strings]

So much for standard c code.

Next thing to notice is `CLK_TCK` problem:

    src/proof/proof.c:236:37: error: ‘CLK_TCK’ undeclared (first use in this function)
    execution_time=(time_t)(clock()/CLK_TCK);

This is an obsolete name for `CLOCKS_PER_SEC`. Old code it seems.

Still not very far.

### 5. First Real Obstacle

    src/tif/tif_msdo.c:33:16: fatal error: io.h: No such file or directory
    #include <io.h>

Little bit of googling reveals:

    > In fact, io.h header has never been a part of ISO C nor C++ standards.
    > It defines features that belongs POSIX compatibility layer on Windows NT

It seems that I have copied just part of the `libtiff` sources as part of the
files. Simple `grep` reveals that the code is originally from 1992 and as
commented "TIFF Library MSDOS-specific Routines.". Also the file has been
renamed (probably due to Windows file system problems):

    static char rcsid[] = "$Header: /usr/people/sam/tiff/libtiff/RCS/tif_msdos.c,v 1.5 92/11/09 11:00:23 sam Exp $";

There are only the windows compatible sources available with the source code.
No unix/posix layer support or sources. The original tool was developed with
Windows. It seems that no long term support and multiple environments were in
consideration.

The google-fu is not with this one. The best I could find was
[Git Lab libtiff project](https://gitlab.com/libtiff/libtiff). This is an active
project. But the oldest commit was from 1999.

We need to do this hard way.

### 6. Hack File Operations

Copied the `src/tif/tif_msdo.c` file and ported the stuff to Unix/Posix land.
This was straight forward, because the original file used Windows Posix
adaptation layer. There were problems like this:

    src/tif/tif_unix.c:42:23: warning: cast from pointer to integer of different
    size [-Wpointer-to-int-cast]
        return (read((int) fd, buf, size));

The system I am using is a 64 bit system and original at 1993 I guess all the
systems were 32 bit. And I just disabled the checks for the GCC. This is
dangerous. Yay I was able to compile and run the executable for the first time:

    > ./proof
    Soft Proofing version 2.01, 9.11.1993
    *** Error in `./proof': free(): invalid next size (normal): 0x0000000000b548b0 ***
    Aborted (core dumped)

Oh well. `gdb` is your friend:

    (gdb) bt
    #0  0x00007ffff7741c37 in __GI_raise (sig=sig@entry=6) at ../nptl/sysdeps/unix/sysv/linux/raise.c:56
    #1  0x00007ffff7745028 in __GI_abort () at abort.c:89
    #2  0x00007ffff777e2a4 in __libc_message (do_abort=do_abort@entry=1,
        fmt=fmt@entry=0x7ffff7890310 "*** Error in `%s': %s: 0x%s ***\n")
        at ../sysdeps/posix/libc_fatal.c:175
    #3  0x00007ffff778a82e in malloc_printerr (ptr=<optimized out>,
        str=0x7ffff7890488 "free(): invalid next size (normal)", action=1) at malloc.c:4998
    #4  _int_free (av=<optimized out>, p=<optimized out>, have_lock=0) at malloc.c:3842
    #5  0x00007ffff77793fd in __fopen_internal (filename=0x622010 "paper.p", mode=0x4184e0 "r", is32=1)
        at iofopen.c:94
    #6  0x0000000000417a78 in FileIOOpen (FileName=0x622010 "paper.p", mode=0x4184e0 "r")
        at src/io/fileio.c:80
    #7  0x0000000000404511 in PaperInit (PaperName=0x622010 "paper.p") at src/proof/paper.c:143
    #8  0x000000000040660a in InitExtStruct () at src/proof/picture.c:402
    #9  0x000000000040614e in PictureCreate () at src/proof/picture.c:162
    #10 0x0000000000406805 in main (argc=1, argv=0x7fffffffdd98) at src/proof/proof.c:84
    (gdb)

So this tries to read file named `paper.p` from current directory. Copied one
to current directory. Next try and more `gdb`. It needs `light.l`. Bit more
testing and now it started to generate a picture file, but again:

    *** Error in `/home/oskar/dev/thesis/proof': free(): invalid next size (normal): 0x000000000062ba30 ***

and in `gdb`:

    #3  0x00007ffff778a82e in malloc_printerr (ptr=<optimized out>,
        str=0x7ffff7890488 "free(): invalid next size (normal)", action=1) at malloc.c:4998
    #4  _int_free (av=<optimized out>, p=<optimized out>, have_lock=0) at malloc.c:3842

I need to take closer look on the memory handling of this program. Too much
simple pointer to `int` casts probably. In this specific instance the closing the
image failed in the old TIFF library.

### 7. All those moments will be lost in time... like tears in rain...

When inspecting and trieng to figure out the `SIGABRT` problem the first suspect
was `first->fp=fopen(FileName,mode);` in `fileio.c`. According to man pages the
`fopen()` should return `NULL` when problems and set `errno`. Simple code
reproduction without any of the other stuff revealed that the problems was some
where else than in `glibc` implementation. Little bit of googling suggested that
there is a memory corruption somewhere.

We need a better hammer. The `valgrind` reveled something else:

    > valgrind ./proof
    ==5994== Memcheck, a memory error detector
    ==5994== Copyright (C) 2002-2015, and GNU GPL'd, by Julian Seward et al.
    ==5994== Using Valgrind-3.11.0 and LibVEX; rerun with -h for copyright info
    ==5994== Command: ./proof
    ==5994==
    Soft Proofing version 2.01, 9.11.1993
    ==5994== Invalid write of size 8
    ==5994==    at 0x405BB3: ColorVectorInit (color.c:299)
    ==5994==    by 0x4057D5: ColorInit (color.c:199)
    ==5994==    by 0x401E6B: InitExtStruct (picture.c:400)
    ==5994==    by 0x4019E7: PictureCreate (picture.c:162)
    ==5994==    by 0x405037: main (proof.c:84)
    ==5994==  Address 0x550e248 is 0 bytes after a block of size 3,208 alloc'd
    ==5994==    at 0x4C2DB8F: malloc (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
    ==5994==    by 0x405B7B: ColorVectorInit (color.c:295)
    ==5994==    by 0x4057D5: ColorInit (color.c:199)
    ==5994==    by 0x401E6B: InitExtStruct (picture.c:400)
    ==5994==    by 0x4019E7: PictureCreate (picture.c:162)
    ==5994==    by 0x405037: main (proof.c:84)

When looking at code in `color.c:299` the code was having a buffer overflow
problem:

    -    for(i=0;i<=color.size;i++)
    +    for(i=0;i<color.size;i++)

This never was a problem in Windows environment. The `malloc` and friends
behaved differently there. In Linux the overflow was too much.
