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
#include "appInteractor2DMeasure_Distance.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class vtkConeSource;

// Class Name: appInteractor2DMeasure_Indicator
class APP_INTERACTION_EXPORT appInteractor2DMeasure_Indicator : public appInteractor2DMeasure_Distance
{
public:

	albaTypeMacro(appInteractor2DMeasure_Indicator, appInteractor2DMeasure_Distance);

	/** Show/Hide Arrow Indicator*/
	void ShowArrow(bool show);

	void AddMeasure(double *point1, double *point2);
	void EditMeasure(int index, double *point1, double *point2);
	void RemoveMeasure(int index);
	void SelectMeasure(int index);

protected:

	appInteractor2DMeasure_Indicator();
	virtual ~appInteractor2DMeasure_Indicator();

	virtual void UpdateConeActor(int index, double * point1, double * point2);
	virtual void DisableMeasure(int index);

	virtual void ShowEditActors();
	virtual void HideEditActors();
	virtual void UpdateEditActors(double * point1, double * point2);

	std::vector<vtkConeSource *>				m_ConeSourceVector;
	std::vector<vtkPolyDataMapper2D *>	m_ConeMapperVector;
	std::vector<vtkActor2D *>						m_ConeActorVector;

	vtkConeSource					*m_EditConeSource;
	vtkPolyDataMapper2D		*m_EditConeMapper;
	vtkActor2D						*m_EditConeActor;

	bool m_ShowArrow;

private:
	appInteractor2DMeasure_Indicator(const appInteractor2DMeasure_Indicator&);   // Not implemented.
	void operator=(const appInteractor2DMeasure_Indicator&);  // Not implemented.
	friend class appInteractor2DMeasure_IndicatorTest;
};
#endif

