/*
script/cpp_api/s_item.h
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

#ifndef S_ITEM_H_
#define S_ITEM_H_

#include "cpp_api/s_base.h"
#include "irr_v3d.h"

struct PointedThing;
struct ItemStack;
class ServerActiveObject;
struct ItemDefinition;
class LuaItemStack;
class ModApiItemMod;
class InventoryList;
class InventoryLocation;

class ScriptApiItem
: virtual public ScriptApiBase
{
public:
	bool item_OnDrop(ItemStack &item,
			ServerActiveObject *dropper, v3f pos);
	bool item_OnPlace(ItemStack &item,
			ServerActiveObject *placer, const PointedThing &pointed);
	bool item_OnUse(ItemStack &item,
			ServerActiveObject *user, const PointedThing &pointed);
	bool item_OnCraft(ItemStack &item, ServerActiveObject *user,
			const InventoryList *old_craft_grid, const InventoryLocation &craft_inv);
	bool item_CraftPredict(ItemStack &item, ServerActiveObject *user,
			const InventoryList *old_craft_grid, const InventoryLocation &craft_inv);

protected:
	friend class LuaItemStack;
	friend class ModApiItemMod;

	bool getItemCallback(const char *name, const char *callbackname);
	void pushPointedThing(const PointedThing& pointed);

};


#endif /* S_ITEM_H_ */
