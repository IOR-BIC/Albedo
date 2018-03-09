/*=========================================================================
Program:   Albedo
Module:    appOpDictionary.h
Language:  C++
Date:      $Date: 2018-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) LTM-IOR 2018 (https://github.com/IOR-LTM)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __appOpDictionary_H__
#define __appOpDictionary_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appOperationsDefines.h"
#include "appDecl.h"

#include "mafOp.h"

#include <vector>
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/dom/DOM.hpp>

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class mafGUIDictionaryWidget;

typedef std::vector<wxString> StringVector;

//----------------------------------------------------------------------------
// Class Name: appOpDictionary
//----------------------------------------------------------------------------
class APP_OPERATIONS_EXPORT appOpDictionary : public mafOp
{
public:
	/** Constructor. */
	appOpDictionary(wxString label = "opDictionary");

	/** Destructor. */
	~appOpDictionary();

	/** RTTI macro. */
	mafTypeMacro(appOpDictionary, mafOp);

	/** Return a copy of the operation */
	/*virtual*/ mafOp* Copy();

	/** Return true for the acceptable vme type. */
	/*virtual*/ bool Accept(mafVME *node);

	/** Builds operation's interface. */
	/*virtual*/ void OpRun();

	/** Execute the operation. */
	/*virtual*/ void OpDo();

	/** Receive events coming from the user interface.*/
	void OnEvent(mafEventBase *maf_event);
	
	//Widgets ID's	
	enum OP_DICTIONARY_ID
	{
		ID_LOAD_DICTIONARY = MINID,
		ID_SAVE_DICTIONARY,
		ID_SELECT_ELEMENT,
		ID_ADD_ELEMENT,
		ID_DELETE_ELEMENT,
		ID_EDIT_ELEMENT,
		ID_MOVE_UP_ELEMENT,
		ID_MOVE_DOWN_ELEMENT,
		ID_SELECT_GROUP,
		ID_ADD_GROUP,
		ID_DELETE_GROUP,
		ID_EDIT_GROUP,
		ID_RESET_GROUP,
		ID_RESET_DICTIONARY,
		ID_INCREMENTAL_NAME,
	};

protected:

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	/*virtual*/ void OpStop(int result);	

	/** Create the Operation GUI */
	virtual void CreateGui();
	void UpdateGui();
	void EnableDisableGui();

	int LoadDictionary(wxString fileName);
	int SaveDictionary(const char *fileName);
	bool CheckNodeElement(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, const char *elementName);
	mafString GetElementAttribute(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, const char *attributeName);

	void SelectGroup(int selection);
	void SelectElement(mafString selection);
	
	void AddGroup();
	void AddElement();

	void DeleteGroup();
	void DeleteElement();

	void EditGroup();
	void EditElement();

	void MoveUpElement();
	void MoveDownElement();

	void ResetGroup();
	void ResetDictionary();

	
	std::vector<StringVector> m_ElementVect;
	StringVector m_GroupVect;

	wxString m_DictionaryName;
	wxString m_DictionaryVersion;

	wxString m_DictionaryTypeName;
	wxString m_DictionaryGroupTypeName;
	wxString m_DictionaryElementTypeName;

	int m_SelectedGroup;
	wxString m_SelectedElement;
	wxString m_ElementToEdit;
	wxString m_GroupToEdit;

	int m_IncrementalName;

	// Gui widgets
	wxComboBox	*m_GroupComboBox;
	mafGUIDictionaryWidget *m_ElementDict;
};
#endif
