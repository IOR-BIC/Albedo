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
	
	// MEASURE
	/** Add Measure*/
	void AddMeasure(double *point1);
	/** Edit Measure*/
	void EditMeasure(int index, double *point1);
	/** Delete the Measure*/
	void RemoveMeasure(int index);
	/** Select a Measure*/
	void SelectMeasure(int index);

	/** Get point coordinate*/
	void GetMeasurePoint(int index, double *point);

protected:

	appInteractor2DMeasure_Point();
	virtual ~appInteractor2DMeasure_Point();
	
	// Draw Measure
	void DrawMeasure(double * wp);
	void MoveMeasure(int index, double * pointCoord);

	// RENDERING
	void UpdatePointActor(int index, double * point);
	void UpdateEditActors(double * point1, double * point2 = NULL);
	void ShowEditActors();
	void HideEditActors();
	void DisableMeasure(int index);

	//UTILS
	void FindAndHighlightCurrentPoint(double * pointCoord);
	
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
