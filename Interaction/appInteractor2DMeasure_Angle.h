/*=========================================================================
Program:   Albedo
Module:    appInteractor2DMeasure_Angle.h
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

#ifndef __appInteractor2DMeasure_Angle_h
#define __appInteractor2DMeasure_Angle_h

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appInteractor2DMeasure.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class vtkSphereSource;
class vtkCubeSource;
class vtkTransformPolyDataFilter;
class vtkTransform;

// Class Name: appInteractor2DMeasure_Angle
class APP_INTERACTION_EXPORT appInteractor2DMeasure_Angle : public appInteractor2DMeasure
{
public:

	albaTypeMacro(appInteractor2DMeasure_Angle, appInteractor2DMeasure);

	/** Get measure line extremity points*/
	void GetMeasureLinePoints(int index, double *point1, double *point2, double *point3, double *point4);

protected:

	appInteractor2DMeasure_Angle();
	virtual ~appInteractor2DMeasure_Angle();

	void EditMeasure(int index, double *point1, double *point2);
	void DrawMeasure(double * wp);

	void UpdateTextActor(int index, double * point1, double * point2);
	void UpdateSphereActor(double * point);

	void CalculateAngle();

	vtkSphereSource				*m_EditSphereSource;
	vtkPolyDataMapper2D		*m_EditSphereMapper;
	vtkActor2D						*m_EditSphereActor;

	vtkCubeSource					*m_EditCubeSource;
	vtkPolyDataMapper2D		*m_EditCubeMapper;
	vtkActor2D						*m_EditCubeActor;

	vtkTransform *m_Transform;
	vtkTransformPolyDataFilter *m_TransformFilter;

	double m_Point1[3], m_Point2[3], m_Point3[3], m_Point4[3];
	double m_Angle;
	double m_StartTheta;
	double m_StartTheta2;

	albaString m_TestText;

private:
	appInteractor2DMeasure_Angle(const appInteractor2DMeasure_Angle&);   // Not implemented.
	void operator=(const appInteractor2DMeasure_Angle&);  // Not implemented.
	friend class appInteractor2DMeasure_AngleTest;
};
#endif
