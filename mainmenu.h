#ifndef __INCLUDED_MAINMENU_H
#define __INCLUDED_MAINMENU_H

#include "menu.h"
#include "resources.h"

class MainMenu : public Menu {
public:
	MainMenu();
	~MainMenu();

	int showMenu();

};

#endif

