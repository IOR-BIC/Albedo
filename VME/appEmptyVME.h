/*=========================================================================
Program:   Albedo
Module:    appEmptyVME.h
Language:  C++
Date:      $Date: 2019-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2019 (http://www.ltmsoftware.org/alba.htm)

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

#include "albaVMEGenericAbstract.h"
#include "albaEvent.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class albaEvent;
class albaGUI;
class albaVME;

//----------------------------------------------------------------------------
// Class Name: appEmptyVME
//----------------------------------------------------------------------------
class ALBA_EXPORT appEmptyVME : public albaVME
{
public:

	enum
	{
		ID_DEFAULT = ID_LAST,
		ID_RESET_BUTTON,
	};

  albaTypeMacro(appEmptyVME, albaVME);
    
	// Return the output. This create the output object on demand.
	virtual albaVMEOutput *GetOutput();

  // Return an xpm-icon that can be used to represent this node.
  static char ** GetIcon();

protected:

  appEmptyVME();
  virtual ~appEmptyVME();
 
	virtual void SetMatrix(const albaMatrix &mat) override;

	virtual void GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes) override;

	// Internally used to create a new instance of the GUI.
	virtual albaGUI *CreateGui();

	void OnEvent(albaEventBase *alba_event);

	// Store and Restore
	virtual int InternalStore(albaStorageElement *node);
	virtual int InternalRestore(albaStorageElement *node);

	/** Called to prepare the update of the output */
	virtual void InternalPreUpdate();

	/** Update the output data structure */
	virtual void InternalUpdate();

	albaGUI *m_Gui;

	// VME Properties
	albaString m_String;
	int m_Integer;
	double m_Double;
	bool m_Bool;

private:

  appEmptyVME(const appEmptyVME&); // Not implemented
  void operator=(const appEmptyVME&); // Not implemented
};
#endif
