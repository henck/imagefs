/***************************************************************************
                          fat.cpp  -  description
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

#include "fat.h"



CFat::CFat(fstream *theStream)
{
  // Connect CFat object to file stream
  pStream = theStream;
  cout << "Mapping FAT table. " << endl;
}

CFat::~CFat()
{

}

void CFat::clear()
{
  static char pcNullSector[512] = {0};
  int i;

  // Fill sectors of both FATS with zeroes
  pStream->seekg (SECTSIZE);
  for (i=0; i<FATSIZE*2; i++) pStream->write (pcNullSector, 512);

  // putbyte FAT id bytes in both FATS
  putbyte (0,0x0F0);
  putbyte (1,0x0FF);
  putbyte (2,0x0FF);

  // for (i=3; i<512; i++) putbyte (i,(char)0);
  pStream->flush();
}


void CFat::putmark (unsigned int iCluster, unsigned short iValue)
{
  unsigned int iBits;
  unsigned char iByte1;
  unsigned char iByte2;

  // Calculate location of cluster mark in bits
  iBits = iCluster * 12;

  if ((iBits % 8) == 0) // offset at beginning of a byte
  {
    // update this byte and the next
    iByte1 = getbyte (iBits / 8);
    iByte2 = getbyte (iBits / 8 + 1);

    iByte1  = (iValue & 0x0FF);
    iByte2  = (iByte2 & 0x0F0) | (iValue >> 8);

    putbyte (iBits / 8, iByte1);
    putbyte (iBits / 8 + 1, iByte2);
  }
  else // offset at end of a byte
  {
    // update this byte and the previous byte
    iByte1 = getbyte (iBits / 8); // left byte (lsb)
    iByte2 = getbyte (iBits / 8 + 1); // right byte (msb)

    iByte1 = ((iValue & 0x0F) << 4) + (iByte1 & 0x0F);
    iByte2 = (iValue >> 4);

    putbyte (iBits / 8, iByte1);
    putbyte (iBits / 8 + 1, iByte2);
  }
}


unsigned short CFat::getmark (unsigned int iCluster, bool bDebug)
{
  unsigned int iBits;
  unsigned short iResult;

  // Calculate location of cluster mark in bits
  iBits = iCluster * 12;
  if (bDebug) cout << dec << "[" << iCluster << "]" << " ";

  iResult = (getbyte (iBits / 8)) | ((getbyte (iBits / 8 + 1)) << 8);

  if ((iBits % 8) == 0) // offset at beginning of a byte
  {
    // update this byte and the next
    iResult = iResult & 0x0FFF;
    if (bDebug) cout << "+" << hex << iResult << " ";
    return iResult;
  }
  else // offset at end of a byte
  {
    // update this byte and the previous byte
    iResult = (iResult >> 4);
    if (bDebug) cout << "-" << hex << iResult << " ";
    return iResult;
  }
}


// Return total number of clusters
unsigned int CFat::size()
{
  // 3 bytes are reserved, 1.5 bytes used per mark.
  return (unsigned int) (((FATSIZE * SECTSIZE) - 3) / 1.5);
}

// Return number of free clusters
unsigned int CFat::freesize()
{
  unsigned int i;
  int iResult = 0;

  for (i=0; i<size(); i++)
  {
    if (getmark(i,false)==0) iResult++;
  }

  return (iResult);
}

// Return number of first free cluster
// Returns -1 if no free cluster found.
int CFat::firstfree()
{
  unsigned int i;

  for (i=0; i<size(); i++)
  {
    if (getmark(i,false)==0) return (i);
  }

  return (-1);
}


// Private functions

// put byte at iOffset from beginning of FAT
void CFat::putbyte (unsigned int iOffset, unsigned char c)
{
  pStream->seekg (SECTSIZE + iOffset, ios::beg);
  pStream->put (c);
  pStream->seekg (10*SECTSIZE + iOffset, ios::beg);
  pStream->put (c);
  pStream->flush();
}

// get byte at iOffset from beginning of FAT
unsigned char CFat::getbyte (unsigned int iOffset)
{
  unsigned char c;

  pStream->seekg (SECTSIZE + iOffset, ios::beg);
  pStream->get (c);
  return c;
}

