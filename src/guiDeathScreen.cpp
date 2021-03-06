/*
guiDeathScreen.cpp
Copyright (C) 2013 celeron55, Perttu Ahola <celeron55@gmail.com>
*/

/*
This file is part of Freeminer.

Freeminer is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Freeminer  is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Freeminer.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "guiDeathScreen.h"
#include "debug.h"
#include "serialization.h"
#include <string>
#include <IGUICheckBox.h>
#include <IGUIEditBox.h>
#include <IGUIButton.h>
#include <IGUIStaticText.h>
#include <IGUIFont.h>
#include "gettext.h"
#include "client.h"

GUIDeathScreen::GUIDeathScreen(gui::IGUIEnvironment* env,
		gui::IGUIElement* parent, s32 id,
		IMenuManager *menumgr, IRespawnInitiator *respawner
):
	GUIModalMenu(env, parent, id, menumgr),
	m_respawner(respawner),
	m_screensize(1,1)
{
}

GUIDeathScreen::~GUIDeathScreen()
{
	removeChildren();
	delete m_respawner;
}

void GUIDeathScreen::removeChildren()
{
	const core::list<gui::IGUIElement*> &children = getChildren();
	core::list<gui::IGUIElement*> children_copy;
	for(core::list<gui::IGUIElement*>::ConstIterator
			i = children.begin(); i != children.end(); i++)
	{
		children_copy.push_back(*i);
	}
	for(core::list<gui::IGUIElement*>::Iterator
			i = children_copy.begin();
			i != children_copy.end(); i++)
	{
		(*i)->remove();
	}
}

void GUIDeathScreen::regenerateGui(v2u32 screensize)
{
	m_screensize = screensize;

	/*
		Remove stuff
	*/
	removeChildren();
	
	/*
		Calculate new sizes and positions
	*/
	core::rect<s32> rect(
			screensize.X/2 - 500/2,
			screensize.Y/2 - 200/2,
			screensize.X/2 + 500/2,
			screensize.Y/2 + 200/2
	);
	
	DesiredRect = rect;
	recalculateAbsolutePosition(false);

	v2s32 size = rect.getSize();

	/*
		Add stuff
	*/
	
	{
		core::rect<s32> rect(0, 0, 400, 50);
		rect = rect + v2s32(size.X/2-400/2, size.Y/2-50/2-25);
		wchar_t* text = wgettext("You died.");
		Environment->addStaticText(text, rect, false,
				true, this, 256);
		delete[] text;
	}
	{
		core::rect<s32> rect(0, 0, 140, 30);
		rect = rect + v2s32(size.X/2-140/2, size.Y/2-30/2+25);
		wchar_t* text = wgettext("Respawn");
		gui::IGUIElement *e = 
		Environment->addButton(rect, this, 257,
			text);
		delete[] text;
		Environment->setFocus(e);
	}
}

void GUIDeathScreen::drawMenu()
{
	gui::IGUISkin* skin = Environment->getSkin();
	if (!skin)
		return;
	video::IVideoDriver* driver = Environment->getVideoDriver();
	
	{
		video::SColor color(180,50,0,0);
		driver->draw2DRectangle(color,
				core::rect<s32>(0,0,m_screensize.X,m_screensize.Y), NULL);
	}
	{
		video::SColor bgcolor(50,0,0,0);
		driver->draw2DRectangle(bgcolor, AbsoluteRect, &AbsoluteClippingRect);
	}

	gui::IGUIElement::draw();
}

bool GUIDeathScreen::OnEvent(const SEvent& event)
{
	if(event.EventType==EET_KEY_INPUT_EVENT)
	{
		if(event.KeyInput.Key==KEY_ESCAPE && event.KeyInput.PressedDown)
		{
			respawn();
			quitMenu();
			return true;
		}
		if(event.KeyInput.Key==KEY_RETURN && event.KeyInput.PressedDown)
		{
			respawn();
			quitMenu();
			return true;
		}
	}
	if(event.EventType==EET_GUI_EVENT)
	{
		if(event.GUIEvent.EventType==gui::EGET_ELEMENT_FOCUS_LOST
				&& isVisible())
		{
			if(!canTakeFocus(event.GUIEvent.Element))
			{
				dstream<<"GUIDeathScreen: Not allowing focus change."
						<<std::endl;
				// Returning true disables focus change
				return true;
			}
		}
		if(event.GUIEvent.EventType==gui::EGET_BUTTON_CLICKED)
		{
			switch(event.GUIEvent.Caller->getID())
			{
			case 257:
				respawn();
				quitMenu();
				return true;
			}
		}
	}

	return Parent ? Parent->OnEvent(event) : false;
}

void GUIDeathScreen::respawn()
{
	m_respawner->respawn();
}

