#include <kos.h>
#include "mainmenu.h"

MainMenu::MainMenu() {
}

MainMenu::~MainMenu() {
}

int MainMenu::showMenu() {
	int choice = 0;

	while (true) {
		MAPLE_FOREACH_BEGIN(MAPLE_FUNC_CONTROLLER, cont_state_t, st)
			if (st->buttons & CONT_START)
				return choice;
			// TODO: change item on up/down
		MAPLE_FOREACH_END()
	}
}

