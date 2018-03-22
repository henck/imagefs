/***************************************************************************
                          image.h  -  description
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

#ifndef IMAGE_H
#define IMAGE_H


/**
  *@author Alex van Oostenrijk
  */

#include <fstream.h>
#include "string.h"
#include "tools.h"
#include "defs.h"
#include "fat.h"
#include "rootdir.h"


class CImage
{
public:
  CImage(char *pcFileName, bool bCreate);
  ~CImage();

  bool format (int iNumberOfSectors, char cFillChar);
  bool writeBootSector (char *pcFileName);
  bool addFile (char *pcFileName);
  bool removeFile(char *pcFilename, bool bSilent);
  void dir();

  CFat *FAT();
  CRootDir *RootDir();

private:
  CFat *pFat;
  CRootDir * pRootDir;
  fstream fStream;         // r/w stream to image file
  unsigned int iSize;      // image size in sectors
};



#endif
