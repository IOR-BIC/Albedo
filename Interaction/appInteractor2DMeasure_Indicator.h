/*=========================================================================
Program:   Albedo
Module:    appInteractor2DMeasure_Indicator.h
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

#ifndef __appInteractor2DMeasure_Indicator_h
#define __appInteractor2DMeasure_Indicator_h

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appInteractor2DMeasure.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------

// Class Name: appInteractor2DMeasure_Indicator
class APP_INTERACTION_EXPORT appInteractor2DMeasure_Indicator : public appInteractor2DMeasure
{
public:

	albaTypeMacro(appInteractor2DMeasure_Indicator, appInteractor2DMeasure);
	
protected:

	appInteractor2DMeasure_Indicator();
	virtual ~appInteractor2DMeasure_Indicator();
	
private:
	appInteractor2DMeasure_Indicator(const appInteractor2DMeasure_Indicator&);   // Not implemented.
	void operator=(const appInteractor2DMeasure_Indicator&);  // Not implemented.
	friend class appInteractor2DMeasure_IndicatorTest;
};
#endif

