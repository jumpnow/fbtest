## fbtest

A simple program to draw a rectangle of fixed color to the 
Linux frame buffer display. The rectangle will have a border
of fixed color, 10 pixels wide.
 
Used for display driver development.

#### Cross-compiling

Assuming a Yocto built toolchain

    export OETMP=<TMPDIR>
    make -f Makefile-cross

The default `Makefile-cross` assumes a hard-fp built gcc.

Comment the hard-fp line and uncomment the soft-fp line if
that's what you have.

#### Natively compiling

    make

#### Running

    root@overo:~# ./fbtest -h

    Usage ./fbtest: [-r<red>] [-g<green>] [-b<blue>] [-B<border>]
      All colors default to 0xff
      The border color applies to all rgb and is 10 pixels wide
      If border is not provided, none is drawn.
      The index defaults to 0, if your FB has more than 1 buffers, you can set it.
      If set the index to -1, we will use all buffers for 20 times.

Examples

    root@overo:~# ./fbtest
    root@overo:~# ./fbtest -r0x40
    root@overo:~# ./fbtest -r0x40 -g0 -b0
    root@overo:~# ./fbtest -r0x80 -g0 -b0
    root@overo:~# ./fbtest -r0xff -g0 -b0
    root@overo:~# ./fbtest -r0x40 -g0 -b0
    root@overo:~# ./fbtest -r0x40 -g0x40 -b0
    root@overo:~# ./fbtest -r0x40 -g0x40 -b0x30
    root@overo:~# ./fbtest -r0x40 -g0x40 -b0x30 -B0xaa
    root@overo:~# ./fbtest -r0x40 -g0x40 -b0x30 -B0xaa -i -1

#### Stop the blinking cursor

Kernel command line parameter

    vt.global_cursor_default=0


#### Wake the framebuffer when it blanks

    echo 0 > /sys/class/graphics/fb0/blank

