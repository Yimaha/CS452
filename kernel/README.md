# Assignment Submissions

A lot of this README was borrowed from the sample code README. If you are looking for proper build/run instructions for our assignment submissions, please see the top-level README.

## ***** IMPORTANT *****

The setupTFTP.sh script does not provide any protection, i.e., students can
potentially overwrite each other's binaries!  Please only run the script
after you have verified that no other user uses that particular RPi.

Also, please do not tinker with the 'public' directories in /u/cs/452/public
directly.  This might leave the TFTP load process unusable, which might be a
problem outside regular business hours.

### Build and Run (linux.student.cs.uwaterloo.ca)

```bash
cp -a /u/cs452/public/iotest <dest>
cd <dest>
make
/u/cs452/public/tools/setupTFTP.sh <barcode> kernel8.img
# reboot RPi
```

### Output on Control PC (as realfolk user)

```bash
logRPi.sh <barcode>
```

### Barcodes (examples)

CS017540
CS017541
CS017542
CS017543

## Knowledge

1. you need to store registers before calling a certain function, under assembly, especially if you are calling a C function, otherwise parameters go bye bye
2. why does HCR_RW have the RW bit set to 1? what does it do

## Changing C to C++

1. Makefile .c -> .cc
2. based on <https://wiki.osdev.org/C%2B%2B>, new flag is added -> `-nostdlib -fno-rtti -fno-exceptions`
3. there are issues with static variable that are not directly assigned to a constant, thus, some of the variables under rpi.c such as spi, aux is changed from a variable to an lazily loaded pointer to the memory. Now, memory would be correct
4. `extern "C"` is used on `memset`, etc under rpi.cc, it is mandatory for program to compile yet the C++ requires some of the C functionality to perform basic operations such as creating a string "hello world", in which, we need to provide a proper funcrtion so linker doesn't break. However, C++ doesn't follow C's naming convention, thus for the underlying linker to identify memset, we need to use `extern "C"` to convert it back to the C compiler format. -> <https://stackoverflow.com/questions/1041866/what-is-the-effect-of-extern-c-in-c>
5. `restrict` is replaced by `__restrict__`
6. Added some extra support for globals, specifically added the block

    ```ld
    /* mark constructor initialization array */
    .init_array : ALIGN(4) {
        __init_array_start = .;
        *(.init_array)
        *(SORT_BY_INIT_PRIORITY(.init_array.*))
        __init_array_end = .;
    }
    ```

    to `linker.ld`, this allows global values to exist properly.

    \*For some reason the BASE values in rpi.cc all have to be hardcoded independently, they cannot all depend on `MMIO_BASE`, seems like constructor init priority is still weird for global values but changing it works and hopefully it will continue to work. This code was taken from <https://forums.raspberrypi.com/viewtopic.php?t=34925>.

## What still needs to be done

1. (DONE) <https://piazza.com/class/lar7zlqtolr5xe/post/31>
2. (DONE) <https://piazza.com/class/lar7zlqtolr5xe/post/33>
3. k1 test process needed to update the actual key to the return value of create task to see if the value is correct
4. we need to have flag-based debugging. This means during make, we can setup flags like "make -DEBUG" that will generate executables with debug enabled, so we can have weird debug lines in our code while the production build doesn't get influenced. refer -> <https://stackoverflow.com/questions/14251038/debug-macros-in-c>. This is incredibly handy in the future and ideally should be available for k1.
5. (sort-of done) We need an offical space to store documentation and design docs, I will get them setup
6. (DONE) Proper slab allocation is needed for kernel.cc w/ delete, ideally a slab size of 100, need to be tested at least manually
7. SPSR restore is not yet complete, needs to be done still.
