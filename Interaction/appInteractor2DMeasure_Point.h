/*=========================================================================
Program:   Albedo
Module:    appInteractor2DMeasure_Point.h
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

#ifndef __appInteractor2DMeasure_Point_h
#define __appInteractor2DMeasure_Point_h

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appInteractor2DMeasure.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class vtkPolyDataMapper2D;

// Class Name: appInteractor2DMeasure_Point
class APP_INTERACTION_EXPORT appInteractor2DMeasure_Point : public appInteractor2DMeasure
{
public:

	albaTypeMacro(appInteractor2DMeasure_Point, appInteractor2DMeasure);
	
	void AddMeasure(double *point);

protected:

	appInteractor2DMeasure_Point();
	virtual ~appInteractor2DMeasure_Point();
	
	//void EditMeasure(int index, double *point);
	void DrawMeasure(double * wp);

	void UpdateTextActor(int index, double * point);

	void UpdatePointActor(int index, double * point);

	// Persistent Point
	std::vector<vtkPointSource *>				m_PointSourceVector;
	std::vector<vtkPolyDataMapper2D *>	m_PointMapperVector;
	std::vector<vtkActor2D *>						m_PointActorVector;

private:
	appInteractor2DMeasure_Point(const appInteractor2DMeasure_Point&);   // Not implemented.
	void operator=(const appInteractor2DMeasure_Point&);  // Not implemented.
	friend class appInteractor2DMeasure_PointTest;
};
#endif

