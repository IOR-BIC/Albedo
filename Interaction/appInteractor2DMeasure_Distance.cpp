/*=========================================================================
Program:   Albedo
Module:    appInteractor2DMeasure_Distance.cpp
Language:  C++
Date:      $Date: 2019-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2019 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "appInteractor2DMeasure_Distance.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(appInteractor2DMeasure_Distance)

//----------------------------------------------------------------------------
appInteractor2DMeasure_Distance::appInteractor2DMeasure_Distance() : appInteractor2DMeasure()
{
	m_ShowArrow = false;
	m_ShowText = true;
	m_TextSide = 1;

	m_MeasureTypeText = "DISTANCE";
}
//----------------------------------------------------------------------------
appInteractor2DMeasure_Distance::~appInteractor2DMeasure_Distance()
{
}