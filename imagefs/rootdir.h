/***************************************************************************
                          rootdir.h  -  description
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

#ifndef ROOTDIR_H
#define ROOTDIR_H


/**
  *@author Alex van Oostenrijk
  */

#include "defs.h"
#include "tools.h"
#include <fstream.h>
#include "rootdirentry.h"

class CRootDir
{
public:
  CRootDir(fstream *theStream);
  ~CRootDir();

  void clear();
  unsigned int freesize();
  void clear (unsigned int iEntry);
  bool put (char *pcFilename, unsigned int iClusterNr, unsigned int iFileSize);
  CRootDirEntry *get (unsigned int iEntry);
  CRootDirEntry *find (char *pcFilename);

private:

  fstream *pStream;
};



#endif