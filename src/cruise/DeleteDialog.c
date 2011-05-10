// *************************** -*- C++ -*- ***********************************
// *  Cruise - Interviews Interface to OCEAN Sea Of Gates Design System.     *
// *                                                                         *
// *  Author : Ireneusz Karkowski 1993                                       *
// *  SccsId = @(#)DeleteDialog.c 1.1  05/21/93 
// ***************************************************************************

#include <InterViews/button.h>
#include "DeleteDialog.h" 
#include <IV-2_6/_enter.h>

DeleteDialog::DeleteDialog(const char* name) : DeleteDialog_core(name) {}

void DeleteDialog::canceled() {
  int value;
  delCancelBS->GetValue(value);
  if (value!=0)
  {
    deleteDialogBS->SetValue(2);  /* Accept will return 0 */
  } 
}

void DeleteDialog::confirmed() {

  int value;
  delOKBS->GetValue(value);
  if (value!=0)
  {
    deleteDialogBS->SetValue(1);  /* Accept will return 1 */
  } 

}


