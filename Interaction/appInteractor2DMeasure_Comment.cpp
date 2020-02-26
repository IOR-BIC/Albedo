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
#include <iterator>

#define BOX_WIDTH 155
#define BOX_HEIGHT 100
#define TEXT_MARGIN 5
#define MAX_LINES 8
#define MAX_CHARS_IN_LINE 30

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
}
//----------------------------------------------------------------------------
appInteractor2DMeasure_Comment::~appInteractor2DMeasure_Comment()
{
	for (int i = 0; i < m_CommentsVect.size(); i++)
	{
		// Text Comment
		m_Renderer->RemoveActor2D(m_TextCommentActorVector[i]);
		vtkDEL(m_TextCommentActorVector[i]);

		// Text Info (Author + Date)
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
	
	m_CommentsVect.clear();

	m_TextCommentActorVector.clear();
	m_TextInfoActorVector.clear();
	m_RectActorVector.clear();
	m_RectPointsVector.clear();
	m_RectPolygonVector.clear();
	m_RectCellArrayVector.clear();
	m_RectPointMapperVector.clear();
}

/// EVENT ////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Comment::OnEvent(albaEventBase *event)
{
	if (!m_IsEnabled) return;

	UpdateAllActors();
	
	Superclass::OnEvent(event);
}

/// MEASURE //////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Comment::AddMeasure(double *point)
{
	point[2] = 0;

	// Update Actors
	UpdateEditActors(point);

	//////////////////////////////////////////////////////////////////////////
	// Add New Comment
	CommentData newComment;

	// Incremental Name
	wxString name = "Note";
	char string[100];
	sprintf(string, " n. %d", m_CommentsVect.size() + 1);
	name.append(string);

	newComment.Name = name;
	newComment.Comment = "Comment";
	newComment.Author = "";
	newComment.EditDate = "";

	time_t now = time(0);
	char timestamp[24] = "";
	strftime(timestamp, sizeof(timestamp), "%d/%m/%Y %H:%M", localtime(&now));

	newComment.CreationDate = timestamp;
	newComment.Opacity = 1;
	newComment.Width = BOX_WIDTH;
	newComment.Height = BOX_HEIGHT;
	newComment.isVisible = true;
	newComment.Position[0] = point[0];
	newComment.Position[1] = point[1];

	m_CommentsVect.push_back(newComment);

	int lastIndex = m_CommentsVect.size() - 1;

	//////////////////////////////////////////////////////////////////////////
	// Add Measure
	albaString text;
	text.Printf("Note n. %d (%.2f, %.2f)", m_MeasuresCount + 1, point[0], point[1]);
	m_MeasureVector.push_back(text);
	m_MeasureLabelVector.push_back("");

	//////////////////////////////////////////////////////////////////////////
	// Add Point
	m_PointSourceVector.push_back(vtkPointSource::New());
	m_PointMapperVector.push_back(vtkPolyDataMapper2D::New());
	m_PointActorVector.push_back(vtkActor2D::New());

	m_Renderer->AddActor2D(m_PointActorVector[lastIndex]);

	UpdatePointActor(m_PointActorVector.size() - 1, point);

	//////////////////////////////////////////////////////////////////////////
	// Add Rect
	AddRect(point);

	//////////////////////////////////////////////////////////////////////////
	// Text Title
	m_TextActorVector.push_back(vtkALBATextActorMeter::New());
	m_TextActorVector[lastIndex]->GetTextActor()->GetTextProperty()->SetFontFamilyToCourier();
	m_TextActorVector[lastIndex]->GetTextActor()->GetTextProperty()->SetFontSize(12);
	m_TextActorVector[lastIndex]->GetTextActor()->GetTextProperty()->SetBold(1);
	m_TextActorVector[lastIndex]->GetTextActor()->GetTextProperty()->SetJustificationToLeft();
	m_TextActorVector[lastIndex]->GetTextActor()->GetTextProperty()->SetVerticalJustificationToBottom();
	
	m_Renderer->AddActor2D(m_TextActorVector[lastIndex]);

	//////////////////////////////////////////////////////////////////////////
	// Add Comment Text
	m_TextCommentActorVector.push_back(vtkALBATextActorMeter::New());
	m_TextCommentActorVector[lastIndex]->GetTextActor()->GetTextProperty()->SetFontSize(12);
	m_TextCommentActorVector[lastIndex]->GetTextActor()->GetTextProperty()->SetFontFamilyToCourier();
	m_TextCommentActorVector[lastIndex]->GetTextActor()->GetTextProperty()->SetJustificationToLeft();
	m_TextCommentActorVector[lastIndex]->GetTextActor()->GetTextProperty()->SetVerticalJustificationToTop();
	
	m_Renderer->AddActor2D(m_TextCommentActorVector[lastIndex]);

	//////////////////////////////////////////////////////////////////////////
	// Add Author Text
	m_TextInfoActorVector.push_back(vtkALBATextActorMeter::New());
	m_TextInfoActorVector[lastIndex]->GetTextActor()->GetTextProperty()->SetFontSize(10);
	m_TextInfoActorVector[lastIndex]->GetTextActor()->GetTextProperty()->SetFontFamilyToCourier();
	m_TextInfoActorVector[lastIndex]->GetTextActor()->GetTextProperty()->SetJustificationToRight();
	m_TextInfoActorVector[lastIndex]->GetTextActor()->GetTextProperty()->SetVerticalJustificationToBottom();
	
	m_Renderer->AddActor2D(m_TextInfoActorVector[lastIndex]);

	UpdateCommentActor(lastIndex);
	
	//////////////////////////////////////////////////////////////////////////

	m_MeasuresCount++;

	albaEventMacro(albaEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Comment::EditMeasure(int index, double *point)
{
	if (index < 0 || index >= m_MeasuresCount)
		return;

	m_MovingMeasure = true;

	point[2] = 0;

	m_LastEditing = index;

	//////////////////////////////////////////////////////////////////////////
	// Update Comment
	m_CommentsVect[index].Position[0] = point[0];
	m_CommentsVect[index].Position[1] = point[1];

	//////////////////////////////////////////////////////////////////////////
	// Update Measure
	albaString text;
	text.Printf("Note n. %d (%.2f, %.2f)", index + 1, point[0], point[1]);
	m_MeasureVector[index] = text;

	// Update Actors
	UpdateEditActors(point);
	// Point
	UpdatePointActor(index, point);
	// Comment
	UpdateCommentActor(index);
	
	//////////////////////////////////////////////////////////////////////////

	albaEventMacro(albaEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Comment::RemoveMeasure(int index)
{
	if (m_Renderer && index < m_MeasuresCount)
	{
		//////////////////////////////////////////////////////////////////////////
		// Comment
		m_CommentsVect.erase(m_CommentsVect.begin() + index);

		//////////////////////////////////////////////////////////////////////////
		// Measure
		m_MeasureVector.erase(m_MeasureVector.begin() + index);
		m_MeasureLabelVector.erase(m_MeasureLabelVector.begin() + index);

		//////////////////////////////////////////////////////////////////////////
		// Point
		m_Renderer->RemoveActor2D(m_PointActorVector[index]);
		vtkDEL(m_PointActorVector[index]);
		m_PointActorVector.erase(m_PointActorVector.begin() + index);

		vtkDEL(m_PointMapperVector[index]);
		m_PointMapperVector.erase(m_PointMapperVector.begin() + index);

		vtkDEL(m_PointSourceVector[index]);
		m_PointSourceVector.erase(m_PointSourceVector.begin() + index);

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
		// Text - Name
		m_Renderer->RemoveActor2D(m_TextActorVector[index]);
		vtkDEL(m_TextActorVector[index]);
		m_TextActorVector.erase(m_TextActorVector.begin() + index);

		//////////////////////////////////////////////////////////////////////////
		// Text Comment
		m_Renderer->RemoveActor2D(m_TextCommentActorVector[index]);
		vtkDEL(m_TextCommentActorVector[index]);
		m_TextCommentActorVector.erase(m_TextCommentActorVector.begin() + index);

		//////////////////////////////////////////////////////////////////////////
		// Text Info
		m_Renderer->RemoveActor2D(m_TextInfoActorVector[index]);
		vtkDEL(m_TextInfoActorVector[index]);
		m_TextInfoActorVector.erase(m_TextInfoActorVector.begin() + index);
		
		//////////////////////////////////////////////////////////////////////////

		m_Renderer->GetRenderWindow()->Render();

		m_MeasuresCount--;

		albaEventMacro(albaEvent(this, CAMERA_UPDATE));
	}
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Comment::SelectMeasure(int index)
{
	Superclass::SelectMeasure(index);

	for (int i = 0; i < m_CommentsVect.size(); i++)
	{
		//m_CommentsVect[i].isVisible = false;
		m_CommentsVect[i].Opacity = 0.5;
		UpdateCommentActor(i);
	}

	if (index > -1 && index < m_CommentsVect.size())
	{
		m_CommentsVect[index].Opacity = 1;
		UpdateCommentActor(index);
	}
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Comment::AddRect(double *point)
{
	//////////////////////////////////////////////////////////////////////////
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

	//////////////////////////////////////////////////////////////////////////
	// Actor
	m_RectActorVector.push_back(vtkActor2D::New());

	m_RectActorVector[index]->SetMapper(m_RectPointMapperVector[index]);
	m_RectActorVector[index]->GetProperty()->SetPointSize(1.5);
	m_RectActorVector[index]->GetProperty()->SetLineWidth(1.0);
	m_RectActorVector[index]->GetProperty()->SetColor(m_RectColor);

	m_Renderer->AddActor2D(m_RectActorVector[index]);
}

/// COMMENTS
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Comment::AddComment(double *point, albaString name, albaString comment /*= ""*/, albaString author /*= ""*/, albaString creation_date /*= ""*/, albaString date /*= ""*/)
{
	if (point) 
	{
		AddMeasure(point);
		int lastIndex = m_CommentsVect.size() - 1;

		m_CommentsVect[lastIndex].Name = name;
		m_CommentsVect[lastIndex].Comment = comment;
		m_CommentsVect[lastIndex].Author = author;
		m_CommentsVect[lastIndex].CreationDate = creation_date;
		m_CommentsVect[lastIndex].EditDate = date;
	}
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Comment::EditComment(int index, albaString name, albaString comment /*= ""*/, albaString author /*= ""*/, albaString date /*= ""*/, double *point /*= NULL*/)
{
	if (index > -1 && index < m_CommentsVect.size())
	{
		m_CommentsVect[index].Name = name;
		m_CommentsVect[index].Comment = comment;
		m_CommentsVect[index].Author = author;

		if (date == "")
		{
			time_t now = time(0);
			char timestamp[24] = "";
			strftime(timestamp, sizeof(timestamp), "%d/%m/%Y %H:%M", localtime(&now));
			date = timestamp;
		}
		// Update Edit Date
		m_CommentsVect[index].EditDate = date;

		if (point)
		{
			// Update Position
			EditMeasure(index, point);
		}
	}
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Comment::RemoveComment(int index)
{
	RemoveMeasure(index);
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Comment::SelectComment(int index)
{
	SelectMeasure(index);
}

/// UPDATE ///////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Comment::UpdateTextActor(int index, double *point)
{
	double text_pos[3];
	text_pos[0] = point[0];
	text_pos[1] = point[1];
	text_pos[2] = 0.0;
	
	if (index > -1 && index < m_RectPointsVector.size())
	{
		UpdateTextName(index, text_pos);
	}
	else
	{
		if (m_LastSelection >= 0)
			m_MeasureLabel = m_CommentsVect[m_LastSelection].Name;
		else
			m_MeasureLabel = "Note";

		m_Scale = m_ParallelScale_OnStart / m_Renderer->GetActiveCamera()->GetParallelScale();
		double margin = TEXT_MARGIN / m_Scale;

		text_pos[0] += margin;
		text_pos[1] -= margin;

		// Update Edit Actor
		m_EditTextActor->SetText(m_MeasureLabel);
		m_EditTextActor->SetTextPosition(text_pos);
		m_EditTextActor->SetVisibility(m_ShowText);
	}
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Comment::UpdateCommentActor(int index)
{
	if (index > -1 && index < m_RectPointsVector.size())
	{
		double text_pos[3];
		text_pos[0] = m_CommentsVect[index].Position[0];
		text_pos[1] = m_CommentsVect[index].Position[1];
		text_pos[2] = 0.0;

		m_Scale = m_ParallelScale_OnStart / m_Renderer->GetActiveCamera()->GetParallelScale();
		double margin = TEXT_MARGIN / m_Scale;

		// Text /////////////////////////////////////////////
		text_pos[0] += margin;
		text_pos[1] -= margin;

		UpdateTextName(index, text_pos);

		// Comment ///////////////////////////////////////////
		text_pos[1] = m_CommentsVect[index].Position[1] + margin;

		int lines = UpdateTextComment(index, text_pos);

		// Rectangle ///////////////////////////////////////////
		text_pos[0] = m_CommentsVect[index].Position[0];
		text_pos[1] = m_CommentsVect[index].Position[1];

		UpdateRect(index, text_pos, lines);

		// Author - Date ///////////////////////////////////////////
		double rect_size[2];
		rect_size[0] = m_CommentsVect[index].Width / m_Scale;
		rect_size[1] = m_CommentsVect[index].Height / m_Scale;

		text_pos[0] += rect_size[0] - margin;
		text_pos[1] += rect_size[1] - margin;

		UpdateTextDate(index, text_pos);
	}
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Comment::UpdateRect(int index, double *point, int h /*= MAX_LINES*/)
{
	m_CommentsVect[index].Height = (BOX_HEIGHT / MAX_LINES)*(h + 1);

	m_RectPointsVector[index]->SetPoint(0, point[0], point[1], 0.0);
	m_RectPointsVector[index]->SetPoint(1, point[0] + m_CommentsVect[index].Width / m_Scale, point[1], 0.0);
	m_RectPointsVector[index]->SetPoint(2, point[0] + m_CommentsVect[index].Width / m_Scale, point[1] + m_CommentsVect[index].Height / m_Scale, 0.0);
	m_RectPointsVector[index]->SetPoint(3, point[0], point[1] + m_CommentsVect[index].Height / m_Scale, 0.0);
	m_RectPointsVector[index]->Modified();

	// Update Opacity
	m_RectActorVector[index]->GetProperty()->SetOpacity(m_CommentsVect[index].Opacity);

	// Update Visibility
	m_RectActorVector[index]->SetVisibility(m_CommentsVect[index].isVisible);
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Comment::UpdateTextName(int index, double *text_pos)
{
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

	m_TextCommentActorVector[index]->SetColor(m_TextColor[0], m_TextColor[1], m_TextColor[2]);
	m_TextInfoActorVector[index]->SetColor(m_TextColor[0], m_TextColor[1], m_TextColor[2]);

	// Update Color
	if (m_LastSelection == index)
		m_TextActorVector[index]->SetColor(m_ColorSelection[0], m_ColorSelection[1], m_ColorSelection[2]);
	else
		m_TextActorVector[index]->SetColor(m_ColorDefault[0], m_ColorDefault[1], m_ColorDefault[2]);

	// Update Opacity
	m_TextActorVector[index]->SetOpacity(m_CommentsVect[index].Opacity);

	// Update Visibility
	m_TextActorVector[index]->SetVisibility(m_ShowText);
}
//----------------------------------------------------------------------------
int appInteractor2DMeasure_Comment::UpdateTextComment(int index, double *text_pos)
{
	int comment_lines = 1;

	if (m_CommentsVect[index].isVisible)
	{
		wxString aux_txt = "";
		int chars_in_line = 0;

		// Split comment to words
		std::istringstream iss(m_CommentsVect[index].Comment.GetCStr());
		std::vector<std::string> results((std::istream_iterator<std::string>(iss)),
			std::istream_iterator<std::string>());

		for (int i = 0; i < results.size(); i++)
		{
			wxString word(results[i].c_str());

			if (chars_in_line + word.Length() + 1 < MAX_CHARS_IN_LINE)
			{
				aux_txt += word + " ";
				chars_in_line += word.Length() + 1;
			}
			else
			{
				if (comment_lines >= MAX_LINES)
				{
					if (i < results.size())
						aux_txt += "...";

					comment_lines = MAX_LINES;
					break;
				}
				else
				{
					aux_txt += "\n" + word + " ";
					chars_in_line = word.Length() + 1;
					comment_lines++;
				}
			}
		}

		m_TextCommentActorVector[index]->SetText(aux_txt);
		m_TextCommentActorVector[index]->SetTextPosition(text_pos);
		m_TextCommentActorVector[index]->Modified();

		// Update Color
		m_TextCommentActorVector[index]->SetColor(m_TextColor[0], m_TextColor[1], m_TextColor[2]);
		
		// Update Opacity
		m_TextCommentActorVector[index]->SetOpacity(m_CommentsVect[index].Opacity);
	}

	// Update Visibility
	m_TextCommentActorVector[index]->SetVisibility(m_ShowText && m_CommentsVect[index].isVisible);

	return comment_lines;
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Comment::UpdateTextDate(int index, double *text_pos)
{
	if (m_CommentsVect[index].isVisible)
	{
		albaString date = m_CommentsVect[index].EditDate == "" ? m_CommentsVect[index].CreationDate : m_CommentsVect[index].EditDate;

		albaString author_date = m_CommentsVect[index].Author + " " + date;
		m_TextInfoActorVector[index]->SetText(author_date);
		m_TextInfoActorVector[index]->SetTextPosition(text_pos);
		m_TextInfoActorVector[index]->Modified();

		// Update Color
		m_TextInfoActorVector[index]->SetColor(m_TextColor[0], m_TextColor[1], m_TextColor[2]);

		// Update Opacity
		m_TextInfoActorVector[index]->SetOpacity(m_CommentsVect[index].Opacity);
	}

	// Update Visibility
	m_TextInfoActorVector[index]->SetVisibility(m_ShowText && m_CommentsVect[index].isVisible);
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Comment::UpdateAllActors()
{
	for (int i = 0; i < m_CommentsVect.size(); i++)
	{
		UpdateCommentActor(i);
	}

	Render();
}

//----------------------------------------------------------------------------
void appInteractor2DMeasure_Comment::ShowComment(int index, bool show)
{
	m_CommentsVect[index].isVisible = show;

	UpdateCommentActor(index);

	Render();
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Comment::SetTextColor(double r, double g, double b)
{
	m_TextColor[0] = r;
	m_TextColor[1] = g;
	m_TextColor[2] = b;

	UpdateAllActors();
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Comment::SetRectColor(double r, double g, double b)
{
	m_RectColor[0] = r;
	m_RectColor[1] = g;
	m_RectColor[2] = b;

	UpdateAllActors();
}

/// UTILS ///////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Comment::FindAndHighlightCurrentPoint(double *point)
{
	if (m_CurrentMeasureIndex < 0)
		SetAction(ACTION_ADD_MEASURE);

	if (m_EditMeasureEnable)
	{
		for (int i = 0; i < m_MeasuresCount; i++)
		{
			if (IsInRectBound(i, point))
			{
				SelectMeasure(i);

				m_CurrentMeasureIndex = i;

				m_Renderer->AddActor2D(m_EditPointActor);
				UpdatePointActor(-1, m_CommentsVect[i].Position);
				m_Renderer->GetRenderWindow()->Render();

				SetAction(ACTION_MOVE_MEASURE);
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
bool appInteractor2DMeasure_Comment::IsInRectBound(int index, double *point)
{
	if (point[0] >= m_RectPointsVector[index]->GetPoint(0)[0] && point[0] <= m_RectPointsVector[index]->GetPoint(1)[0] &&
		point[1] >= m_RectPointsVector[index]->GetPoint(0)[1] && point[1] <= m_RectPointsVector[index]->GetPoint(2)[1])
		return true;

	return false;
}

/// LOAD/SAVE ///////////////////////////////////////////////////////////////