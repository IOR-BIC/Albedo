/*=========================================================================
Program:   Albedo
Module:    appOpAddComment.h
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

#ifndef __appOpAddComment_H__
#define __appOpAddComment_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appOperationsDefines.h"
#include "appDecl.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class appInteractor2DMeasure_Comment;
class albaGUIDictionaryWidget;
class albaGUIDialog;

// Begin For Testing
class vtkImageData;
class albaVMEImage;
// End For Testing

//----------------------------------------------------------------------------
// Class Name: appOpAddComment
//----------------------------------------------------------------------------
class APP_OPERATIONS_EXPORT appOpAddComment : public albaOp
{
public:

	struct CommentData
	{
		wxString Name;
		wxString Comment;
		wxString Author;
		wxString DateTime_Modify;
		wxString DateTime_Creation;
		double Position[2];

	} CommentInfo;

	//Widgets ID's	
	enum OP_COMMENT_ID
	{
		ID_COMMENT = MINID,
		ID_ADD_ELEMENT,
		ID_DELETE_ELEMENT,
		ID_SELECT_ELEMENT,
		ID_EDIT_ELEMENT,
		ID_SHOW_ELEMENT,
		ID_LOAD_COMMENTS,
		ID_SAVE_COMMENTS,
		ID_SHOW_DIALOG,
		ID_DEL_DIALOG,
		ID_CLOSE_DIALOG,
	};

	/** Constructor. */
	appOpAddComment(wxString label = "Op Empty", bool enable = true);

	/** Destructor. */
	~appOpAddComment();

	/** RTTI macro. */
	albaTypeMacro(appOpAddComment, albaOp);

	/** Return a copy of the operation */
	/*virtual*/ albaOp* Copy();

	/** Return true for the acceptable vme type. */
	/*virtual*/ bool Accept(albaVME *node);

	/** Builds operation's interface. */
	/*virtual*/ void OpRun();

	/** Execute the operation. */
	/*virtual*/ void OpDo();

	/** Receive events coming from the user interface.*/
	void OnEvent(albaEventBase *alba_event);

	void Onevent_FromInteractor(albaEventBase * alba_event);

	void OnEvent_FromGui(albaEventBase *alba_event);
	
	void EnableOp(bool enable) { m_Enabled = enable; };

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char ** GetIcon();

protected:

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	/*virtual*/ void OpStop(int result);	

	/** Create the Operation GUI */
	virtual void CreateGui();
	void UpdateGui();
	void EnableDisableGui();
	
	/// Elements
	void SelectElement(int selection, bool updateInteractor = true);
	void NewElement();
	void AddElement(CommentData info);
	void DeleteElement();
	void EditElement();
	void ShowHideElement();

	int ImportImage(); // For Testing

	/// Dialog
	void CreateDialog();
	void UpdateDialog();
	void ShowDialog();
	void HideDialog();

	void Load();
	void Save();
	
	albaGUIDialog	*m_Dialog;
	bool m_IsDialogOpened;

	wxTextCtrl *m_Name_textCtrl;
	wxTextCtrl *m_Comment_textCtrl;
	wxTextCtrl *m_Author_textCtrl;
	wxStaticText *m_Date_text;

	bool m_Enabled;

	wxListBox *m_CommentListBox;
	albaGUIDictionaryWidget * m_CommentDict;
	appInteractor2DMeasure_Comment *m_CommentInteractor;

	std::vector<CommentData> m_CommentsVect;

	int m_Selected;

	wxString m_NameToEdit;
	wxString m_CommentToEdit;
	wxString m_AuthorToEdit;
	wxString m_DateToEdit;
	wxString m_CrationDate;
	int m_ShowElement;

	wxString m_Current_Author; // Check Author Name from settings
};
#endif
