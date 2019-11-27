/*=========================================================================
Program:   Albedo
Module:    appInteractor2DMeasure_Angle.cpp
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

#include "appInteractor2DMeasure_Angle.h"

#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkLineSource.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkALBATextActorMeter.h"
#include "vtkMath.h"
#include "vtkActor2D.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkPolyDataSource.h"
#include "vtkSphereSource.h"
#include "vtkCubeSource.h"
#include "vtkActor2D.h"
#include "vtkProperty2D.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkSmartPointer.h"
#include "vtkTransform.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(appInteractor2DMeasure_Angle)

//----------------------------------------------------------------------------
appInteractor2DMeasure_Angle::appInteractor2DMeasure_Angle() : appInteractor2DMeasure()
{
	m_ShowText = true;
	m_TextSide = 1;
	m_CanEdit = false;

	// Angle tool
	vtkNEW(m_EditSphereSource);
	vtkNEW(m_EditSphereMapper);
	m_EditSphereMapper->SetInput(m_EditSphereSource->GetOutput());
	m_EditSphereMapper->SetTransformCoordinate(m_Coordinate);

	vtkNEW(m_EditSphereActor);
	m_EditSphereActor->SetMapper(m_EditSphereMapper);
	m_EditSphereActor->GetProperty()->SetColor(0.5,0.1,0.7);
	m_EditSphereActor->GetProperty()->SetOpacity(0.2);
	m_EditSphereActor->GetProperty()->SetLineWidth(1.0);


	// Cube  tool (Remove)
	vtkNEW(m_EditCubeSource);
	vtkNEW(m_EditCubeMapper);
	m_EditCubeMapper->SetInput(m_EditCubeSource->GetOutput());
	m_EditCubeMapper->SetTransformCoordinate(m_Coordinate);

	vtkNEW(m_EditCubeActor);
	m_EditCubeActor->SetMapper(m_EditCubeMapper);
	m_EditCubeActor->GetProperty()->SetColor(0.7, 0.3, 0.5);
	m_EditCubeActor->GetProperty()->SetOpacity(0.2);
	m_EditCubeActor->GetProperty()->SetLineWidth(1.0);

	m_Transform = vtkTransform::New();
	m_TransformFilter = vtkTransformPolyDataFilter::New();

	m_Angle = 0.0;

	m_MeasureTypeText = "ANGLE";
}
//----------------------------------------------------------------------------
appInteractor2DMeasure_Angle::~appInteractor2DMeasure_Angle()
{
	m_Renderer->RemoveActor2D(m_EditSphereActor);

	vtkDEL(m_EditSphereSource);
	vtkDEL(m_EditSphereMapper);
	vtkDEL(m_EditSphereActor);

	vtkDEL(m_TransformFilter);
	vtkDEL(m_Transform);
}

//----------------------------------------------------------------------------
void appInteractor2DMeasure_Angle::EditMeasure(int index, double *point1, double *point2)
{
	if (index < 0 || index >= m_MeasuresCount)
		return;

	m_CanEdit = true;

	point1[2] = point2[2] = 0;

	m_LastEditing = index;

	//////////////////////////////////////////////////////////////////////////

	int index2 = (2 * (index / 2)) + (index % 2);

	// Edit Actors
	UpdateLineActor(-1, point1, point2);
	UpdateTextActor(-1, point1, point2);
	//UpdateConeActor(-1, point1, point2);

	// Distance
	m_Distance = DistanceBetweenPoints(point1, point2);

	GetMeasureLinePoints(index, m_Point1, m_Point2, m_Point3, m_Point4);

	if (index == index2)
	{
		m_Point1[0] = point1[0];
		m_Point1[1] = point1[1];
		m_Point1[2] = point1[2];

		m_Point2[0] = point2[0];
		m_Point2[1] = point2[1];
		m_Point2[2] = point2[2];
	}
	else
	{
		m_Point3[0] = point1[0];
		m_Point3[1] = point1[1];
		m_Point3[2] = point1[2];

		m_Point4[0] = point2[0];
		m_Point4[1] = point2[1];
		m_Point4[2] = point2[2];
	}

	CalculateAngle();

	// Line
	UpdateLineActor(index, m_Point1, m_Point2);

	// Text
	UpdateTextActor(index, m_Point1, m_Point2);
	//UpdateTextActor(index2, point1, point2);

	// Arrow
	//UpdateConeActor(index, point1, point2);

	//////////////////////////////////////////////////////////////////////////

	albaEventMacro(albaEvent(this, CAMERA_UPDATE));
}

// RENDERING
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Angle::DrawMeasure(double * wp)
{
	// No point has yet been picked
	if (m_AddMeasurePhase_Counter == 0)
	{
		// Initialization
		m_EndMeasure = false;
		m_ActorAdded = false;
		m_Distance = 0.0;
		m_Angle = 0.0;
		m_EditLineSource->SetPoint1(wp);

		// Edit Line
		if (m_CurrentMeasureIndex >= 0)
		{
			// if m_CurrentPointIndex == 0 // common point not edit // TODO
			double tmp_point[3];
			if (m_CurrentPointIndex == 1)
			{
				m_LineSourceVector[m_CurrentMeasureIndex]->GetPoint2(tmp_point);
				m_EditLineSource->SetPoint1(tmp_point);
			}
			else if (m_CurrentPointIndex == 2)
			{
				m_LineSourceVector[m_CurrentMeasureIndex]->GetPoint1(tmp_point);
				m_EditLineSource->SetPoint1(tmp_point);
			}
		}

		m_EditLineSource->SetPoint2(wp);
		m_EditLineSource->Update();

		m_AddMeasurePhase_Counter++;
	}
	// First point has been picked and the second one is being dragged
	else if (m_AddMeasurePhase_Counter == 1 && m_DraggingLeft)
	{
		if (!m_ActorAdded)
		{
			m_ActorAdded = true;
			m_Renderer->AddActor2D(m_EditLineActor);
			m_Renderer->AddActor2D(m_EditTextActor);
			m_Renderer->AddActor2D(m_EditSphereActor);
			m_Renderer->AddActor2D(m_EditCubeActor);
		}

		m_EditLineSource->GetPoint1(m_Point1); // Store Point1

		UpdateLineActor(-1, m_Point1, wp);

		m_Distance = DistanceBetweenPoints(m_Point1, wp);
	}
	// Finished dragging the second point
	else if (m_AddMeasurePhase_Counter == 1)
	{
		if (m_Distance > 0.0)
		{
			m_EditLineSource->GetPoint2(m_Point2); // Store Point2

			if (m_CurrentMeasureIndex >= 0)
			{
				EditMeasure(m_CurrentMeasureIndex, m_Point1, m_Point2);
				m_AddMeasurePhase_Counter = 2;
				m_EndMeasure = true;
			}
			else
			{
				AddMeasure(m_Point1, m_Point2);
			}
		}
		else
		{
			SelectMeasure(m_LastSelection);
		}

		m_AddMeasurePhase_Counter++;
	}
	// Start dragging the third point
	else if (m_AddMeasurePhase_Counter == 2 && m_DraggingLeft)
	{
		m_EditLineSource->GetPoint1(m_Point3); // Store Point3
		UpdateLineActor(-1, m_Point3, wp);

		m_Point4[0] = wp[0];
		m_Point4[1] = wp[1];
		m_Point4[2] = wp[2]; // Set Point4 for UpdateTextActor Position

		CalculateAngle();

		UpdateTextActor(-1, NULL, NULL);
		//UpdateConeActor(-1, m_Point3, wp);
		UpdateSphereActor(m_Point1);
		UpdatePointActor(wp);
	}
	// Finished dragging the third point
	else if (m_AddMeasurePhase_Counter == 2)
	{
		m_Distance = DistanceBetweenPoints(m_Point3, m_Point4);

		m_AddMeasurePhase_Counter++;
		m_EndMeasure = true;
	}

	if (m_EndMeasure)
	{
		m_AddMeasurePhase_Counter = 0;

		// Delete temporary Line and Text
		m_Renderer->RemoveActor2D(m_EditLineActor);
		m_Renderer->RemoveActor2D(m_EditTextActor);
		m_Renderer->RemoveActor2D(m_EditSphereActor);
		m_Renderer->RemoveActor2D(m_EditCubeActor);

		if (m_Distance > 0.0)
		{
			m_EditLineSource->GetPoint2(m_Point4); // Store Point4

			if (m_CurrentMeasureIndex >= 0)
			{
				EditMeasure(m_CurrentMeasureIndex, m_Point3, m_Point4);
				//albaEventMacro(albaEvent(this, ID_LINE_CHANGED, m_Distance));
			}
			else
			{
				AddMeasure(m_Point3, m_Point4);
				CalculateAngle();
				UpdateTextActor(m_LineSourceVector.size() - 1, NULL, NULL);
				UpdateSphereActor(m_Point1);
			}
		}
		else
		{
			SelectMeasure(m_LastSelection);
		}

		m_ActorAdded = false;
	}

	SetAction(ID_ADD_MEASURE);

	m_Renderer->GetRenderWindow()->Render();
}

//----------------------------------------------------------------------------
void appInteractor2DMeasure_Angle::UpdateTextActor(int index, double * point1, double * point2)
{
	albaString ds = "Text";
	ds = wxString::Format(_("%.2f° | %.2f"), m_Angle, m_StartTheta);

	ds = m_TestText; // TO REMOVE

	double text_pos[3];
	if (point1 && point2)
	{
		text_pos[0] = (point1[0] + point2[0]) / 2;
		text_pos[1] = (point1[1] + point2[1]) / 2;
		text_pos[2] = (point1[2] + point2[2]) / 2;
	}
	else
	{
		text_pos[0] = (m_Point1[0] + m_Point2[0] + m_Point3[0] + m_Point4[0]) / 4;
		text_pos[1] = (m_Point1[1] + m_Point2[1] + m_Point3[1] + m_Point4[1]) / 4;
		text_pos[2] = (m_Point1[2] + m_Point2[2] + m_Point3[2] + m_Point4[2]) / 4;
	}

	text_pos[0] -= m_TextSide *TEXT_H_SHIFT;

	if (index > -1)
	{
		m_TextActorVector[index]->SetText(ds);
		m_TextActorVector[index]->SetTextPosition(text_pos);

		if (m_LastSelection == index)
			m_TextActorVector[index]->SetColor(m_ColorSelection[0], m_ColorSelection[1], m_ColorSelection[2]);
		else
			m_TextActorVector[index]->SetColor(m_ColorDefault[0], m_ColorDefault[1], m_ColorDefault[2]);

		m_TextActorVector[index]->SetOpacity(m_Opacity);
		m_TextActorVector[index]->SetVisibility(m_ShowText && (index % 2) == 1);
	}
	else
	{
		m_EditTextActor->SetText(ds);
		m_EditTextActor->SetTextPosition(text_pos);
		m_EditTextActor->SetVisibility(m_ShowText);
	}
}

//----------------------------------------------------------------------------
void appInteractor2DMeasure_Angle::UpdateSphereActor(double * point)
{
	m_EditSphereSource->SetCenter(point);
	m_EditSphereSource->SetRadius(m_Distance / 3);

	m_StartTheta = 0;

	//////////////////////////////////////////////////////////////////////////

	double v1[3], v2[3];
	v1[0] = m_Point2[0] - m_Point1[0];
	v1[1] = m_Point2[1] - m_Point1[1];
	v1[2] = m_Point2[2] - m_Point1[2];

	v2[0] = m_Point4[0] - m_Point3[0];
	v2[1] = m_Point4[1] - m_Point3[1];
	v2[2] = m_Point4[2] - m_Point3[2];

	double a1 = atan2(v1[1], v1[0]);
	double a2 = atan2(v2[1], v2[0]);

	a1 *= vtkMath::RadiansToDegrees();
	a2 *= vtkMath::RadiansToDegrees();


	double b1 = a1, b2 = a2;

 	if (a1 < 0.0)b1 = 360 + a1;
 	if (a2 < 0.0)b2 = 360 + a2;
	
	double result = (a2 - a1);
	if (result < 0.0) result += 360;

	m_TestText = wxString::Format(_("%.2f° \n(%.2f | %.2f)\t(%.2f )"), m_Angle, a1, a2, result);
	
	if (result <= 180.0) 
	{
		m_EditSphereSource->SetStartTheta(b1);
		m_EditSphereSource->SetEndTheta(b2);
	}
	else
	{
		m_EditSphereSource->SetStartTheta(b2);
		m_EditSphereSource->SetEndTheta(b1);
	}

	if (m_Angle <= 91.0 && m_Angle >= 89.0)
	{
// 		m_EditSphereSource->SetPhiResolution(1);
// 		m_EditSphereSource->SetThetaResolution(1);

		m_EditCubeActor->VisibilityOn();
		m_EditCubeSource->SetXLength(m_Distance / 3);
		m_EditCubeSource->SetYLength(m_Distance / 3);
		m_EditCubeSource->SetCenter(m_Point1);
		m_EditCubeSource->Update();

		//vtkALBASmartPointer<vtkTransform> transform;

		m_Transform->Identity();
		m_Transform->PostMultiply();
		m_Transform->RotateZ(90);
		m_Transform->Translate(10, 10, 0);
		m_Transform->Update();
		
		//vtkALBASmartPointer<vtkTransformPolyDataFilter> transformFilter;
		m_TransformFilter->SetTransform(m_Transform);
		m_TransformFilter->SetInput(m_EditCubeSource->GetOutput());
		m_TransformFilter->Update();

		//m_EditCubeSource->SetOutput(transformFilter->GetOutput());
	}
	else
	{
// 		m_EditSphereSource->SetPhiResolution(8);
// 		m_EditSphereSource->SetThetaResolution(360);
		m_EditCubeActor->VisibilityOff();
	}

	m_EditSphereSource->Update();
}

//----------------------------------------------------------------------------
void appInteractor2DMeasure_Angle::CalculateAngle()
{
	double angle, v1[3], vn1, v2[3], vn2, s;
	v1[0] = m_Point2[0] - m_Point1[0];
	v1[1] = m_Point2[1] - m_Point1[1];
	v1[2] = m_Point2[2] - m_Point1[2];
	v2[0] = m_Point4[0] - m_Point3[0];
	v2[1] = m_Point4[1] - m_Point3[1];
	v2[2] = m_Point4[2] - m_Point3[2];

	vn1 = vtkMath::Norm(v1);
	vn2 = vtkMath::Norm(v2);
	s = vtkMath::Dot(v1, v2);

	if (vn1 != 0 && vn2 != 0)
		angle = acos(s / (vn1 * vn2));
	else
	{
		angle = 0;
	}

	angle *= vtkMath::RadiansToDegrees();
	//m_AbsoluteAngle = angle;
	//if(angle >= 90.0 && m_MeasureType == ANGLE_BETWEEN_LINES) 
	//  angle = 180.0 - angle; 

	m_Angle = angle;

	albaEventMacro(albaEvent(this, ID_RESULT_LINE, m_Distance));
}

//---------------------------------------------------------------------------
void appInteractor2DMeasure_Angle::GetMeasureLinePoints(int index, double *point1, double *point2, double *point3, double *point4)
{
	int index2 = (2 * (index / 2)) + (index % 2);

	// Return first line points values
	if (index >= 0 && index < m_LineSourceVector.size())
	{
		m_LineSourceVector[index]->GetPoint1(point1);
		m_LineSourceVector[index]->GetPoint2(point2);
	}

	// Return second line points values
	if (index2 >= 0 && index2 < m_LineSourceVector.size())
	{
		m_LineSourceVector[index2]->GetPoint1(point3);
		m_LineSourceVector[index2]->GetPoint2(point4);
	}
}