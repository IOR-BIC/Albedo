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

	void SelectMeasure(int index);

	void AddComment(albaString name, albaString comment = "", albaString author = "", albaString date = "", double *point = NULL);
	void EditComment(int index, albaString name, albaString comment = "", albaString author = "", albaString date = "", double *point = NULL);
	void RemoveComment(int index);

	void ShowComment(int index, bool show);
		
	albaString GetName(int index) { return m_CommentsVect[index].Name; };
	albaString GetComment(int index) { return m_CommentsVect[index].Comment; };
	albaString GetAuthor(int index) { return m_CommentsVect[index].Author; };
	albaString GetDateTime(int index) { return m_CommentsVect[index].DateTime; };
	void GetPosition(int index, double *point) { GetMeasurePoint(index, point); };

	void GetName(int index, albaString name) { m_CommentsVect[index].Name = name; };
	void GetComment(int index, albaString comment) { m_CommentsVect[index].Comment = comment; };
	void GetAuthor(int index, albaString author) { m_CommentsVect[index].Author = author; };
	void GetDateTime(int index, albaString date) { m_CommentsVect[index].DateTime = date; };

	bool IsVisible(int index) { return m_CommentsVect[index].isVisible; };
	void SetTextColor(double r, double g, double b);
	void SetRectColor(double r, double g, double b);

protected:

	struct CommentData
	{
		albaString Name;
		albaString Comment;
		albaString Author;
		albaString DateTime;
		double Position[2];

		int Width;
		int Height;
		bool isVisible;
	};

	appInteractor2DMeasure_Comment();
	virtual ~appInteractor2DMeasure_Comment();

	void AddRect(double * point);

	void UpdateAllActors();
	void UpdateTextActor(int index, double * point);

	void FindAndHighlightCurrentPoint(double * pointCoord);

	bool IsInBound(int index, double * pointCoord);

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
	
	double m_OriginalScale;
	double m_TextColor[3];
	double m_RectColor[3];

private:
	appInteractor2DMeasure_Comment(const appInteractor2DMeasure_Comment&);   // Not implemented.
	void operator=(const appInteractor2DMeasure_Comment&);  // Not implemented.
	friend class appInteractor2DMeasure_CommentTest;
};
#endif
