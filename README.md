## fbtest

A simple program to draw a rectangle of fixed color to the 
Linux frame buffer display. The rectangle will have a border
of fixed color, 10 pixels wide.
 
Used for display driver development.

#### Cross-compiling

Assuming a Yocto built toolchain

    export OETMP=<TMPDIR>
    ln -s Makefile-cross Makefile
    make


#### Natively compiling

    ln -s Makefile-native Makefile
    make

