# QCow Utils

This repo contains utilities for parsing and generally working with QCow images.

## Development Notes

For now we still do not support all the features of QCow files, neither taking into account possible interoperability.
Mostly because there isn't a real standard, and furthermore this implementation have been completed for another project.

-- Here put the table of supported features --

## Use and Installation

Just clone the repo (maybe using `--depth=1` for a shallow copy), and then simply include the `qcow_parser.h` header in your application.
NOTE: at the moment we do not offer a make target to compile it into a dynamic/static library, but there will probably be.

Once included just call the exposed functions: `qread` and `qwrite` to perform reading and writing operations.

## INFO
This folder contains the images of the OSes supported (currently only Arch Linux, which is still under testing). 

## References
- [qcow format specification](https://github.com/zchee/go-qcow2/blob/master/docs/specification.md) 
- [qemu official specification](https://github.com/qemu/qemu/blob/master/docs/interop/qcow2.txt)

