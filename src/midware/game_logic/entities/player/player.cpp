#include "player.h"



namespace zq {

Player::Player(WorldSession* session)
{
	m_objectType |= TYPEMASK_PLAYER;
	m_objectTypeId = TYPEID_PLAYER;
	initValue(PROERTY_TYPE::INT32, EPlayerFieldInt32::INT32_FIELD_MAX);
	initValue(PROERTY_TYPE::UINT64, EPlayerFieldUint64::UINT64_FIELD_MAX);
	initValue(PROERTY_TYPE::STRING, EPlayerFieldString::STRING_FIELD_MAX);
}

Player::~Player()
{

}

}
