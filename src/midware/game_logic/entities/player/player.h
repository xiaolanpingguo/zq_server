#pragma once


#include "../object/object.h"
#include "../../server/world_session.h"

namespace zq{


enum EPlayerFieldInt32 : uint32
{
	PLAYER_RACE = 0x0001,
	PLAYER_CLASS = 0x0002,
	PLAYER_GENDER = 0x0003,
	PLAYER_SKIN = 0x0004,
	PLAYER_FACE = 0x0005,
	PLAYER_HAIR_STYLE = 0x0006,
	PLAYER_HAIR_COLOR = 0x0007,
	PLAYER_FACIA_HAIR = 0x0008,
	PLAYER_OUT_FIT_ID = 0x0009,
	INT32_FIELD_MAX
};

enum EPlayerFieldUint64 : uint32
{
	PLAYER_GUID = 0x0000,
	UINT64_FIELD_MAX
};

enum EPlayerFieldString : uint32
{
	PLAYER_NAME = 0x0000,
	STRING_FIELD_MAX
};


class Player : public Object
{
public:
    explicit Player(WorldSession* session);
    ~Player();  
       
};

}
