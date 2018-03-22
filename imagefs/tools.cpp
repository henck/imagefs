/***************************************************************************
                          tools.cpp  -  description
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

#include "tools.h"


bool strcmpcase (char *s1, char *s2)
{
  unsigned int i;

  if (strlen(s1) != strlen(s2)) return false;

  for (i=0; i < strlen(s1); i++)
  {
    if (toupper(s1[i]) != toupper (s2[i])) return false;
  }

  return true;
}