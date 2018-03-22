/***************************************************************************
                          image.cpp  -  description
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
 * Contains the CImage class. This class can open an image
 * (upon creation of a CImage object) and create a new image
 * file if it does not yet exist.
 * Methods are offered for formatting, writing a boot
 * sector, adding files, removing files and showing a
 * directory listing.
 *
 * Comments:
 *
 *
 *
 **********************************************************/


#include "image.h"


/***********************************************************
 *
 * function: CImage constructor
 *
 * Purpose:
 * Create an instance of the CImage class.
 *
 * Arguments:
 * pcFilename: full path to the image file open.
 * bCreate: if true, create image file if it does not
 *          yet exist.
 *
 *
 * Pre: true
 *
 *
 * Post:
 * An instance of CImage is created, with an open read/
 * write stream to the associated image file.
 *
 *
 **********************************************************/

CImage::CImage(char *pcFilename, bool bCreate) // throws 0
{
  // Open file. If bCreate then create it as necessary;
  if (bCreate)
    fStream.open (pcFilename, ios::in | ios::out | ios::binary);
  else
    fStream.open (pcFilename, ios::in | ios::out | ios::binary | ios::nocreate);

  if (!fStream) throw (0);

  // Find out what the stream size is
  fStream.seekg (0, ios::end);
  iSize = fStream.tellp() / SECTSIZE;

  fStream.seekg (0, ios::beg);

  // Connect FAT table
  pFat = new CFat (&fStream);

  // Connect root directory
  pRootDir = new CRootDir (&fStream);

  // Display stream size.
  cout << "Image opened, current size is " << iSize << " sectors." << endl;
}

/***********************************************************
 *
 * function: CImage destructor
 *
 * Purpose:
 * Destroy an instance of CImage
 *
 * Arguments: none
 *
 * Pre: true
 *
 * Post:
 * Associated FAT and RootDir objects are deleted.
 * Associated read/write stream to image file is closed.
 * Instance of CImage class is deleted.
 *
 *
 **********************************************************/

CImage::~CImage()
{
  delete pFat;
  delete pRootDir;

  fStream.close();
}

/***********************************************************
 *
 * function: CImage::format
 *
 * Purpose:
 * Format image file to desired number of sectors, using
 * specified fill character.
 *
 * Arguments:
 * iNumberOfSectors: new size of image file in sectors.
 * cFillChar: character to fill formatted space with.
 *
 * Pre: The stream to the image file is open.
 * This should be the case when the CImage instance
 * is created.
 *
 * Post:
 * The image file is now iNumberOfSectors in size, which
 * are filled with cFillChar.
 *
 * Return values:
 * true: format successful.
 * false: number of sectors too low, must be at least 19
 *        to contain two FAT tables and boot sector.
 *
 **********************************************************/

bool CImage::format(int iNumberOfSectors, char cFillChar)
{
  int i;
  char *pStr;

  // Verify that image file is large enough to contain FAT
  if (iNumberOfSectors < FATSIZE*2 + BOOTSIZE)
  {
    cout << "Fatal: # sectors must be at least 19 to contain FAT and boot sector." << endl;
    return false;
  }

  // Create contents of 1 sector
  pStr = new char[SECTSIZE];
  for (i = 0; i < SECTSIZE; i++) pStr[i] = cFillChar;

  // Format all sectors
  fStream.seekg (0, ios::beg);
  for (i = 0; i < iNumberOfSectors; i++)
  {
    fStream.write (pStr, SECTSIZE);
  }
  fStream.flush();

  // Clear up sector contents from memory
  delete[] pStr;

  // Report number of sectors formatted
  cout << "Formatted " << iNumberOfSectors << " sectors with fill character " << cFillChar << "." << endl;
  return true;
}

/***********************************************************
 *
 * function: CImage::writeBootSector
 *
 * Purpose:
 * Write a bootsector from a file to the image
 *
 * Arguments:
 * pcFilename: full path to bootsector file.
 *
 * Pre: stream to image file is open.
 *
 * Post:
 * Bootsector is written to first sector of image.
 *
 *
 * Return values:
 * true: write successful.
 * false: could not open bootsector input file or
 *        input file was less than 1 sector in size.
 *
 *
 **********************************************************/
bool CImage::writeBootSector (char *pcFilename)
{
  int iSize;
  int i;
  char c;

  // Attempt to open boot sector input file
  fstream ff;
  ff.open (pcFilename, ios::in | ios::binary | ios::nocreate);
  if (!ff)
  {
    cout << "Fatal: could not open boot sector input file." << endl;
    return false;
  }

  // Find out size of boot sector input file
  ff.seekg (0, ios::end);
  iSize = ff.tellp();
  ff.seekg (0, ios::beg);

  // Determine if boot sector input file is large enough
  if (iSize < SECTSIZE)
  {
    cout << "Fatal: boot sector file size must be at least " << SECTSIZE << " bytes." << endl;
    ff.close();
    return false;
  }

  if (iSize > SECTSIZE)
  {
    cout << "Warning: boot sector file (" << iSize << " bytes) is larger than one sector (" << SECTSIZE << " bytes)." << endl;
    cout << "- skipping excess bytes." << endl;
  }

  // Copy boot sector into image
  fStream.seekg (0, ios::beg);
  for (i=0; i<512; i++)
  {
    ff.get (c);
    fStream.put (c);
  }
  fStream.flush();

  ff.close();

  cout << "Boot sector copied successfully." << endl;
  return true;
}

/***********************************************************
 *
 * function: CImage::extractFilename
 *
 * Purpose:
 * Extract a file name from a path.
 *
 * Arguments:
 * pcPath: path to extract file name from.
 *
 * Pre: pcPath points to valid string.
 *
 * Post: Filename was extracted from path.
 *
 * Return values:
 * Returns the extracted file name.
 *
 ***********************************************************/

char *extractFilename (char *pcPath)
{
  char *pcStart;

  pcStart = pcPath;
  // go to end of path string.
  while (*pcPath != '\0') pcPath++;
  // Search back to first slash or start of string.
  while (pcPath >= pcStart && *pcPath != '\\' && *pcPath != '/')
    pcPath--;
  pcPath++;

  return (pcPath);
}

/***********************************************************
 *
 * function: CImage::addFile
 *
 * Purpose:
 * Add a file to the open image, if there's room.
 *
 * Arguments:
 * pcFilename: full path to file to add.
 *
 * Pre: image stream is open.
 *
 * Post: File is added to the image.
 *       If the file already existed, it was overwritten.
 *
 * Return values:
 * true: file successfully added.
 * false: file could not be opened, or
 *        root directory of image is full, or
 *        not enough free space in image, or
 *        could not write directory entry.
 *
 **********************************************************/

bool CImage::addFile (char *pcFilename)
{
  fstream ff;
  unsigned int iFileSizeBytes;
  unsigned int iFileSizeSectors;
  char acSector[SECTSIZE];
  int i;
  int iClusterNr;
  int iStartClusterNr;

  // Attempt to open input file
  ff.open (pcFilename, ios::in | ios::binary | ios::nocreate);
  if (!ff)
  {
    cout << "Error: could not open input file " << pcFilename << "." << endl;
    return (false);
  }

  // Remove path from fle name.
  pcFilename = extractFilename (pcFilename);

  // See if a file with the same name already exists on the disk. If so, remove
  // it first.
  removeFile (pcFilename, true);

  // Find out if there's enough space in the image's root directory.
  if (pRootDir->freesize() == 0)
  {
    cout << "Error: root directory is full." << endl;
    return (false);
  }

  // Find out size of input file
  ff.seekg (0, ios::end);
  iFileSizeBytes = ff.tellp();
  iFileSizeSectors = ff.tellp() / SECTSIZE;
  if ((ff.tellp() % SECTSIZE) != 0) iFileSizeSectors++;
  ff.seekg (0, ios::beg);

  cout << "Copying file " << pcFilename << " (" << iFileSizeSectors << " clusters)." << endl;

  // Determine if there are enough free clusters in the FAT table
  if (FAT()->freesize() < iFileSizeSectors)
  {
    cout << "Error: Not enough free space in image." << endl;
    return (false);
  }

  iStartClusterNr = FAT()->firstfree();

  do
  {
    // Read one sector from the input file
    memset (acSector, 0, 512);
    for (i=0; i<SECTSIZE; i++)
    {
      ff.get(acSector[i]);
      if (!ff) break;
    }

    iClusterNr = FAT()->firstfree();

    // Copy cluster onto image
    fStream.seekg ((FATSIZE * 2 + BOOTSIZE + ROOTDIRSIZE + (iClusterNr - 2)) * SECTSIZE, ios::beg);
    for (i=0; i<SECTSIZE; i++)
    {
      fStream.put (acSector[i]);
    }

    // place end of allocation chain mark.
    FAT()->putmark (iClusterNr, 0xFFF);
    // if this was not the last cluster of the input file,
    // replace the allocation chain mark with
    // the number of the next free cluster.
    if (ff.tellp() < (int) iFileSizeBytes)
      FAT()->putmark (iClusterNr, FAT()->firstfree());


  } while (ff.tellp() < (int) iFileSizeBytes);

  if (!pRootDir->put (pcFilename, iStartClusterNr, iFileSizeBytes))
  {
    cout << "Error: could not write entry in root directory." << endl;
    return (false);
  }

  return (true);
}

/***********************************************************
 *
 * function: CImage::removeFile
 *
 * Purpose:
 * Remove a file to the open image, if it exists.
 *
 * Arguments:
 * pcFilename: name of file to remove.
 * bSilent: if true, do not report errors.
 *
 * Pre: image stream is open.
 *
 * Post: File is removed from the image.
 *
 * Return values:
 * true: file successfully removed.
 * false: file not present in image
 *
 **********************************************************/

bool CImage::removeFile(char *pcFilename, bool bSilent)
{
  int i;
  CRootDirEntry *pEntry;
  unsigned short iCluster;
  unsigned short iNextCluster;

  // Find filename in root directory
  for (i=0; i<ROOTDIRMAXENTRIES; i++)
  {
    pEntry = RootDir()->get (i);
    if (strcmpcase(pEntry->tostring(), pcFilename) == true)
      break;
    delete pEntry;
  }
  if (i == ROOTDIRMAXENTRIES)
  {
    if (!bSilent) cout << pcFilename << ": file not found." << endl;
    return (false);
  }

  if (!bSilent) cout << "Removing FAT marks for " << pcFilename << "." << endl;

  // Remove marks from FAT

  iCluster = pEntry->data.wClusterNr;
  while (iCluster != 0x0FFF)
  {
    iNextCluster = FAT()->getmark (iCluster, false);
    FAT()->putmark (iCluster, 0);
    iCluster = iNextCluster;
  }

  // Clear the entry in the root directory.
  if (!bSilent) cout << "Clearing root directory entry for " << pcFilename << "." << endl;
  RootDir()->clear (i);

  delete pEntry;

  return (true);
}

/***********************************************************
 *
 * function: CImage::dir
 *
 * Purpose:
 * List files in image.
 *
 * Arguments: none
 *
 * Pre: image stream is open.
 *
 * Post:
 * Directory contents are listed.
 *
 **********************************************************/

void CImage::dir()
{
  int i;
  CRootDirEntry *pEntry;

  // loop through root directory entries.
  for (i=0; i<ROOTDIRMAXENTRIES; i++)
  {
    // read entry
    pEntry = RootDir()->get (i);
    // see if entry is empty
    if (pEntry->data.abName[0] == '\0') continue;
    // display file name
    cout << pEntry->tostring() << endl;
    // delete entry created by RootDir()->get()
    delete pEntry;
  }
}


// ACCESSORS

// Return pointer to FAT object
CFat * CImage::FAT()
{
  return (pFat);
}

// Return pointer to RootDir object
CRootDir *CImage::RootDir()
{
  return (pRootDir);
}
