/*=========================================================================
Program:   Albedo
Module:    appInteractor2DTemplate.h
Language:  C++
Date:      $Date: 2019-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2020 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __appInteractor2DTemplate_h
#define __appInteractor2DTemplate_h

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appInteractionDefines.h"

#include "albaEvent.h"
#include "albaInteractorPER.h"
#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkPolyDataMapper.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class albaDeviceButtonsPadMouse;
class albaRWI;

class vtkLineSource;
class vtkCoordinate;
class vtkPolyDataMapper2D;
class vtkActor2D;
class vtkRenderWindow;
class vtkRenderer;
class vtkXYPlotActor;
class vtkTextActor;
class vtkRendererCollection;
class vtkALBATextActorMeter;
class vtkPointSource;
class vtkConeSource;

// Class Name: appInteractor2DTemplate
class APP_INTERACTION_EXPORT appInteractor2DTemplate : public albaInteractorPER
{
public:

	albaTypeMacro(appInteractor2DTemplate, albaInteractorPER);

	virtual void OnEvent(albaEventBase *event);

	/// Measures

	/** Add Measure*/
	void AddMeasure(double point1[3], double point2[3], double point3[3], double point4[3]);
	
	/** Edit Measure*/
	void EditMeasure(int index, double point[3]);

	/** Move Measure*/
	void MoveMeasure(int index, double point[3]);

	/** Reset Measure*/
	void ResetMeasure();

	/*Set Renderer by View needed*/
	void SetRendererByView(albaView * view);
	
	/** Set Color Default*/
	void SetColor(double r, double g, double b);
	/** Set Color Selection*/
	void SetColorSelection(double r, double g, double b);

	void SetMode(int mode) { m_Mode = mode; };

protected:

	appInteractor2DTemplate();
	virtual ~appInteractor2DTemplate();

	// Mouse Events
	virtual void OnLeftButtonDown(albaEventInteraction *e);
	virtual void OnLeftButtonUp(albaEventInteraction *e);
	virtual void OnMove(albaEventInteraction *e);

	void InitRenderer(albaEventInteraction *e);

	void ScreenToWorld(double screen[2], double world[3]);

	/** Add Point*/
	void AddPoint(double point[3]);
	/** Add Line*/
	void AddLine(double point1[3], double point2[3]);
	/** Add Texture*/
	void AddTexture();

	void UpdateLineActor(int index, double * point1, double * point2);
	void UpdatePointActor(int index, double * point);
	void UpdateTexture();

	void FindAndHighlightCurrentPoint(double * pointCoord);
	
	bool IsInBound(double *pos);

	albaDeviceButtonsPadMouse	*m_Mouse;
	vtkRenderer								*m_Renderer;
	albaView									*m_View;
	vtkCoordinate							*m_Coordinate;

	// Persistent Points
	std::vector<vtkPointSource *>				m_PointSourceVector;
	std::vector<vtkPolyDataMapper2D *>	m_PointMapperVector;
	std::vector<vtkActor2D *>						m_PointActorVector;

	// Persistent Lines
	std::vector<vtkLineSource *>				m_LineSourceVector;
	std::vector<vtkPolyDataMapper2D *>	m_LineMapperVector;
	std::vector<vtkActor2D *>						m_LineActorVector;

	// Persistent Quad
	vtkPolyData *m_Quad;
	vtkActor *m_TexturedQuad;

	// Edit Actors
	vtkPointSource				*m_EditPointSource;
	vtkPolyDataMapper2D		*m_EditPointMapper;
	vtkActor2D						*m_EditPointActor;
	
	double m_ColorDefault[3];
	double m_ColorSelection[3];

	int m_CurrentPointIndex;
	int m_TotalPoints;

	double *m_Bounds;
	bool m_IsInBound;
	bool m_ButtonDownInside;

	bool m_ParallelView;

	double m_ResetPoint1[3]; 
	double m_ResetPoint2[3]; 
	double m_ResetPoint3[3]; 
	double m_ResetPoint4[3];
	double m_ResetPoint5[3];

	int m_Mode;

private:
	appInteractor2DTemplate(const appInteractor2DTemplate&);   // Not implemented.
	void operator=(const appInteractor2DTemplate&);  // Not implemented.
	friend class appInteractor2DTemplateTest;
};
#endif

