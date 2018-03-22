# imagefs
imagefs allows you to create a file on your hard disk (or any other drive) and treat it as if it were a floppy disk.  imagefs lets you copy files onto and delete file from this virtual floppy disk, install boot sector code, and display a directory of files on the virtual disk.

## Installation

In order to compile and run imagefs, you need the a compatible C++ compiler. The `configure` script will figure out for you if your compiler is compatible enough. 

In order to compile and install imagefs on your system, type the following in the base directory of the imagefs distribution: 

    ./configure
    make
    make install

Since Imagefs uses `autoconf` you should have not trouble compiling it.

## Usage 

### General Usage 

    imagefs.exe <c/create> <image file name> <number of sectors>
    imagefs.exe <b/boot>   <image file name> <bootsector file name>
    imagefs.exe <a/add>    <image file name> <file> [more files]
    imagefs.exe <r/remove> <image file name> <file> [more files]
    imagefs.exe <d/dir>    <image file name>

### Creating an image file 

To use imagefs, the first thing you must do is create an image file. The file can be any size you want. It will always use FAT12; other file systems are not yet supported. If you want other file systems in, be sure to let me know or add them yourself. Here is the syntax for the call to imagefs: 

    imagefs.exe <c/create> <image file name> <number of sectors>

For example, in order to create an empty, formatted 3.5" HD floppy image (1.44 Mb), run this: 

    imagefs c myimage.img 2880

[The 1.44Mb floppy disk has 2880 sectors of 512 bytes each.] imagefs will respond with 

    Create new image file [myimage.img] with [2880] sectors.
    Creating FAT table.
    Image opened, current size is 0 sectors.
    Formatted 2880 sectors with fill character ÷.

This means that a new image of 2880 sectors has been created and formatted to use FAT12. The file is 1.474.560 bytes in size. You can specify other disk sizes as desired. However, imagefs only supports sector sizes of 512 bytes per sector, and clusters equals sectors. Exotic floppy formats can therefore not be used. 

### Adding files 

You can add one or more files to your image with a single command. The syntax is: 

    imagefs.exe <a/add>    <image file name> <file> [more files]

This assumes that you have an empty image ready, created as detailed in paragraph 3. For example, you could run: 

    imagefs c myimage.img 2880
    imagefs a myimage.img file1.dat file2.dat

to create an empty image of 1.44 Mb and add files file1.dat and file2.dat to it. These files will be added in order: first file1.dat, then file2.dat. If the image is full, imagefs will report an error. Normally, imagefs will respond with: 

    Add files to image file [myimage.img].
    Creating FAT table.
    Image opened, current size is 2880 sectors.
    Copying file file1.dat (22 clusters).
    Copying file file2.dat (17 clusters).

### Displaying a directory 

To see what's on the virtual diskette, you can instruct imagefs to display a directory listing. The syntax is: 

    imagefs.exe <d/dir>    <image file name>

So, continuing with our example, running 

    imagefs d myimage.img

will cause imagefs to print: 
  
    Read directory from image file [myimage.img].
    Creating FAT table.
    Image opened, current size is 2880 sectors.
    Directory listing:
    FILE1.DAT
    FILE2.DAT

### Removing files 

You can remove files from the virtual image using imagefs. This works exactly the same as on a normal diskette under DOS or Windows; the FAT table is updated to reflect the removal of the files. The syntax is: 

    imagefs.exe <r/remove> <image file name> <file> [more files]

In order to remove file1.dat from the image, we will run 

    imagefs r myimage.img file1.dat

and imagefs responds: 

    Remove files from image file [myimage.img].
    Creating FAT table.
    Image opened, current size is 2880 sectors.
    removing FAT marks...
    clearing entry
    
if the file could not be found, imagefs says: 
  
    Remove files from image file [myimage.img].
    Creating FAT table.
    Image opened, current size is 2880 sectors.
    file1.dat: file not found.

A directory listing (`imagefs d myimage.img`) will reflect the change: 

    Read directory from image file [myimage.img].
    Creating FAT table.
    Image opened, current size is 2880 sectors.
    Directory listing:
    FILE2.DAT

### Adding boot sector code 

The most interesting part of creating the image is adding boot sector code so that Bochs can boot the image. You need a flat binary file for this (.COM, .BIN) with a maximum file size of 512 bytes (one sector). If the input file is larger, the excess bytes are ignored. The syntax is: 

    imagefs.exe <b/boot>   <image file name> <bootsector file name>

So, to copy boot code onto the image we run 
  
    imagefs b myimage.img bootcode.bin

and imagefs says 
  
    Write boot sector [bootcode.bin] to image file [myimage.img].
    Creating FAT table.
    Image opened, current size is 2880 sectors.
    Warning: boot sector file (761 bytes) is larger than one sector (512 bytes).
    - skipping excess bytes.
    Boot sector copied successfully.

because our boot code happened to be larger than 512 bytes. The first 512 bytes are neatly copied into the image, though. The disk is now ready to be booted. Note that if you were to copy the resulting image to an actual floppy disk (using an imaging tool like DITU), it will work without problems. imagefs makes an exact virtual image of a normal disk. 
