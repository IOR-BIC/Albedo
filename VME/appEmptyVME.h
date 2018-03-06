/*=========================================================================
Program:   Albedo
Module:    appEmptyVME.h
Language:  C++
Date:      $Date: 2018-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) LTM-IOR 2018 (http://www.ltmsoftware.org/alba.htm)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __appEmptyVME_h
#define __appEmptyVME_h

//----------------------------------------------------------------------------
// Includes :
//----------------------------------------------------------------------------
#include "appVMEDefines.h"

#include "mafVMEGenericAbstract.h"
#include "mafEvent.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class mafEvent;
class mafGUI;
class mafVME;

//----------------------------------------------------------------------------
// Class Name: appEmptyVME
//----------------------------------------------------------------------------
class MAF_EXPORT appEmptyVME : public mafVME
{
public:

	enum
	{
		ID_DEFAULT = ID_LAST,
		ID_RESET_BUTTON,
	};

  mafTypeMacro(appEmptyVME, mafVME);
    
	// Return the output. This create the output object on demand.
	virtual mafVMEOutput *GetOutput();

  // Return an xpm-icon that can be used to represent this node.
  static char ** GetIcon();

protected:

  appEmptyVME();
  virtual ~appEmptyVME();
 
	virtual void SetMatrix(const mafMatrix &mat) override;

	virtual void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes) override;

	// Internally used to create a new instance of the GUI.
	virtual mafGUI *CreateGui();

	void OnEvent(mafEventBase *maf_event);

	mafGUI *m_Gui;

	// VME Properties
	mafString m_String;
	int m_Integer;
	double m_Double;
	bool m_Bool;

private:

  appEmptyVME(const appEmptyVME&); // Not implemented
  void operator=(const appEmptyVME&); // Not implemented
};
#endif
