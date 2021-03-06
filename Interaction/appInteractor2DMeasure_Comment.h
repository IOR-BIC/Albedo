/*=========================================================================
Program:   Albedo
Module:    appInteractor2DMeasure_Comment.h
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

#ifndef __appInteractor2DMeasure_Comment_h
#define __appInteractor2DMeasure_Comment_h

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appInteractor2DMeasure_Point.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkTextMapper.h"
#include "vtkTextActor.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------

// Class Name: appInteractor2DMeasure_Comment
class APP_INTERACTION_EXPORT appInteractor2DMeasure_Comment : public appInteractor2DMeasure_Point
{
public:

	albaTypeMacro(appInteractor2DMeasure_Comment, appInteractor2DMeasure_Point);

	virtual void OnEvent(albaEventBase *event);
	
	void AddComment(double *point, albaString name, albaString comment = "", albaString author = "", albaString creation_date = "", albaString date ="");
	void EditComment(int index, albaString name, albaString comment = "", albaString author = "", albaString date = "", double *point = NULL);
	void RemoveComment(int index);
	void SelectComment(int index);
	void ShowComment(int index, bool show);

	albaString GetName(int index) { return m_CommentsVect[index].Name; };
	albaString GetComment(int index) { return m_CommentsVect[index].Comment; };
	albaString GetAuthor(int index) { return m_CommentsVect[index].Author; };
	albaString GetDateTime(int index) { return m_CommentsVect[index].EditDate; };
	albaString GetCreationDateTime(int index) { return m_CommentsVect[index].CreationDate; };
	
	void SetName(int index, albaString name) { m_CommentsVect[index].Name = name; };
	void SetComment(int index, albaString comment) { m_CommentsVect[index].Comment = comment; };
	void SetAuthor(int index, albaString author) { m_CommentsVect[index].Author = author; };
	void SetDateTime(int index, albaString date) { m_CommentsVect[index].EditDate = date; };
	void SetCreationDate(int index, albaString date) { m_CommentsVect[index].CreationDate = date; };

	bool IsVisible(int index) { return m_CommentsVect[index].isVisible; };
	void SetTextColor(double r, double g, double b);
	void SetRectColor(double r, double g, double b);

protected:

	struct CommentData
	{
		albaString Name;
		albaString Comment;
		albaString Author;
		wxString CreationDate;
		albaString EditDate;
		double Position[2];
		double Opacity;
		int Width;
		int Height;
		bool isVisible;
	};

	appInteractor2DMeasure_Comment();
	virtual ~appInteractor2DMeasure_Comment();

	/// MEASURE
	/** Add Measure*/
	void AddMeasure(double *point1);
	/** Edit Measure*/
	void EditMeasure(int index, double *point);
	/** Delete the Measure*/
	void RemoveMeasure(int index);
	/** Select a Measure*/
	void SelectMeasure(int index);

	void AddRect(double * point);

	void UpdateAllActors();

	void UpdateTextActor(int index, double *point);
	
	void UpdateCommentActor(int index);

	void UpdateTextDate(int index, double * text_pos);

	void UpdateRect(int index, double *point, int h = 8);

	int UpdateTextComment(int index, double * text_pos);

	void UpdateTextName(int index, double * text_pos);

	void FindAndHighlightCurrentPoint(double *point);

	bool IsInRectBound(int index, double *point);

	// Comment Vector
	std::vector<CommentData> m_CommentsVect;

	// Text Actor Vector
	std::vector<vtkALBATextActorMeter *> m_TextCommentActorVector;
	std::vector<vtkALBATextActorMeter *> m_TextInfoActorVector;

	// Rect Vector
	std::vector<vtkPoints *>						m_RectPointsVector;
	std::vector<vtkPolygon *>						m_RectPolygonVector;
	std::vector<vtkCellArray *>					m_RectCellArrayVector;
	std::vector<vtkPolyData *>					m_RectPolyDataVector;
	std::vector<vtkPolyDataMapper2D *>	m_RectPointMapperVector;
	std::vector<vtkActor2D *>						m_RectActorVector;

	double m_TextColor[3];
	double m_RectColor[3];

	double m_Scale;

private:
	appInteractor2DMeasure_Comment(const appInteractor2DMeasure_Comment&);   // Not implemented.
	void operator=(const appInteractor2DMeasure_Comment&);  // Not implemented.
	friend class appInteractor2DMeasure_CommentTest;
};
#endif
