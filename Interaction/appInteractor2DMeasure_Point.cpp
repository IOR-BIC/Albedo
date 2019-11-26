/*=========================================================================
Program:   Albedo
Module:    appInteractor2DMeasure_Point.cpp
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

#include "appInteractor2DMeasure_Point.h"
#include "appInteractor2DMeasure.h"
#include "vtkALBATextActorMeter.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkLineSource.h"
#include "vtkPointSource.h"
#include "albaDefines.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkActor2D.h"
#include "vtkProperty2D.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(appInteractor2DMeasure_Point)

//----------------------------------------------------------------------------
appInteractor2DMeasure_Point::appInteractor2DMeasure_Point() : appInteractor2DMeasure()
{
	m_ShowArrow = false;
	m_ShowText = true;
	m_TextSide = -2;
	m_CanEditLine = false;
	m_EditMeasureEnable = false;

	m_MeasureTypeText = "POINT";
}
//----------------------------------------------------------------------------
appInteractor2DMeasure_Point::~appInteractor2DMeasure_Point()
{
	SetAction(ID_NO_ACTION);

	vtkDEL(m_Coordinate);

	m_Renderer->RemoveActor2D(m_EditTextActor);
	m_Renderer->RemoveActor2D(m_EditPointActor);

	// Edit Actor
	vtkDEL(m_EditPointSource);
	vtkDEL(m_EditPointMapper);
	vtkDEL(m_EditPointActor);
	vtkDEL(m_EditTextActor);

	for (int i = 0; i < m_PointActorVector.size(); i++)
	{
		// Points
		m_Renderer->RemoveActor2D(m_PointActorVector[i]);
		vtkDEL(m_PointActorVector[i]);
		vtkDEL(m_PointMapperVector[i]);
		vtkDEL(m_PointSourceVector[i]);

		// Texts
		m_Renderer->RemoveActor2D(m_TextActorVector[i]);
		vtkDEL(m_TextActorVector[i]);
	}

	m_Renderer->GetRenderWindow()->Render();

	m_PointSourceVector.clear();
	m_PointMapperVector.clear();
	m_PointActorVector.clear();

	m_TextActorVector.clear();
}

// RENDERING
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Point::DrawMeasure(double * wp)
{
	// No point has yet been picked
	if (m_AddMeasurePhase_Counter == 0)
	{
		// Initialization
		m_EndMeasure = false;
		m_ActorAdded = false;
		m_Distance = 0.0;

		if (!m_ActorAdded)
		{
			// Add Edit Actors
			m_ActorAdded = true;
			m_Renderer->AddActor2D(m_EditTextActor);
			m_Renderer->AddActor2D(m_EditPointActor);
		}

// 		m_EditPointSource->SetCenter(wp);
// 		m_EditPointSource->Update();

		UpdatePointActor(-1,wp);
		UpdateTextActor(-1, wp);
		m_EditPointSource->Update();

		m_AddMeasurePhase_Counter++;
	}
	// Point has been picked and now is being dragged
	else if (m_AddMeasurePhase_Counter == 1 && m_DraggingLeft)
	{
		UpdatePointActor(-1, wp);
		UpdateTextActor(-1, wp);
	}
	// Finished dragging the point
	else if (m_AddMeasurePhase_Counter == 1)
	{
		double tmpPt[3];
		m_EditPointSource->GetCenter(tmpPt);

			if (m_CurrentLineIndex >= 0)
			{
				//EditMeasure(m_CurrentLineIndex, tmpPt1, tmpPt1);
				//albaEventMacro(albaEvent(this, ID_LINE_CHANGED, m_Distance));
			}
			else
			{
				AddMeasure(tmpPt);
				albaEventMacro(albaEvent(this, ID_LINE_ADDED, m_Distance));
			}

		m_AddMeasurePhase_Counter++;
		m_EndMeasure = true;
	}

	if (m_EndMeasure)
	{
		m_AddMeasurePhase_Counter = 0;

		// Remove Edit Actors
 		m_Renderer->RemoveActor2D(m_EditTextActor);
		m_Renderer->RemoveActor2D(m_EditPointActor);
		m_ActorAdded = false;
	}

	SetAction(ID_ADD_LINE);

	m_Renderer->GetRenderWindow()->Render();
}

//----------------------------------------------------------------------------
void appInteractor2DMeasure_Point::UpdateTextActor(int index, double * point)
{
	albaString ds = "Text";
// 	ds = wxString::Format(_("%.2f° | %.2f"), m_Angle, m_StartTheta);
// 	ds = m_TestText; // TO REMOVE

	double text_pos[3] = { 0,0,0 };

	if (point)
	{
		text_pos[0] = point[0];
		text_pos[1] = point[1];
		text_pos[2] = point[2];
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
void appInteractor2DMeasure_Point::UpdatePointActor(int index, double * point)
{
	if (index > -1)
	{
		m_PointSourceVector[index]->SetCenter(point);
		m_PointSourceVector[index]->Update();

		m_PointMapperVector[index]->SetTransformCoordinate(m_Coordinate);
		m_PointMapperVector[index]->SetInput(m_PointSourceVector[index]->GetOutput());

		m_PointActorVector[index]->SetMapper(m_PointMapperVector[index]);
		m_PointActorVector[index]->GetProperty()->SetPointSize(10.0);

		if (m_LastSelection == index)
			m_PointActorVector[index]->GetProperty()->SetColor(m_ColorSelection[0], m_ColorSelection[1], m_ColorSelection[2]);
		else
			m_PointActorVector[index]->GetProperty()->SetColor(m_ColorDefault[0], m_ColorDefault[1], m_ColorDefault[2]);

		m_PointActorVector[index]->GetProperty()->SetOpacity(m_Opacity);
	}
	else
	{
		m_EditPointSource->SetCenter(point);
		m_EditPointSource->Update();
	}
}


// MEASURE
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Point::AddMeasure(double *point)
{
	// Update Edit Actors
	UpdatePointActor(-1, point);
	UpdateTextActor(-1, point);

	//////////////////////////////////////////////////////////////////////////
	// Add Line
	m_PointSourceVector.push_back(vtkPointSource::New());
	m_PointMapperVector.push_back(vtkPolyDataMapper2D::New());
	m_PointActorVector.push_back(vtkActor2D::New());

	m_Renderer->AddActor2D(m_PointActorVector[m_PointActorVector.size() - 1]);

	UpdatePointActor(m_PointActorVector.size() - 1, point);

	//////////////////////////////////////////////////////////////////////////
	// Add Text
	m_TextActorVector.push_back(vtkALBATextActorMeter::New());
	m_Renderer->AddActor2D(m_TextActorVector[m_TextActorVector.size() - 1]);

	UpdateTextActor(m_TextActorVector.size() - 1, point);

	//////////////////////////////////////////////////////////////////////////

	m_MeasuresCount++;

	albaEventMacro(albaEvent(this, CAMERA_UPDATE));
}