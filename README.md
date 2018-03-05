=========================================================================
Program:   Albedo - ALBA Example Application
Language:  C++
Date:      $Date: 2018-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
=========================================================================
Copyright (c) LTM-IOR 2018 (https://github.com/IOR-LTM)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================

....................................
....................................
[Application - Albedo] (StartUp Project)

*Albedo ----------------------------
 Initialize Logic, Register, Icons 
 Plugs Operations, Views, custom VMEs, custom Visual Pipes
 Plugs custom Managers, Snapshot, Wizard, Licence, etc...
 Manages Start, Exit, Fatal Exceptions, EventKeys 
 
*appLogic --------------------------
 Initialize Window app and Gui (Menu, Toolbar, Panels)
 Manages events (system, operations, views, VMEs, others)
 
*appViewManager (optional) ---------
 Manages views events
 
-appWizardSample (example)
....................................
....................................

[appCommon]

-appUtils
 Implements Utility (es. GetAppDirectory() or GetConfigDirectory())
....................................
....................................

[appGui]

-appSideBar
 Implements custom SideBar (VME, Views, Operation panels)

-appGUILabel 
 Implements Custom Gui widgets, panels, dialogs
....................................
....................................

[appOperations]

-appOpEmpty
-appOpGuiSample
....................................
....................................

[appViews]

-appViewImageCompound
....................................
....................................

------------------------------------
TESTING

[appTests]

-appTest
-appFakeLogicForTesting
....................................
....................................

[appOperationsTest]

-appOpEmptyTest
....................................