/***************************************************************************
                          fat.h  -  description
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

#ifndef FAT_H
#define FAT_H


/**
  *@author Alex van Oostenrijk
  */

#include "defs.h"
#include <fstream.h>

class CFat
{
public:
  CFat (fstream *theStream);
  ~CFat();

  void clear();
  void putmark (unsigned int iCluster, unsigned short iValue);
  unsigned short getmark (unsigned int iCluster, bool bDebug);

  unsigned int size();
  unsigned int freesize();
  int firstfree();

private:
  void putbyte (unsigned int iOffset, unsigned char c);
  unsigned char getbyte (unsigned int iOffset);

  fstream *pStream;
};


#endif