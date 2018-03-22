# This spec file was generated using Kpp
# If you find any problems with this spec file please report
# the error to ian geiser <geiseri@msoe.edu>
Summary:   Imagefs creates a virtual FAT12 file system in a single file.
Name:      imagefs
Version:   1.03
Release:   1.03
Copyright: GPL
Vendor:    Alex van Oostenrijk <vanoostenrijk [@] hotmail [.] com>
Url:       http://www.dosx.f2s.com

Packager:  Alex van Oostenrijk <vanoostenrijk [@] hotmail [.] com>
Group:     Development/Tools
Source:    imagefs-1.03.tar.gz
BuildRoot: 

%description
imagefs allows you to create a file on your hard disk (or any other drive) and treat it as if
it were a floppy disk.  imagefs lets you copy files onto and delete file from this virtual
floppy disk, install boot sector code, and display a directory of files on the virtual disk.

imagefs is especially useful for testing bootable software with Bochs.  Instead of being
forced to copy your files to a diskette every time you want a test run, you can now have
bochs boot the image file instead.  This saves minutes every time you test.  Plus, you can
include calls to imagefs in your makefile, thus automating the process.

imagefs was developed for use in the development of the Slikware operating system


%prep
%setup
CFLAGS="$RPM_OPT_FLAGS" CXXFLAGS="$RPM_OPT_FLAGS" ./configure \
                 \
                $LOCALFLAGS
%build
# Setup for parallel builds
numprocs=`egrep -c ^cpu[0-9]+ /proc/stat || :`
if [ "$numprocs" = "0" ]; then
  numprocs=1
fi

make -j$numprocs

%install
make install-strip DESTDIR=$RPM_BUILD_ROOT

cd $RPM_BUILD_ROOT
find . -type d | sed '1,2d;s,^\.,\%attr(-\,root\,root) \%dir ,' > $RPM_BUILD_DIR/file.list.imagefs
find . -type f | sed 's,^\.,\%attr(-\,root\,root) ,' >> $RPM_BUILD_DIR/file.list.imagefs
find . -type l | sed 's,^\.,\%attr(-\,root\,root) ,' >> $RPM_BUILD_DIR/file.list.imagefs

%clean
rm -rf $RPM_BUILD_ROOT/*
rm -rf $RPM_BUILD_DIR/imagefs
rm -rf ../file.list.imagefs


%files -f ../file.list.imagefs
