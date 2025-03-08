## INFO
This folder contains the images of the OSes supported (currently only Arch Linux, which is still under testing). 

Furthermore, the images (for now) will use the qcow format ([qcow format specification](https://github.com/zchee/go-qcow2/blob/master/docs/specification.md), 
[qemu official specification](https://github.com/qemu/qemu/blob/master/docs/interop/qcow2.txt)), 
as currently there is no interest on trying to define a new format (which would still require a program to convert the common qcow format in the one created).

Finally, along those images are included the tools needed to manage, parse, and more.

## Operational Note
Here is how currently the OS images handling is going to be performed:
 - First the utility `qcow2img` will parse the qcow2 image and produce the raw virtual disk image;
 - Then the image will be loaded inside the disk ( or even a partition ) of the machine that is running the hypervisor;
 - Once loaded into the disk, the usual boot process will start, by starting the bootloader already provided by the image,
   optionally as a fallback, or if specified, will be used a custom bootloader that will be provided along the hypervisor;
 - From this point the operation should be seemless to booting into an OS on bare-metal.

Furthemore, along the disk space will be allocate, as usual, the RAM memory for the VM.
Also, while the Guest has indeed direct access to the disk, it will be routed via the secure/controlled DMA provided by the CPU (for AMD is implemented via NorthBridge controllers).

For what concerns both storing and loading image snapshot:
 - First the virtual disk image is extracted and formated back into qcow2 format;
 - Alongside, the RAM memory and CPU state dumps will be stored using a custom format (which will also provide optional compression and other useful features);
 - Finally, everything is stored along a small file containing metadata needed to load back the snapshot (the entire folder could be optionally compressed into an archive like a tarball to save more space).

For loading back the image the operations will be executed in reverse.

*NOTE*: this procedures described above have not been extensively tested, neither precisely defined, hence they are obviously subject to change.
