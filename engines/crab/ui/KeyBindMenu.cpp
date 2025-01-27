/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*
 * This code is based on the CRAB engine
 *
 * Copyright (c) Arvind Raja Yadav
 *
 * Licensed under MIT
 *
 */

#include "crab/crab.h"
#include "crab/ui/KeyBindMenu.h"

namespace Crab {

using namespace pyrodactyl::ui;
using namespace pyrodactyl::image;
using namespace pyrodactyl::text;
using namespace pyrodactyl::input;

void KeyBindMenu::load(rapidxml::xml_node<char> *node) {
	if (nodeValid(node)) {
		if (nodeValid("menu", node)) {
			rapidxml::xml_node<char> *menode = node->first_node("menu");

			if (nodeValid("primary", menode))
				_prim.load(menode->first_node("primary"));

			if (nodeValid("alt", menode))
				_alt.load(menode->first_node("alt"));

			if (nodeValid("prompt", menode))
				_prompt.load(menode->first_node("prompt"));

			if (nodeValid("inc", menode))
				_inc.load(menode->first_node("inc"));

			if (nodeValid("dim", menode))
				_dim.load(menode->first_node("dim"));

			if (nodeValid("divide", menode))
				_divide.load(menode->first_node("divide"));

			if (nodeValid("desc", menode))
				_desc.load(menode->first_node("desc"));
		}

		// Initialize the menus
		initMenu(CON_GAME);
		initMenu(CON_UI);

		if (nodeValid("controls", node))
			_selControls.load(node->first_node("controls"));
	}
}

void KeyBindMenu::startAndSize(const int &type, int &start, int &size) {
	switch (type) {
	case CON_GAME:
		start = IG_START;
		size = IG_SIZE;
		break;
	case CON_UI:
		start = IU_START;
		size = IU_SIZE;
		break;
	default:
		break;
	}
}

void KeyBindMenu::initMenu(const int &type) {
	warning("STUB: KeyBindMenu::initMenu()");

#if 0
	int start = 0, size = 0;
	StartAndSize(type, start, size);

	// Initialize the menu
	menu[type].element.resize(size * 2);
	for (int i = 0; i < size * 2; i += 2) {
		int xoffset = inc.x * (i / 2) + divide.x * ((i / 2) / dim.x);
		int yoffset = inc.y * ((i / 2) % dim.x) + divide.y * ((i / 2) / dim.x);

		menu[type].element[i].Init(prim, xoffset, yoffset);
		menu[type].element[i].caption.text = SDL_GetScancodeName(g_engine->_inputManager->iv[start + (i / 2)].key);

		menu[type].element[i + 1].Init(alt, xoffset, yoffset);
		menu[type].element[i + 1].caption.text = SDL_GetScancodeName(g_engine->_inputManager->iv[start + (i / 2)].alt);
	}
#endif
}

void KeyBindMenu::drawDesc(const int &type) {
	int start = 0, size = 0;
	startAndSize(type, start, size);

	for (int i = 0; i < size; i++) {
		int xoffset = _inc.x * i + _divide.x * (i / _dim.x);
		int yoffset = _inc.y * (i % _dim.x) + _divide.y * (i / _dim.x);

		_desc.draw(g_engine->_inputManager->_iv[i + start], xoffset, yoffset);
	}
}

void KeyBindMenu::draw() {
	_selControls.draw();

	_menu[_selControls._cur].draw();
	drawDesc(_selControls._cur);
}

void KeyBindMenu::setCaption() {
	warning("STUB: KeyBindMenu::setCaption()");

#if 0
	int start = 0, size = 0;
	StartAndSize(sel_controls.cur, start, size);

	for (int i = 0; i < size * 2; i += 2) {
		menu[sel_controls.cur].element[i].caption.text = SDL_GetScancodeName(g_engine->_inputManager->iv[start + (i / 2)].key);
		menu[sel_controls.cur].element[i + 1].caption.text = SDL_GetScancodeName(g_engine->_inputManager->iv[start + (i / 2)].alt);
	}
#endif
}

void KeyBindMenu::handleEvents(const Common::Event &event) {
	warning("STUB: KeyBindMenu::handleEvents()");

	/*
	if (sel_controls.handleEvents(Event))
		SetCaption();

	switch (state) {
	case STATE_NORMAL:
		choice = menu[sel_controls.cur].handleEvents(Event);
		if (choice >= 0) {
			prompt.Swap(menu[sel_controls.cur].element[choice].caption);
			state = STATE_KEY;
			break;
		}

		break;
	case STATE_KEY:
		if (Event.type == SDL_KEYDOWN) {
			SwapKey(Event.key.keysym.scancode);
			SetCaption();
			menu[sel_controls.cur].element[choice].caption.col = prompt.col_prev;
			state = STATE_NORMAL;
		}
		break;
	default:
		break;
	}
	*/
}

#if 0
void KeyBindMenu::handleEvents(const SDL_Event &Event) {
	if (sel_controls.handleEvents(Event))
		SetCaption();

	switch (state) {
	case STATE_NORMAL:
		choice = menu[sel_controls.cur].handleEvents(Event);
		if (choice >= 0) {
			prompt.Swap(menu[sel_controls.cur].element[choice].caption);
			state = STATE_KEY;
			break;
		}

		break;
	case STATE_KEY:
		if (Event.type == SDL_KEYDOWN) {
			SwapKey(Event.key.keysym.scancode);
			SetCaption();
			menu[sel_controls.cur].element[choice].caption.col = prompt.col_prev;
			state = STATE_NORMAL;
		}
		break;
	default:
		break;
	}
}

void KeyBindMenu::SwapKey(const SDL_Scancode &find) {
	int start = 0, size = 0;
	StartAndSize(sel_controls.cur, start, size);
	int pos = start + (choice / 2);

	for (int i = start; i < start + size; ++i) {
		if (g_engine->_inputManager->iv[i].key == find) {
			g_engine->_inputManager->iv[i].key = g_engine->_inputManager->iv[pos].key;
			break;
		} else if (g_engine->_inputManager->iv[i].alt == find) {
			g_engine->_inputManager->iv[i].alt = g_engine->_inputManager->iv[pos].key;
			break;
		}
	}

	if (choice % 2 == 0)
		g_engine->_inputManager->iv[pos].key = find;
	else
		g_engine->_inputManager->iv[pos].alt = find;
}
#endif

void KeyBindMenu::setUI() {
	_menu[CON_GAME].clear();
	_menu[CON_UI].clear();

	// Initialize the menus
	_prim.setUI();
	_alt.setUI();
	initMenu(CON_GAME);
	initMenu(CON_UI);

	_desc.setUI();
	_selControls.setUI();
}

} // End of namespace Crab
