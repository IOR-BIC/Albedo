/*=========================================================================
Program:   Albedo
Module:    appInteractor2DMeasure_Distance.h
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

#ifndef __appInteractor2DMeasure_Distance_h
#define __appInteractor2DMeasure_Distance_h

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appInteractor2DMeasure.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class vtkLineSource;
class vtkPolyDataMapper2D;
class vtkActor2D;

#define POINT_UPDATE_DISTANCE 2.5
#define POINT_UPDATE_DISTANCE_2 (POINT_UPDATE_DISTANCE * POINT_UPDATE_DISTANCE)

// Class Name: appInteractor2DMeasure_Distance
class APP_INTERACTION_EXPORT appInteractor2DMeasure_Distance : public appInteractor2DMeasure
{
public:

	enum LINE_POINT
	{
		NO_POINT = 0,
		POINT_1,
		POINT_2,
		POINT_3,
	};

	albaTypeMacro(appInteractor2DMeasure_Distance, appInteractor2DMeasure);

	// MEASURE
	/** Add Measure*/
	virtual void AddMeasure(double *point1, double *point2);
	/** Edit Measure*/
	virtual void EditMeasure(int index, double *point1, double *point2);
	/** Delete the Measure*/
	virtual void RemoveMeasure(int index);
	/** Select a Measure*/
	virtual void SelectMeasure(int index);

	/** Get measure line extremity points*/
	void GetMeasureLinePoints(int index, double *point1, double *point2);

	/** Show/Hide Measure Line*/
	void ShowLine(bool show);

protected:

	appInteractor2DMeasure_Distance();
	virtual ~appInteractor2DMeasure_Distance();

	// Draw Measure
	void DrawMeasure(double * wp);
	void MoveMeasure(int index, double * point);

	// RENDERING
	virtual void UpdateLineActor(int index, double * point1, double * point2);
	void UpdateTextActor(int index, double * point1, double * point2);
	virtual void UpdateEditActors(double * point1, double * point2);
	virtual void ShowEditActors();
	virtual void HideEditActors();
	virtual void DisableMeasure(int index);
	
	//UTILS
	virtual void FindAndHighlightCurrentPoint(double * point);
	
	// Persistent line
	std::vector<vtkLineSource *>				m_LineSourceVector;
	std::vector<vtkPolyDataMapper2D *>	m_LineMapperVector;
	std::vector<vtkActor2D *>						m_LineActorVector;

	// Edit Actors
	vtkLineSource					*m_EditLineSource;
	vtkPolyDataMapper2D		*m_EditLineMapper;
	vtkActor2D						*m_EditLineActor;

	int m_CurrentPointIndex;
	bool m_ShowLine;

private:
	appInteractor2DMeasure_Distance(const appInteractor2DMeasure_Distance&);   // Not implemented.
	void operator=(const appInteractor2DMeasure_Distance&);  // Not implemented.
	friend class appInteractor2DMeasure_DistanceTest;
};
#endif
