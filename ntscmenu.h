#ifndef __INCLUDED_NTSCMENU_H
#define __INCLUDED_NTSCMENU_H

#include "menu.h"

class NtscMenu {
public:
	NtscMenu();
	~NtscMenu();

	void showMenu();

private:
	pvr_ptr_t	addr;
	pvr_poly_hdr_t	header;

};

#endif

