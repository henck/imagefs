/***************************************************************************
                          rootdirentry.cpp  -  description
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

#include "rootdirentry.h"


CRootDirEntry::CRootDirEntry()
{
  memset (&data, 0, sizeof(data));
  memset (&data, 0x020, 11); // fill name and ext with spaces
}

CRootDirEntry::~CRootDirEntry()
{


}


void CRootDirEntry::clear (fstream *theStream)
{
  int i;

  for (i=0; i<sizeof(data); i++)
    theStream->put((unsigned char) 0);
}

void CRootDirEntry::read (fstream *theStream)
{
  int i;
  unsigned char *pData;

  pData = (unsigned char *) &data;
  for (i=0; i<sizeof(data); i++)
  {
    theStream->get (*pData);
    pData++;
  }
}

void CRootDirEntry::write (fstream *theStream)
{
  int i;
  unsigned char *pData;

  pData = (unsigned char *) &data;
  for (i=0; i<sizeof(data); i++)
  {
    theStream->put (*pData);
    pData++;
  }
  theStream->flush();
}


// data is kept in a string local to the object
// and is overwritten on subsequent calls.
char *CRootDirEntry::tostring ()
{
  int i;
  int iPos = 0;

  memset (acFilename, 0, 13);

  // copy base name to pcDest string
  for (i=0; i<8; i++)
  {
    if (data.abName[i] == '\0' || data.abName[i] == ' ')
      break;
    acFilename[iPos++] = data.abName[i];
  }

  // if there is an extension, add a dot to pcDest
  if (data.abExt[0] != '\0' && data.abExt[0] != ' ')
    acFilename[iPos++] = '.';

  // add extension to pcDest string
  for (i=0; i<3; i++)
  {
    if (data.abExt[i] == '\0' || data.abExt[i] == ' ')
      break;
    acFilename[iPos++] = data.abExt[i];
  }

  return (acFilename);
}
