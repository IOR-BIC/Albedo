/*=========================================================================
Program:   Albedo
Module:    appInteractor2DMeasure_DistancePoint.h
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

#ifndef __appInteractor2DMeasure_DistancePoint_h
#define __appInteractor2DMeasure_DistancePoint_h

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appInteractor2DMeasure.h"
#include "appInteractor2DMeasure_Point.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class vtkLineSource;
class vtkPolyDataMapper2D;
class vtkActor2D;

#define POINT_UPDATE_DISTANCE 2.5
#define POINT_UPDATE_DISTANCE_2 (POINT_UPDATE_DISTANCE * POINT_UPDATE_DISTANCE)

// Class Name: appInteractor2DMeasure_DistancePoint
class APP_INTERACTION_EXPORT appInteractor2DMeasure_DistancePoint : public appInteractor2DMeasure_Point
{
public:

	enum LINE_POINT
	{
		NO_POINT = 0,
		POINT_1,
		POINT_2,
		POINT_3,
	};

	albaTypeMacro(appInteractor2DMeasure_DistancePoint, appInteractor2DMeasure_Point);

	/// MEASURE
	/** Add Measure*/
	virtual void AddMeasure(double *point);
	/** Edit Measure*/
	virtual void EditMeasure(int index, double *point);
	/** Delete the Measure*/
	virtual void RemoveMeasure(int index);
	/** Select a Measure*/
	virtual void SelectMeasure(int index);

	/** Get measure line extremity points*/
	void GetMeasureLinePoints(int index, double *point1, double *point2);

	/** Show/Hide Measure Line*/
	void ShowLine(bool show);

// 	bool Load(albaVME *input, wxString tag);
// 	bool Save(albaVME *input, wxString tag);

protected:

	appInteractor2DMeasure_DistancePoint();
	virtual ~appInteractor2DMeasure_DistancePoint();

	// Draw Measure
// 	void DrawMeasure(double * wp);
// 	void MoveMeasure(int index, double * point);

	/// RENDERING
	virtual void UpdateLineActor(int index, double * point1, double * point2);
	//virtual void UpdateTextActor(int index, double * point1, double * point2);
	virtual void UpdateEditActors(double * point1, double * point2);
	
	virtual void ShowEditActors();
	virtual void HideEditActors();
	//virtual void DisableMeasure(int index);
	
	int GetGroupByPoint(int index);
	int GetGroupByLine(int index);

	///UTILS
	//virtual void FindAndHighlightCurrentPoint(double * point);
	
	struct Group
	{
		std::vector<int> IndexPointVect;
		std::vector<int> IndexLineVect;
	};

	std::vector<Group> m_LinePointsVector;
	int m_Group;

	// Persistent line
	std::vector<vtkLineSource *>				m_LineSourceVector;
	std::vector<vtkPolyDataMapper2D *>	m_LineMapperVector;
	std::vector<vtkActor2D *>						m_LineActorVector;

	// Edit Actors
	vtkLineSource					*m_EditLineSource;
	vtkPolyDataMapper2D		*m_EditLineMapper;
	vtkActor2D						*m_EditLineActor;

	double m_LastPointAdded[3];
	double m_LastPointEdited[3];

	int m_CurrentPointIndex;
	bool m_ShowLine;

private:
	appInteractor2DMeasure_DistancePoint(const appInteractor2DMeasure_DistancePoint&);   // Not implemented.
	void operator=(const appInteractor2DMeasure_DistancePoint&);  // Not implemented.
	friend class appInteractor2DMeasure_DistancePointTest;
};
#endif
