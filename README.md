nvesabios
=========

Dump the VESA resolution table for Nvidia VBIOS ROM.


Usage
------------------
    Usage: ./nvesabios [OPTION] [NVIDIA VBIOS]
        -v, --verbose  Show verbose VESA modelines

Starting
-------------

First of all, we need a tools to dump the VBIOS.
You can use all of your favorite tools.

For Linux users,

    git clone git://people.freedesktop.org/~stuart/vbtracetool

After `make`,

    ./vbtracetool -w 2> myvga.rom

All right, we got it.

Finally,

    ./nvesabios myvga.rom

to get dump the VESA table.
