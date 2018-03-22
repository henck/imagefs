/***************************************************************************
                          imagefs.cpp  -  description
                             -------------------
    begin                : Sat Feb 9 2002
    copyright            : (C) 2002 by Alex van Oostenrijk
    email                : vanoostenrijk [@] hotmail [.] com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/***********************************************************
 *
 * Purpose:
 * imagefs can create and manage a disk image of any size
 * and offers functions that can use it as if it were a disk.
 * The disk image file that imagefs outputs/updates may be
 * used as input to Bochs.
 *
 * Operations:
 *
 * - create new image file of [size] with FAT12 (for now),
 *   filled (formatted) with [character]. Create empty FAT
 *   table.
 *
 * - write bootsector to [image] from [file]
 *
 * - Copy [file(s)] into [image]. Updates FAT.
 *
 * - (optional) Delete [file(s)] from [image]. Updates FAT.
 *
 * - Display directory contents of [image]
 *
 *
 * Syntax:
 *
 * imagefs <action> [arguments]
 * Possible values
 * imagefs <c/create> <image file name> <number of sectors>
 * imagefs <b/boot>   <image file name> <bootsector file name>
 * imagefs <a/add>    <image file name> <file> [more files]
 * imagefs <r/remove> <image file name> <file> [more files]
 * imagefs <d/dir>    <image file name>
 *
 *
 * Comments:
 * - This project makes use of STL.
 *
 ***********************************************************/

/***********************************************************
 *
 * Purpose:
 * Contains main() function and globals.
 *
 * Comments:
 *
 **********************************************************/


#include <stdio.h>
#include <string.h>
#include <iostream.h>
#include "tools.h"
#include "image.h"
#include "rootdirentry.h"
#include <vector>

using namespace std;

enum
{
  OP_UNKNOWN,
  OP_CREATE,
  OP_WRITEBOOT,
  OP_ADD,
  OP_REMOVE,
  OP_DIR
} operations;

//********** GLOBALS ********************

char *pcImageFile;
int iNumberOfSectors;
char *pcBootSectorFile;
vector <char *> filelist;
unsigned int iFileSize;


/***********************************************************
 *
 * function: usage
 *
 * Purpose:
 * Displays program options. Displayed when wrong or
 * insufficient arguments are supplied by user.
 *
 * Arguments:
 *   pcDirname = full path to project executable (should
 *               be argv[0]).
 *
 *
 * Pre: true
 *
 *
 * Post: -
 *
 *
 **********************************************************/

void usage(char *pcDirname)
{
  char *pcExecname;

  // Retrieve executable file name from pcDirname
  pcExecname = pcDirname;
  // go to end of string
  while (*pcExecname != '\0') pcExecname++;
  // search back to first slash or start of string
  while (pcExecname >= pcDirname && *pcExecname != '\\' && *pcExecname != '/') pcExecname--;
  pcExecname++;

  printf ("Usage:\n");
  printf ("%s <c/create> <image file name> <number of sectors>\n", pcExecname);
  printf ("%s <b/boot>   <image file name> <bootsector file name>\n", pcExecname);
  printf ("%s <a/add>    <image file name> <file> [more files]\n", pcExecname);
  printf ("%s <r/remove> <image file name> <file> [more files]\n", pcExecname);
  printf ("%s <d/dir>    <image file name>\n", pcExecname);
}


/***********************************************************
 *
 * function: main
 *
 * Purpose:
 * Program entry point.
 *
 *
 * Arguments:
 *   argc - contains number of arguments supplied to program,
 *          including one implicit argument: the executable
 *          path.
 *   argv - list of additional arguments.
 *          argv[0] is the full path to the executable.
 *
 *
 * Pre: true
 *
 *
 * Post:
 *  0 - program completed successfully.
 *  1 - abnormal program termination.
 *
 *
 **********************************************************/
int main (int argc, char **argv)
{
  int operation;
  int i;
  CImage *pImage;

  // Check whether we were called with any arguments
  if (argc < 2)
  {
    usage(argv[0]);
    return 1;
  }

  // See what operation we are instructed to perform
  operation = OP_UNKNOWN;
  if (strcmpcase (argv[1], "create") == true || strcmpcase (argv[1], "c") == true) operation = OP_CREATE;
  if (strcmpcase (argv[1], "boot") == true || strcmpcase (argv[1], "b") == true) operation = OP_WRITEBOOT;
  if (strcmpcase (argv[1], "add") == true || strcmpcase (argv[1], "a") == true) operation = OP_ADD;
  if (strcmpcase (argv[1], "remove") == true || strcmpcase (argv[1], "r") == true) operation = OP_REMOVE;
  if (strcmpcase (argv[1], "dir") == true || strcmpcase (argv[1], "d") == true) operation = OP_DIR;

  // If operation is not known, quit.
  if (operation == OP_UNKNOWN)
  {
    usage(argv[0]);
    return 1;
  }

  // Depending on operation, parse arguments
  switch (operation)
  {
  case OP_CREATE:
    // arg #1 = 'create/c', arg #2 = image file name, arg #3 = number of sectors
    if (argc != 4)
    {
      usage(argv[0]);
      return 1;
    }
    pcImageFile = argv[2];
    iNumberOfSectors = atoi (argv[3]);
    printf ("Create new image file [%s] with [%d] sectors.\n", pcImageFile, iNumberOfSectors);
    break;


  case OP_WRITEBOOT:
    // arg #1 = 'boot/b', arg #2 = image file name, arg #3 = bootsector file name
    if (argc != 4)
    {
      usage(argv[0]);
      return 1;
    }
    pcImageFile = argv[2];
    pcBootSectorFile = argv[3];
    printf ("Write boot sector [%s] to image file [%s].\n", pcBootSectorFile, pcImageFile);
    break;


  case OP_ADD:
    // arg #1 = 'add/a', arg #2 = image file name, arg #3...#n = files
    if (argc < 4)
    {
      usage(argv[0]);
      return 1;
    }
    pcImageFile = argv[2];
    printf ("Add files to image file [%s].\n", pcImageFile);
    for (i = 3; i<argc; i++)
    {
      filelist.push_back (argv[i]);
    }
    break;


  case OP_REMOVE:
    // arg #1 = 'remove/r', arg #2 = image file name, arg #3...#n = files
    if (argc < 4)
    {
      usage(argv[0]);
      return 1;
    }
    pcImageFile = argv[2];
    printf ("Remove files from image file [%s].\n", pcImageFile);
    for (i = 3; i<argc; i++)
    {
      filelist.push_back (argv[i]);
    }
    break;


  case OP_DIR:
    // arg #1 = 'dir/d', arg #2 = image file name
    if (argc != 3)
    {
      usage(argv[0]);
      return 1;
    }
    pcImageFile = argv[2];
    printf ("Read directory from image file [%s].\n", pcImageFile);
    break;

  default:
    printf ("Fatal: unknown operation (this should never happen).\n");
    return 1;
    break;
  }

  // Perform requested operation
  switch (operation)
  {
  case OP_CREATE:
    // try and create new image file
    try
    {
      pImage = new CImage (pcImageFile, true);
    }
    catch (...)
    {
      cout << "Fatal: could not create file " << pcImageFile << "." << endl;
    }

    // format new image file to correct size
    if (pImage->format (iNumberOfSectors, (char) 0x0F6) == false)
      return 1;

    // Make empty FAT
    pImage->FAT()->clear();
    // Make empty root directory
    pImage->RootDir()->clear();
    break;
  case OP_WRITEBOOT:
    // try and open image file
    try
    {
      pImage = new CImage (pcImageFile, false);
    }
    catch (...)
    {
      cout << "Fatal: could not open file " << pcImageFile << "." << endl;
    }

    if (pImage->writeBootSector (pcBootSectorFile) == false)
      return 1;

    break;
  case OP_ADD:
    // try and open image file
    try
    {
      pImage = new CImage (pcImageFile, false);
    }
    catch (...)
    {
      cout << "Fatal: could not open file " << pcImageFile << "." << endl;
    }

    // Loop through list of input files and add them one by one.
    for (i=0; i<filelist.size(); i++)
      pImage->addFile (filelist[i]);
    break;
  case OP_REMOVE:
    // try and open image file
    try
    {
      pImage = new CImage (pcImageFile, false);
    }
    catch (...)
    {
      cout << "Fatal: could not open file " << pcImageFile << "." << endl;
    }

    // Loop through list of input files and remove them one by one.
    for (i=0; i<filelist.size(); i++)
      pImage->removeFile (filelist[i], false);
    break;
  case OP_DIR:
    // try and open image file
    try
    {
      pImage = new CImage (pcImageFile, false);
    }
    catch (...)
    {
      cout << "Fatal: could not open file " << pcImageFile << "." << endl;
    }

    // Show directory listing.
    cout << "Directory listing: " << endl;
    pImage->dir();
    break;
  }

  return 0;
}
