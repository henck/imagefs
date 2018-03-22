/***************************************************************************
                          rootdirentry.h  -  description
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

#ifndef ROOTDIRENTRY_H
#define ROOTDIRENTRY_H


/**
  *@author Alex van Oostenrijk
  */

#include <string.h>
#include "defs.h"
#include <fstream.h>



class CRootDirEntry
{
public:
  CRootDirEntry();
  ~CRootDirEntry();

  void clear(fstream *theStream);
  void read(fstream *theStream);
  void write(fstream *theStream);
  char *tostring();

  struct {
    char abName[8];
    char abExt[3];
    unsigned char bAttr;
    char res[10];
    char rTime[2];
    char rDate[2];
    unsigned short wClusterNr;
    unsigned int iSize;
  } data;
private:
  char acFilename[13];

};






#endif