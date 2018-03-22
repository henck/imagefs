/***************************************************************************
                          rootdir.cpp  -  description
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

#include "rootdir.h"


CRootDir::CRootDir(fstream *theStream)
{
  // Connect CRootDir object to file stream
  pStream = theStream;
}

CRootDir::~CRootDir()
{

}


void CRootDir::clear()
{
  static char pcNullSector[512] = {0};
  int i;

  // Write ROOTDIRSIZE null-sectors to root dir
  pStream->seekg ((FATSIZE*2+BOOTSIZE) * SECTSIZE);
  for (i=0; i<ROOTDIRSIZE; i++) pStream->write (pcNullSector, 512);
  pStream->flush();
}

// Return number of free directory entries
unsigned int CRootDir::freesize()
{
  int i;
  int iOldPos;
  int iResult = 0;
  unsigned char c;

  iOldPos = pStream->tellp();

  for (i=0; i<(ROOTDIRSIZE*SECTSIZE) / ROOTDIRENTRYSIZE; i++)
  {
    pStream->seekg ((FATSIZE * 2 + BOOTSIZE) * SECTSIZE + i * ROOTDIRENTRYSIZE, ios::beg);
    pStream->get (c);
    if (c == '\0') iResult++;
  }

  pStream->seekg (iOldPos);

  return iResult;
}


// Returns false if root directory is full.
bool CRootDir::put (char *pcFilename, unsigned int iClusterNr, unsigned int iFileSize)
{
  CRootDirEntry entry;
  int iEntry = -1;
  unsigned int i;
  unsigned int j;
  int iOldPos;
  unsigned char c;

  // find spot
  for (i=0; i<(ROOTDIRSIZE*SECTSIZE) / ROOTDIRENTRYSIZE; i++)
  {
    pStream->seekg ((FATSIZE * 2 + BOOTSIZE) * SECTSIZE + i * ROOTDIRENTRYSIZE, ios::beg);
    pStream->get (c);
    if (c == '\0')
    {
      iEntry = i;
      break;
    }
  }
  if (iEntry == -1) return false;

  // Fill entry structure
  for (i=0; i<strlen(pcFilename); i++)
  {
    if (pcFilename[i] == '.')
    {
      for (j=i+1; j<strlen(pcFilename); j++)
      {
        entry.data.abExt[j-i-1] = toupper(pcFilename[j]);
      }
      break;
    }
    entry.data.abName[i] = toupper(pcFilename[i]);
  }
  entry.data.bAttr = 0x20; // archive bit on
  entry.data.wClusterNr = iClusterNr;
  entry.data.iSize = iFileSize;
  // TODO fill time and date fields

  // write new entry
  iOldPos = pStream->tellp();
  pStream->seekg ((FATSIZE * 2 + BOOTSIZE) * SECTSIZE
                   + iEntry * ROOTDIRENTRYSIZE, ios::beg);
  entry.clear (pStream);
  pStream->seekg ((FATSIZE * 2 + BOOTSIZE) * SECTSIZE
                   + iEntry * ROOTDIRENTRYSIZE, ios::beg);
  entry.write (pStream);
  pStream->seekg (iOldPos);

  return true;
}

void CRootDir::clear (unsigned int iEntry)
{
  CRootDirEntry *pEntry;
  int iOldPos;

  pEntry = new CRootDirEntry ();
  iOldPos = pStream->tellp();
  pStream->seekg ((FATSIZE * 2 + BOOTSIZE) * SECTSIZE
                   + iEntry * ROOTDIRENTRYSIZE, ios::beg);
  pEntry->clear (pStream);
  pStream->seekg (iOldPos);
}


CRootDirEntry *CRootDir::get (unsigned int iEntry)
{
  CRootDirEntry *pEntry;
  int iOldPos;

  pEntry = new CRootDirEntry ();
  iOldPos = pStream->tellp();
  pStream->seekg ((FATSIZE * 2 + BOOTSIZE) * SECTSIZE
                   + iEntry * ROOTDIRENTRYSIZE, ios::beg);
  pEntry->read (pStream);
  pStream->seekg (iOldPos);

  return (pEntry);
}


CRootDirEntry *CRootDir::find (char *pcFilename)
{
  int i;
  CRootDirEntry *pEntry;

  // Find filename in root directory
  for (i=0; i<ROOTDIRMAXENTRIES; i++)
  {
    pEntry = get (i);
    if (strcmpcase(pEntry->tostring(), pcFilename) == true)
      return (pEntry);
    delete pEntry;
  }
  return (NULL);
}
