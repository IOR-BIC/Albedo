/*=========================================================================
Program:   Albedo
Module:    appUtils.h
Language:  C++
Date:      $Date: 2018-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) LTM-IOR 2018 (http://www.ltmsoftware.org/alba.htm)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "appCommonDefines.h"

#ifndef __appUtils_H__
#define __appUtils_H__

//----------------------------------------------------------------------------
// Class Name: appUtils
//----------------------------------------------------------------------------
class APP_COMMON_EXPORT appUtils
{
public:

  /** In DEBUG mode return Application source code directory in WIN32 path format. 
  In RELEASE mode it returns the Application installation directory in WIN32 path format (same as mafGetApplicationDirectory)*/

  static wxString GetApplicationDirectory();
	static wxString GetConfigDirectory();
};
#endif