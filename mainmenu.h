#ifndef __INCLUDED_MAINMENU_H
#define __INCLUDED_MAINMENU_H

#include <plx/font.h>
#include <plx/list.h>
#include <plx/dr.h>

#include "menu.h"

class MainMenu : public Menu {
public:
	MainMenu();
	~MainMenu();

	int showMenu();

};

#endif

