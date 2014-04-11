/*
clientserver.h
Copyright (C) 2010-2013 celeron55, Perttu Ahola <celeron55@gmail.com>
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

#ifndef CLIENTSERVER_HEADER
#define CLIENTSERVER_HEADER

#include <vector>
#include <utility>
#include <string>
#include "irrlichttypes.h"
#include <msgpack.hpp>

#define MAX_PACKET_SIZE 512

/*
	changes by PROTOCOL_VERSION:

	PROTOCOL_VERSION 3:
		Base for writing changes here
	PROTOCOL_VERSION 4:
		Add TOCLIENT_MEDIA
		Add TOCLIENT_TOOLDEF
		Add TOCLIENT_NODEDEF
		Add TOCLIENT_CRAFTITEMDEF
		Add TOSERVER_INTERACT
		Obsolete TOSERVER_CLICK_ACTIVEOBJECT
		Obsolete TOSERVER_GROUND_ACTION
	PROTOCOL_VERSION 5:
		Make players to be handled mostly as ActiveObjects
	PROTOCOL_VERSION 6:
		Only non-cached textures are sent
	PROTOCOL_VERSION 7:
		Add TOCLIENT_ITEMDEF
		Obsolete TOCLIENT_TOOLDEF
		Obsolete TOCLIENT_CRAFTITEMDEF
		Compress the contents of TOCLIENT_ITEMDEF and TOCLIENT_NODEDEF
	PROTOCOL_VERSION 8:
		Digging based on item groups
		Many things
	PROTOCOL_VERSION 9:
		ContentFeatures and NodeDefManager use a different serialization
		    format; better for future version cross-compatibility
		Many things
	PROTOCOL_VERSION 10:
		TOCLIENT_PRIVILEGES
		Version raised to force 'fly' and 'fast' privileges into effect.
		Node metadata change (came in later; somewhat incompatible)
	PROTOCOL_VERSION 11:
		TileDef in ContentFeatures
		Nodebox drawtype
		(some dev snapshot)
		TOCLIENT_INVENTORY_FORMSPEC
		(0.4.0, 0.4.1)
	PROTOCOL_VERSION 12:
		TOSERVER_INVENTORY_FIELDS
		16-bit node ids
		TOCLIENT_DETACHED_INVENTORY
	PROTOCOL_VERSION 13:
		InventoryList field "Width" (deserialization fails with old versions)
	PROTOCOL_VERSION 14:
		Added transfer of player pressed keys to the server
		Added new messages for mesh and bone animation, as well as attachments
		GENERIC_CMD_SET_ANIMATION
		GENERIC_CMD_SET_BONE_POSITION
		GENERIC_CMD_SET_ATTACHMENT
	PROTOCOL_VERSION 15:
		Serialization format changes
	PROTOCOL_VERSION 16:
		TOCLIENT_SHOW_FORMSPEC
	PROTOCOL_VERSION 17:
		Serialization format change: include backface_culling flag in TileDef
		Added rightclickable field in nodedef
		TOCLIENT_SPAWN_PARTICLE
		TOCLIENT_ADD_PARTICLESPAWNER
		TOCLIENT_DELETE_PARTICLESPAWNER
	PROTOCOL_VERSION 18:
		damageGroups added to ToolCapabilities
		sound_place added to ItemDefinition
	PROTOCOL_VERSION 19:
		GENERIC_CMD_SET_PHYSICS_OVERRIDE
	PROTOCOL_VERSION 20:
		TOCLIENT_HUDADD
		TOCLIENT_HUDRM
		TOCLIENT_HUDCHANGE
		TOCLIENT_HUD_SET_FLAGS
	PROTOCOL_VERSION 21:
		TOCLIENT_BREATH
		TOSERVER_BREATH
		range added to ItemDefinition
		drowning, leveled added to ContentFeatures
		stepheight and collideWithObjects added to object properties
		version, heat and humidity transfer in MapBock
		automatic_face_movement_dir and automatic_face_movement_dir_offset
			added to object properties
	PROTOCOL_VERSION 23:
		TOCLIENT_ANIMATIONS
*/

#define LATEST_PROTOCOL_VERSION 23

// Server's supported network protocol range
#define SERVER_PROTOCOL_VERSION_MIN 13
#define SERVER_PROTOCOL_VERSION_MAX LATEST_PROTOCOL_VERSION

// Client's supported network protocol range
#define CLIENT_PROTOCOL_VERSION_MIN 13
#define CLIENT_PROTOCOL_VERSION_MAX LATEST_PROTOCOL_VERSION

// Constant that differentiates the protocol from random data and other protocols
#define PROTOCOL_ID 0x4f457403

#define PASSWORD_SIZE 28       // Maximum password length. Allows for
                               // base64-encoded SHA-1 (27+\0).

#define TEXTURENAME_ALLOWED_CHARS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.-"

// TOCLIENT_* commands

#define TOCLIENT_INIT 0x10
enum {
	// u8 deployed version
	TOCLIENT_INIT_DEPLOYED,
	// u64 map seed
	TOCLIENT_INIT_SEED,
	// float recommended send interval (server step)
	TOCLIENT_INIT_STEP,
	// v3f player's position
	TOCLIENT_INIT_POS
};

#define TOCLIENT_REMOVENODE 0x22
enum {
	TOCLIENT_REMOVENODE_POS
};

#define TOCLIENT_BREATH 0x4e
enum {
	// u16 breath
	TOCLIENT_BREATH_BREATH
};

#define TOCLIENT_TIME_OF_DAY 0x29
enum {
	// u16 time (0-23999)
	TOCLIENT_TIME_OF_DAY_TIME,
	// f32 time_speed
	TOCLIENT_TIME_OF_DAY_TIME_SPEED
};

#define TOCLIENT_ACTIVE_OBJECT_REMOVE_ADD 0x31
enum {
	// list of ids
	TOCLIENT_ACTIVE_OBJECT_REMOVE_ADD_REMOVE,
	// list of [id, type, initialization_data]
	TOCLIENT_ACTIVE_OBJECT_REMOVE_ADD_ADD
};

struct ActiveObjectAddData {
	ActiveObjectAddData(u16 id_, u8 type_, std::string data_) : id(id_), type(type_), data(data_) {}
	ActiveObjectAddData() : id(0), type(0), data("") {}
	u16 id;
	u8 type;
	std::string data;
	MSGPACK_DEFINE(id, type, data)
};

#define TOCLIENT_ACTIVE_OBJECT_MESSAGES 0x32
enum {
	// list of pair<id, message> where id is u16 and message is string
	TOCLIENT_ACTIVE_OBJECT_MESSAGES_MESSAGES
};
typedef std::vector<std::pair<unsigned int, std::string> > ActiveObjectMessages;

#define TOCLIENT_HP 0x33
enum {
	TOCLIENT_HP_HP
};

#define TOCLIENT_MOVE_PLAYER 0x34
enum {
	// v3f player position
	TOCLIENT_MOVE_PLAYER_POS,
	// f32 pitch
	TOCLIENT_MOVE_PLAYER_PITCH,
	// f32 yaw
	TOCLIENT_MOVE_PLAYER_YAW
};

#define TOCLIENT_DEATHSCREEN 0x37
enum {
	// bool set camera point target
	TOCLIENT_DEATHSCREEN_SET_CAMERA,
	// v3f camera point target (to point the death cause or whatever)
	TOCLIENT_DEATHSCREEN_CAMERA_POINT
};

#define TOCLIENT_MEDIA 0x38
enum {
	// vector<pair<name, data>>
	TOCLIENT_MEDIA_MEDIA
};
typedef std::vector<std::pair<std::string, std::string> > MediaData;

#define TOCLIENT_ANNOUNCE_MEDIA 0x3c
enum {
	// list of [string name, string sha1_digest]
	TOCLIENT_ANNOUNCE_MEDIA_LIST,
	// string, url of remote media server
	TOCLIENT_ANNOUNCE_MEDIA_REMOTE_SERVER
};
typedef std::vector<std::pair<std::string, std::string> > MediaAnnounceList;

#define TOCLIENT_PLAY_SOUND 0x3f
enum {
	// s32
	TOCLIENT_PLAY_SOUND_ID,
	// string
	TOCLIENT_PLAY_SOUND_NAME,
	// f32
	TOCLIENT_PLAY_SOUND_GAIN,
	// u8
	TOCLIENT_PLAY_SOUND_TYPE,
	// v3f
	TOCLIENT_PLAY_SOUND_POS,
	// u16
	TOCLIENT_PLAY_SOUND_OBJECT_ID,
	// bool
	TOCLIENT_PLAY_SOUND_LOOP
};

#define TOCLIENT_PRIVILEGES 0x41
enum {
	// list of strings
	TOCLIENT_PRIVILEGES_PRIVILEGES
};

#define TOCLIENT_STOP_SOUND 0x40
enum {
	// s32
	TOCLIENT_STOP_SOUND_ID
};

#define TOCLIENT_INVENTORY_FORMSPEC 0x42
enum {
	// string
	TOCLIENT_INVENTORY_FORMSPEC_DATA
};

#define TOCLIENT_SHOW_FORMSPEC 0x44
enum {
	// string formspec
	TOCLIENT_SHOW_FORMSPEC_DATA,
	// string formname
	TOCLIENT_SHOW_FORMSPEC_NAME
};

#define TOCLIENT_CHAT_MESSAGE 0x30
enum {
	// string
	TOCLIENT_CHAT_MESSAGE_DATA
};

#define TOCLIENT_ACCESS_DENIED 0x35
enum {
	// string
	TOCLIENT_ACCESS_DENIED_REASON
};

#define TOCLIENT_NODEDEF 0x3a
enum {
	TOCLIENT_NODEDEF_DEFINITIONS
};

#define TOCLIENT_ITEMDEF 0x3d
enum {
	TOCLIENT_ITEMDEF_DEFINITIONS
};

#define TOCLIENT_INVENTORY 0x27
enum {
	// string, serialized inventory
	TOCLIENT_INVENTORY_DATA
};

#define TOCLIENT_DETACHED_INVENTORY 0x43
enum {
	TOCLIENT_DETACHED_INVENTORY_NAME,
	TOCLIENT_DETACHED_INVENTORY_DATA
};

#define TOCLIENT_MOVEMENT 0x45
// all values are floats here
enum {
	TOCLIENT_MOVEMENT_ACCELERATION_DEFAULT,
	TOCLIENT_MOVEMENT_ACCELERATION_AIR,
	TOCLIENT_MOVEMENT_ACCELERATION_FAST,
	TOCLIENT_MOVEMENT_SPEED_WALK,
	TOCLIENT_MOVEMENT_SPEED_CROUCH,
	TOCLIENT_MOVEMENT_SPEED_FAST,
	TOCLIENT_MOVEMENT_SPEED_CLIMB,
	TOCLIENT_MOVEMENT_SPEED_JUMP,
	TOCLIENT_MOVEMENT_LIQUID_FLUIDITY,
	TOCLIENT_MOVEMENT_LIQUID_FLUIDITY_SMOOTH,
	TOCLIENT_MOVEMENT_LIQUID_SINK,
	TOCLIENT_MOVEMENT_GRAVITY
};

#define TOCLIENT_SPAWN_PARTICLE 0x46
enum {
	TOCLIENT_SPAWN_PARTICLE_POS,
	TOCLIENT_SPAWN_PARTICLE_VELOCITY,
	TOCLIENT_SPAWN_PARTICLE_ACCELERATION,
	TOCLIENT_SPAWN_PARTICLE_EXPIRATIONTIME,
	TOCLIENT_SPAWN_PARTICLE_SIZE,
	TOCLIENT_SPAWN_PARTICLE_COLLISIONDETECTION,
	TOCLIENT_SPAWN_PARTICLE_VERTICAL,
	TOCLIENT_SPAWN_PARTICLE_TEXTURE
};

#define TOCLIENT_ADD_PARTICLESPAWNER 0x47
enum {
	TOCLIENT_ADD_PARTICLESPAWNER_AMOUNT,
	TOCLIENT_ADD_PARTICLESPAWNER_SPAWNTIME,
	TOCLIENT_ADD_PARTICLESPAWNER_MINPOS,
	TOCLIENT_ADD_PARTICLESPAWNER_MAXPOS,
	TOCLIENT_ADD_PARTICLESPAWNER_MINVEL,
	TOCLIENT_ADD_PARTICLESPAWNER_MAXVEL,
	TOCLIENT_ADD_PARTICLESPAWNER_MINACC,
	TOCLIENT_ADD_PARTICLESPAWNER_MAXACC,
	TOCLIENT_ADD_PARTICLESPAWNER_MINEXPTIME,
	TOCLIENT_ADD_PARTICLESPAWNER_MAXEXPTIME,
	TOCLIENT_ADD_PARTICLESPAWNER_MINSIZE,
	TOCLIENT_ADD_PARTICLESPAWNER_MAXSIZE,
	TOCLIENT_ADD_PARTICLESPAWNER_COLLISIONDETECTION,
	TOCLIENT_ADD_PARTICLESPAWNER_VERTICAL,
	TOCLIENT_ADD_PARTICLESPAWNER_TEXTURE,
	TOCLIENT_ADD_PARTICLESPAWNER_ID
};

#define TOCLIENT_DELETE_PARTICLESPAWNER 0x48
enum {
	TOCLIENT_DELETE_PARTICLESPAWNER_ID
};

#define TOCLIENT_ANIMATIONS 0x4f
enum {
	TOCLIENT_ANIMATIONS_DEFAULT_START,
	TOCLIENT_ANIMATIONS_DEFAULT_STOP,
	TOCLIENT_ANIMATIONS_WALK_START,
	TOCLIENT_ANIMATIONS_WALK_STOP,
	TOCLIENT_ANIMATIONS_DIG_START,
	TOCLIENT_ANIMATIONS_DIG_STOP,
	TOCLIENT_ANIMATIONS_WD_START,
	TOCLIENT_ANIMATIONS_WD_STOP
};

#define TOCLIENT_HUDRM 0x4a
enum {
	TOCLIENT_HUDRM_ID
};

#define TOCLIENT_HUD_SET_FLAGS 0x4c
enum {
	TOCLIENT_HUD_SET_FLAGS_FLAGS,
	TOCLIENT_HUD_SET_FLAGS_MASK
};

#define TOCLIENT_HUD_SET_PARAM 0x4d
enum {
	TOCLIENT_HUD_SET_PARAM_ID,
	TOCLIENT_HUD_SET_PARAM_VALUE
};

#define TOCLIENT_HUDCHANGE 0x4b
enum {
	TOCLIENT_HUDCHANGE_ID,
	TOCLIENT_HUDCHANGE_STAT,
	TOCLIENT_HUDCHANGE_V2F,
	TOCLIENT_HUDCHANGE_V3F,
	TOCLIENT_HUDCHANGE_STRING,
	TOCLIENT_HUDCHANGE_U32
};

#define TOCLIENT_HUDADD 0x49
enum {
	TOCLIENT_HUDADD_ID,
	TOCLIENT_HUDADD_TYPE,
	TOCLIENT_HUDADD_POS,
	TOCLIENT_HUDADD_NAME,
	TOCLIENT_HUDADD_SCALE,
	TOCLIENT_HUDADD_TEXT,
	TOCLIENT_HUDADD_NUMBER,
	TOCLIENT_HUDADD_ITEM,
	TOCLIENT_HUDADD_DIR,
	TOCLIENT_HUDADD_ALIGN,
	TOCLIENT_HUDADD_OFFSET,
	TOCLIENT_HUDADD_WORLD_POS
};

#define TOCLIENT_ADDNODE 0x21
enum {
	TOCLIENT_ADDNODE_POS,
	TOCLIENT_ADDNODE_NODE,
	TOCLIENT_ADDNODE_REMOVE_METADATA
};

#define TOCLIENT_BLOCKDATA 0x20
enum {
	TOCLIENT_BLOCKDATA_POS,
	TOCLIENT_BLOCKDATA_DATA,
	TOCLIENT_BLOCKDATA_HEAT,
	TOCLIENT_BLOCKDATA_HUMIDITY
};

#define TOCLIENT_SET_SKY 0xae
enum {
	TOCLIENT_SET_SKY_COLOR,
	TOCLIENT_SET_SKY_TYPE,
	TOCLIENT_SET_SKY_PARAMS
};

#define TOCLIENT_OVERRIDE_DAY_NIGHT_RATIO 0x50
enum {
	TOCLIENT_OVERRIDE_DAY_NIGHT_RATIO_DO,
	TOCLIENT_OVERRIDE_DAY_NIGHT_RATIO_VALUE
};

// TOSERVER_* commands

#define TOSERVER_INIT 0x10
enum {
	// u8 SER_FMT_VER_HIGHEST_READ
	TOSERVER_INIT_FMT,
	TOSERVER_INIT_NAME,
	TOSERVER_INIT_PASSWORD,
	TOSERVER_INIT_PROTOCOL_VERSION_MIN,
	TOSERVER_INIT_PROTOCOL_VERSION_MAX
};

#define TOSERVER_INIT2 0x11

#define TOSERVER_PLAYERPOS 0x23
enum {
	// v3f
	TOSERVER_PLAYERPOS_POSITION,
	// v3f
	TOSERVER_PLAYERPOS_SPEED,
	// f32
	TOSERVER_PLAYERPOS_PITCH,
	// f32
	TOSERVER_PLAYERPOS_YAW,
	// u32
	TOSERVER_PLAYERPOS_KEY_PRESSED
};

#define TOSERVER_CHAT_MESSAGE 0x32
enum {
	TOSERVER_CHAT_MESSAGE_DATA
};

#define TOSERVER_GOTBLOCKS 0x24
enum {
	TOSERVER_GOTBLOCKS_BLOCKS,
	TOSERVER_GOTBLOCKS_RANGE
};

#define TOSERVER_DELETEDBLOCKS 0x25
enum {
	TOSERVER_DELETEDBLOCKS_DATA
};

#define TOSERVER_DAMAGE 0x35
enum {
	TOSERVER_DAMAGE_VALUE
};

#define TOSERVER_CHANGE_PASSWORD 0x36
enum {
	TOSERVER_CHANGE_PASSWORD_OLD,
	TOSERVER_CHANGE_PASSWORD_NEW
};

#define TOSERVER_PLAYERITEM 0x37
enum {
	TOSERVER_PLAYERITEM_VALUE
};

#define TOSERVER_RESPAWN 0x38

#define TOSERVER_REMOVED_SOUNDS 0x3a
enum {
	TOSERVER_REMOVED_SOUNDS_IDS
};

#define TOSERVER_NODEMETA_FIELDS 0x3b
enum {
	TOSERVER_NODEMETA_FIELDS_POS,
	TOSERVER_NODEMETA_FIELDS_FORMNAME,
	TOSERVER_NODEMETA_FIELDS_DATA
};

#define TOSERVER_REQUEST_MEDIA 0x40
enum {
	TOSERVER_REQUEST_MEDIA_FILES
};

#define TOSERVER_RECEIVED_MEDIA 0x41

#define TOSERVER_BREATH 0x42
enum {
	TOSERVER_BREATH_VALUE
};

enum ToServerCommand
{
	TOSERVER_INVENTORY_ACTION = 0x31,
	/*
		See InventoryAction in inventory.h
	*/

	TOSERVER_INTERACT = 0x39,
	/*
		[0] u16 command
		[2] u8 action
		[3] u16 item
		[5] u32 length of the next item
		[9] serialized PointedThing
		actions:
		0: start digging (from undersurface) or use
		1: stop digging (all parameters ignored)
		2: digging completed
		3: place block or item (to abovesurface)
		4: use item

		(Obsoletes TOSERVER_GROUND_ACTION and TOSERVER_CLICK_ACTIVEOBJECT.)
	*/

	TOSERVER_INVENTORY_FIELDS = 0x3c,
	/*
		u16 command
		u16 len
		u8[len] form name (reserved for future use)
		u16 number of fields
		for each field:
			u16 len
			u8[len] field name
			u32 len
			u8[len] field value
	*/
};

#endif
