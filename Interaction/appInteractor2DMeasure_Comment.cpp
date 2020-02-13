/*=========================================================================
Program:   Albedo
Module:    appInteractor2DMeasure_Comment.cpp
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

#include "appInteractor2DMeasure_Comment.h"
#include "appInteractor2DMeasure_Point.h"

#include "albaDefines.h"
#include "albaVME.h"

#include "vtkALBATextActorMeter.h"
#include "vtkCamera.h"
#include "vtkPointSource.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkPolygon.h"
#include "vtkProperty2D.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include "vtkTextActor.h"
#include "vtkTextMapper.h"
#include "vtkTextProperty.h"

#define BOX_WIDTH 240
#define BOX_HEIGHT 100
#define TEXT_MARGIN 5
#define MAX_LINES 8
#define MAX_CHARS_IN_LINE 48

//------------------------------------------------------------------------------
albaCxxTypeMacro(appInteractor2DMeasure_Comment)

//----------------------------------------------------------------------------
appInteractor2DMeasure_Comment::appInteractor2DMeasure_Comment() : appInteractor2DMeasure_Point()
{
	m_ShowText = true;
	m_MovingMeasure = false;
	m_EditMeasureEnable = true;

	m_MeasureTypeText = "Comment";

	SetColor(200, 200, 200); // Text Name
	SetColorSelection(250, 200, 200); // Selection
	SetTextColor(0, 0, 0); // Text Comment and Info
	SetRectColor(1, 1, 0.5); // Box Color

	m_OriginalScale = -1;
}
//----------------------------------------------------------------------------
appInteractor2DMeasure_Comment::~appInteractor2DMeasure_Comment()
{
	for (int i = 0; i < m_CommentsVect.size(); i++)
	{
		// Text Comment
		m_Renderer->RemoveActor2D(m_TextCommentActorVector[i]);
		vtkDEL(m_TextCommentActorVector[i]);

		// Text Info
		m_Renderer->RemoveActor2D(m_TextInfoActorVector[i]);
		vtkDEL(m_TextInfoActorVector[i]);

		// Rect
		m_Renderer->RemoveActor2D(m_RectActorVector[i]);
		vtkDEL(m_RectActorVector[i]);
		vtkDEL(m_RectPointsVector[i]);
		vtkDEL(m_RectPolygonVector[i]);
		vtkDEL(m_RectCellArrayVector[i]);
		vtkDEL(m_RectPointMapperVector[i]);
	}

	m_Renderer->GetRenderWindow()->Render();

	m_CommentsVect.clear();

	m_TextCommentActorVector.clear();
	m_TextInfoActorVector.clear();
	m_RectActorVector.clear();
	m_RectPointsVector.clear();
	m_RectPolygonVector.clear();
	m_RectCellArrayVector.clear();
	m_RectPointMapperVector.clear();
}

//----------------------------------------------------------------------------
void appInteractor2DMeasure_Comment::OnEvent(albaEventBase *event)
{
	UpdateAllActors();
	
	Superclass::OnEvent(event);
}

//----------------------------------------------------------------------------
void appInteractor2DMeasure_Comment::SelectMeasure(int index)
{
	Superclass::SelectMeasure(index);

// 	for (int i = 0; i < m_CommentsVect.size(); i++)
// 	{
// 		m_CommentsVect[i].isVisible = false;
// 		UpdateTextActor(i, m_CommentsVect[i].Position);
// 	}
// 
// 	if (index > -1 && index < m_CommentsVect.size())
// 	{
// 		m_CommentsVect[index].isVisible = true;
// 		UpdateTextActor(index, m_CommentsVect[index].Position);
// 	}
}

/// COMMENTS
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Comment::AddComment(albaString name, albaString comment /*= ""*/, albaString author /*= ""*/, albaString date /*= ""*/, double *point /*= NULL*/)
{
	if (m_OriginalScale == -1)
		m_OriginalScale = m_Renderer->GetActiveCamera()->GetParallelScale();

	// New Comment
	CommentData newComment;
	newComment.Name = name;
	newComment.Comment = comment;
	newComment.Author = author;
	newComment.DateTime = date;
	newComment.Width = BOX_WIDTH;
	newComment.Height = BOX_HEIGHT;
	newComment.isVisible = true;

	m_CommentsVect.push_back(newComment);

	int lastIndex = m_CommentsVect.size() - 1;

	double comment_point[3];

	if (point != NULL)
	{
		comment_point[0] = point[0];
		comment_point[1] = point[1];
		comment_point[2] = 0.0;

		AddMeasure(comment_point);
	}
	else
	{
		GetMeasurePoint(lastIndex, comment_point);
	}

	m_CommentsVect[lastIndex].Position[0] = comment_point[0];
	m_CommentsVect[lastIndex].Position[1] = comment_point[1];

	/// ADD Actors
	// Add Rect
	AddRect(comment_point);

	// Text Title
	m_TextActorVector[lastIndex]->GetTextActor()->GetTextProperty()->SetFontFamilyToCourier();
	m_TextActorVector[lastIndex]->GetTextActor()->GetTextProperty()->SetFontSize(12);
	m_TextActorVector[lastIndex]->GetTextActor()->GetTextProperty()->SetBold(1);
	m_TextActorVector[lastIndex]->GetTextActor()->GetTextProperty()->SetJustificationToLeft();
	m_TextActorVector[lastIndex]->GetTextActor()->GetTextProperty()->SetVerticalJustificationToBottom();

	// Add Comment Text
	m_TextCommentActorVector.push_back(vtkALBATextActorMeter::New());
	m_TextCommentActorVector[lastIndex]->GetTextActor()->GetTextProperty()->SetFontSize(12);
	m_TextCommentActorVector[lastIndex]->GetTextActor()->GetTextProperty()->SetFontFamilyToCourier();
	m_TextCommentActorVector[lastIndex]->GetTextActor()->GetTextProperty()->SetJustificationToLeft();
	m_TextCommentActorVector[lastIndex]->GetTextActor()->GetTextProperty()->SetVerticalJustificationToTop();
	m_Renderer->AddActor2D(m_TextCommentActorVector[lastIndex]);

	// Add Author Text
	m_TextInfoActorVector.push_back(vtkALBATextActorMeter::New());
	m_TextInfoActorVector[lastIndex]->GetTextActor()->GetTextProperty()->SetFontSize(10);
	m_TextInfoActorVector[lastIndex]->GetTextActor()->GetTextProperty()->SetFontFamilyToCourier();
	m_TextInfoActorVector[lastIndex]->GetTextActor()->GetTextProperty()->SetJustificationToRight();
	m_TextInfoActorVector[lastIndex]->GetTextActor()->GetTextProperty()->SetVerticalJustificationToBottom();
	m_Renderer->AddActor2D(m_TextInfoActorVector[lastIndex]);

	//////////////////////////////////////////////////////////////////////////

	UpdateTextActor(lastIndex, comment_point);
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Comment::AddRect(double * point)
{
	// Points
	m_RectPointsVector.push_back(vtkPoints::New());
	int index = m_RectPointsVector.size() - 1;

	double position[3] = { 0.0, 0.0, 0.0 };
	position[0] = point[0];
	position[1] = point[1];
	position[2] = point[2];

	m_RectPointsVector[index]->SetNumberOfPoints(4);
	m_RectPointsVector[index]->SetPoint(0, position[0], position[1], position[2]);
	m_RectPointsVector[index]->SetPoint(1, position[0] + BOX_WIDTH, position[1], position[2]);
	m_RectPointsVector[index]->SetPoint(2, position[0] + BOX_WIDTH, position[1] + BOX_HEIGHT, position[2]);
	m_RectPointsVector[index]->SetPoint(3, position[0], position[1] + BOX_HEIGHT, position[2]);

	//////////////////////////////////////////////////////////////////////////
	m_RectPolygonVector.push_back(vtkPolygon::New());

	// Polygon
	m_RectPolygonVector[index]->GetPointIds()->SetNumberOfIds(4); // Make a quad
	m_RectPolygonVector[index]->GetPointIds()->SetId(0, 0);
	m_RectPolygonVector[index]->GetPointIds()->SetId(1, 1);
	m_RectPolygonVector[index]->GetPointIds()->SetId(2, 2);
	m_RectPolygonVector[index]->GetPointIds()->SetId(3, 3);

	// CellArray
	m_RectCellArrayVector.push_back(vtkCellArray::New());
	m_RectCellArrayVector[index]->InsertNextCell(m_RectPolygonVector[index]);

	// PolyData
	m_RectPolyDataVector.push_back(vtkPolyData::New());

	m_RectPolyDataVector[index]->SetPoints(m_RectPointsVector[index]);
	m_RectPolyDataVector[index]->SetPolys(m_RectCellArrayVector[index]);

	// Mapper
	m_RectPointMapperVector.push_back(vtkPolyDataMapper2D::New());

	m_RectPointMapperVector[index]->SetInput(m_RectPolyDataVector[index]);
	m_RectPointMapperVector[index]->SetTransformCoordinate(m_Coordinate);

	// Actor
	m_RectActorVector.push_back(vtkActor2D::New());

	m_RectActorVector[index]->SetMapper(m_RectPointMapperVector[index]);
	m_RectActorVector[index]->GetProperty()->SetPointSize(1.5);
	m_RectActorVector[index]->GetProperty()->SetLineWidth(1.0);
	m_RectActorVector[index]->GetProperty()->SetColor(m_RectColor);

	m_Renderer->AddActor2D(m_RectActorVector[index]);
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Comment::EditComment(int index, albaString name, albaString comment /*= ""*/, albaString author /*= ""*/, albaString date /*= ""*/, double *point /*= NULL*/)
{
	if (index > -1 && index < m_CommentsVect.size())
	{
		m_CommentsVect[index].Name = name;
		m_CommentsVect[index].Comment = comment;
		m_CommentsVect[index].Author = author;
		m_CommentsVect[index].DateTime = date;
		m_CommentsVect[index].isVisible = true;

		double comment_point[3];

		if (point != NULL)
		{
			comment_point[0] = point[0];
			comment_point[1] = point[1];
			comment_point[2] = point[2];
		}
		else
		{
			GetMeasurePoint(index, comment_point);
		}

		m_CommentsVect[index].Position[0] = comment_point[0];
		m_CommentsVect[index].Position[1] = comment_point[1];

		UpdateTextActor(index, comment_point);
	}
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Comment::RemoveComment(int index)
{
	RemoveMeasure(index);

	m_CommentsVect.erase(m_CommentsVect.begin() + index);

	//////////////////////////////////////////////////////////////////////////
	// Text
	m_Renderer->RemoveActor2D(m_TextCommentActorVector[index]);
	vtkDEL(m_TextCommentActorVector[index]);
	m_TextCommentActorVector.erase(m_TextCommentActorVector.begin() + index);

	m_Renderer->RemoveActor2D(m_TextInfoActorVector[index]);
	vtkDEL(m_TextInfoActorVector[index]);
	m_TextInfoActorVector.erase(m_TextInfoActorVector.begin() + index);

	//////////////////////////////////////////////////////////////////////////
	// Rect
	m_Renderer->RemoveActor2D(m_RectActorVector[index]);
	vtkDEL(m_RectActorVector[index]);
	m_RectActorVector.erase(m_RectActorVector.begin() + index);

	vtkDEL(m_RectPointsVector[index]);
	m_RectPointsVector.erase(m_RectPointsVector.begin() + index);
	
	vtkDEL(m_RectPolygonVector[index]);
	m_RectPolygonVector.erase(m_RectPolygonVector.begin() + index);
	
	vtkDEL(m_RectCellArrayVector[index]);
	m_RectCellArrayVector.erase(m_RectCellArrayVector.begin() + index);

	vtkDEL(m_RectPointMapperVector[index]);
	m_RectPointMapperVector.erase(m_RectPointMapperVector.begin() + index);

	//////////////////////////////////////////////////////////////////////////

	m_Renderer->GetRenderWindow()->Render();
	albaEventMacro(albaEvent(this, CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
void appInteractor2DMeasure_Comment::UpdateTextActor(int index, double *point)
{
	double text_pos[3];
	text_pos[0] = point[0];
	text_pos[1] = point[1];
	text_pos[2] = 0.0;
	
	if (index > -1 && index < m_RectPointsVector.size())
	{
		if (m_CommentsVect[index].isVisible)
		{
			double scale = m_OriginalScale / m_Renderer->GetActiveCamera()->GetParallelScale();
			double margin = TEXT_MARGIN / scale;

			// Text /////////////////////////////////////////////
			text_pos[0] += margin;
			text_pos[1] -= margin;
			albaString text = m_CommentsVect[index].Name;

			if (text.Length() > MAX_CHARS_IN_LINE)
			{
				// Truncate text
				wxString txt = text;				
				text = txt.substr(0, MAX_CHARS_IN_LINE - 2);
				text += "..";
			}

			m_TextActorVector[index]->SetText(text);
			m_TextActorVector[index]->SetTextPosition(text_pos);
			m_TextActorVector[index]->Modified();

			// Comment ///////////////////////////////////////////
			albaString comment = m_CommentsVect[index].Comment;
			wxString aux_txt = "";
			int chars_in_line = 0;
			int comment_lines = 1;

			for (int i = 0; i < comment.GetSize(); i++)
			{
				char c = comment[i];

				if (c == '\n')
				{
					comment_lines++;
					if (comment_lines >= MAX_LINES)
						break;
				}

				aux_txt += comment[i];
				chars_in_line++;
				
				if (chars_in_line == MAX_CHARS_IN_LINE && c != ' ')
				{
					aux_txt += "-\n";
					chars_in_line = 0;
					comment_lines++;
				}

				if (chars_in_line >= MAX_CHARS_IN_LINE) // Space or special char
				{
					aux_txt += "\n";
					chars_in_line = 0;
					comment_lines++;
				}

				if (i > (MAX_CHARS_IN_LINE * MAX_LINES) - 4) // Truncate text
				{
					aux_txt += "...";
					break;
				}	
			}

			comment = aux_txt;
			text_pos[1] = point[1] + margin;

			m_TextCommentActorVector[index]->SetText(comment);
			m_TextCommentActorVector[index]->SetTextPosition(text_pos);
			m_TextCommentActorVector[index]->Modified();

			// Rectangle ///////////////////////////////////////////
			m_CommentsVect[index].Height = (BOX_HEIGHT / MAX_LINES)*(comment_lines + 1);

			double rect_size[2];
			rect_size[0] = m_CommentsVect[index].Width / scale;
			rect_size[1] = m_CommentsVect[index].Height / scale;

			m_RectPointsVector[index]->SetPoint(0, point[0], point[1], point[2]);
			m_RectPointsVector[index]->SetPoint(1, point[0] + m_CommentsVect[index].Width / scale, point[1], point[2]);
			m_RectPointsVector[index]->SetPoint(2, point[0] + m_CommentsVect[index].Width / scale, point[1] + m_CommentsVect[index].Height / scale, point[2]);
			m_RectPointsVector[index]->SetPoint(3, point[0], point[1] + m_CommentsVect[index].Height / scale, point[2]);
			m_RectPointsVector[index]->Modified();

			// Author - Date ///////////////////////////////////////////
			text_pos[0] = point[0] + rect_size[0] - margin;
			text_pos[1] = point[1] + rect_size[1] - margin;

			albaString author_date = m_CommentsVect[index].Author + " " + m_CommentsVect[index].DateTime;
			m_TextInfoActorVector[index]->SetText(author_date);
			m_TextInfoActorVector[index]->SetTextPosition(text_pos);
			m_TextInfoActorVector[index]->Modified();

      //////////////////////////////////////////////////////////////////////////
			// Update Color
			if (m_LastSelection == index)
			{
				m_TextActorVector[index]->SetColor(m_ColorSelection[0], m_ColorSelection[1], m_ColorSelection[2]);
			}
			else
			{
				m_TextActorVector[index]->SetColor(m_ColorDefault[0], m_ColorDefault[1], m_ColorDefault[2]);
			}

			m_TextCommentActorVector[index]->SetColor(m_TextColor[0], m_TextColor[1], m_TextColor[2]);
			m_TextInfoActorVector[index]->SetColor(m_TextColor[0], m_TextColor[1], m_TextColor[2]);

			// Update Opacity
			m_TextActorVector[index]->SetOpacity(m_Opacity);
			m_TextCommentActorVector[index]->SetOpacity(m_Opacity);
			m_TextInfoActorVector[index]->SetOpacity(m_Opacity);
			m_RectActorVector[index]->GetProperty()->SetOpacity(m_Opacity);
		}

		// Update Visibility
		m_TextActorVector[index]->SetVisibility(m_ShowText);
		m_TextCommentActorVector[index]->SetVisibility(m_ShowText && m_CommentsVect[index].isVisible);
		m_TextInfoActorVector[index]->SetVisibility(m_ShowText && m_CommentsVect[index].isVisible);
		m_RectActorVector[index]->SetVisibility(m_CommentsVect[index].isVisible);
	}
	else
	{
		if (m_LastSelection > 0)
			m_MeasureLabel = m_CommentsVect[m_LastSelection].Name;

		// Update Edit Actor
		m_EditTextActor->SetText(m_MeasureLabel);
		m_EditTextActor->SetTextPosition(text_pos);
		m_EditTextActor->SetVisibility(m_ShowText);
	}
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Comment::UpdateAllActors()
{
	for (int i = 0; i < m_CommentsVect.size(); i++)
	{
		double point[3];
		GetMeasurePoint(i, point);
		m_CommentsVect[i].Position[0] = point[0];
		m_CommentsVect[i].Position[1] = point[1];
		UpdateTextActor(i, m_CommentsVect[i].Position);
	}

	if (m_Renderer)
		m_Renderer->GetRenderWindow()->Render();

	albaEventMacro(albaEvent(this, CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
void appInteractor2DMeasure_Comment::ShowComment(int index, bool show)
{
	m_CommentsVect[index].isVisible = show;

	UpdateAllActors();
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Comment::SetTextColor(double r, double g, double b)
{
	m_TextColor[0] = r;
	m_TextColor[1] = g;
	m_TextColor[2] = b;
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Comment::SetRectColor(double r, double g, double b)
{
	m_RectColor[0] = r;
	m_RectColor[1] = g;
	m_RectColor[2] = b;
}

//----------------------------------------------------------------------------
void appInteractor2DMeasure_Comment::FindAndHighlightCurrentPoint(double * pointCoord)
{
	if (m_CurrentMeasureIndex < 0)
		SetAction(ID_ADD_MEASURE);

	if (m_EditMeasureEnable)
	{
		for (int i = 0; i < m_MeasuresCount; i++)
		{
			if (IsInBound(i, pointCoord))
			{
				SelectMeasure(i);

				m_CurrentMeasureIndex = i;

				m_Renderer->AddActor2D(m_EditPointActor);
				UpdatePointActor(-1, m_CommentsVect[i].Position);
				m_Renderer->GetRenderWindow()->Render();

				SetAction(ID_MOVE_MEASURE);
				return;
			}
		}

		if (m_CurrentMeasureIndex >= 0)
		{
			m_Renderer->RemoveActor2D(m_EditPointActor);
			m_Renderer->GetRenderWindow()->Render();
			m_CurrentMeasureIndex = -1;
		}
	}
}
//----------------------------------------------------------------------------
bool appInteractor2DMeasure_Comment::IsInBound(int index, double * pointCoord)
{
	if (pointCoord[0] >= m_RectPointsVector[index]->GetPoint(0)[0] && pointCoord[0] <= m_RectPointsVector[index]->GetPoint(1)[0] &&
		pointCoord[1] >= m_RectPointsVector[index]->GetPoint(0)[1] && pointCoord[1] <= m_RectPointsVector[index]->GetPoint(2)[1])
		return true;

	return false;
}