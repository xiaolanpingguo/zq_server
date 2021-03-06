#pragma once


#include "interface_header/base/platform.h"


namespace zq{


	enum Races
	{
		RACE_NONE = 0,
		RACE_HUMAN = 1,
		RACE_ORC = 2,
		RACE_DWARF = 3,
		RACE_NIGHTELF = 4,
		RACE_UNDEAD_PLAYER = 5,
		RACE_TAUREN = 6,
		RACE_GNOME = 7,
		RACE_TROLL = 8,
		//RACE_GOBLIN             = 9,
		RACE_BLOODELF = 10,
		RACE_DRAENEI = 11
		//RACE_FEL_ORC            = 12,
		//RACE_NAGA               = 13,
		//RACE_BROKEN             = 14,
		//RACE_SKELETON           = 15,
		//RACE_VRYKUL             = 16,
		//RACE_TUSKARR            = 17,
		//RACE_FOREST_TROLL       = 18,
		//RACE_TAUNKA             = 19,
		//RACE_NORTHREND_SKELETON = 20,
		//RACE_ICE_TROLL          = 21
	};

	// max+1 for player race
#define MAX_RACES         12

#define RACEMASK_ALL_PLAYABLE \
    ((1<<(RACE_HUMAN-1))   |(1<<(RACE_ORC-1))          |(1<<(RACE_DWARF-1))   | \
     (1<<(RACE_NIGHTELF-1))|(1<<(RACE_UNDEAD_PLAYER-1))|(1<<(RACE_TAUREN-1))  | \
     (1<<(RACE_GNOME-1))   |(1<<(RACE_TROLL-1))        |(1<<(RACE_BLOODELF-1))| \
     (1<<(RACE_DRAENEI-1)))

#define RACEMASK_ALLIANCE \
    ((1<<(RACE_HUMAN-1)) | (1<<(RACE_DWARF-1)) | (1<<(RACE_NIGHTELF-1)) | \
     (1<<(RACE_GNOME-1)) | (1<<(RACE_DRAENEI-1)))


// Class value is index in ChrClasses.dbc
enum Classes
{
	CLASS_NONE = 0,
	CLASS_WARRIOR = 1,
	CLASS_PALADIN = 2,
	CLASS_HUNTER = 3,
	CLASS_ROGUE = 4,
	CLASS_PRIEST = 5,
	CLASS_DEATH_KNIGHT = 6,
	CLASS_SHAMAN = 7,
	CLASS_MAGE = 8,
	CLASS_WARLOCK = 9,
	//CLASS_UNK           = 10,
	CLASS_DRUID = 11
};

	// max+1 for player class
#define MAX_CLASSES       12

#define CLASSMASK_ALL_PLAYABLE \
    ((1<<(CLASS_WARRIOR-1))|(1<<(CLASS_PALADIN-1))|(1<<(CLASS_HUNTER-1))| \
    (1<<(CLASS_ROGUE-1))  |(1<<(CLASS_PRIEST-1)) |(1<<(CLASS_SHAMAN-1))| \
    (1<<(CLASS_MAGE-1))   |(1<<(CLASS_WARLOCK-1))|(1<<(CLASS_DRUID-1)) | \
    (1<<(CLASS_DEATH_KNIGHT-1)))




enum LocaleConstant : uint8
{
	LOCALE_enUS = 0,
	LOCALE_koKR = 1,
	LOCALE_frFR = 2,
	LOCALE_deDE = 3,
	LOCALE_zhCN = 4,
	LOCALE_zhTW = 5,
	LOCALE_esES = 6,
	LOCALE_esMX = 7,
	LOCALE_ruRU = 8,

	TOTAL_LOCALES
};


enum SpellEffects
{
	SPELL_EFFECT_INSTAKILL = 1,
	SPELL_EFFECT_SCHOOL_DAMAGE = 2,
	SPELL_EFFECT_DUMMY = 3,
	SPELL_EFFECT_PORTAL_TELEPORT = 4,
	SPELL_EFFECT_TELEPORT_UNITS = 5,
	SPELL_EFFECT_APPLY_AURA = 6,
	SPELL_EFFECT_ENVIRONMENTAL_DAMAGE = 7,
	SPELL_EFFECT_POWER_DRAIN = 8,
	SPELL_EFFECT_HEALTH_LEECH = 9,
	SPELL_EFFECT_HEAL = 10,
	SPELL_EFFECT_BIND = 11,
	SPELL_EFFECT_PORTAL = 12,
	SPELL_EFFECT_RITUAL_BASE = 13,
	SPELL_EFFECT_RITUAL_SPECIALIZE = 14,
	SPELL_EFFECT_RITUAL_ACTIVATE_PORTAL = 15,
	SPELL_EFFECT_QUEST_COMPLETE = 16,
	SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL = 17,
	SPELL_EFFECT_RESURRECT = 18,
	SPELL_EFFECT_ADD_EXTRA_ATTACKS = 19,
	SPELL_EFFECT_DODGE = 20,
	SPELL_EFFECT_EVADE = 21,
	SPELL_EFFECT_PARRY = 22,
	SPELL_EFFECT_BLOCK = 23,
	SPELL_EFFECT_CREATE_ITEM = 24,
	SPELL_EFFECT_WEAPON = 25,
	SPELL_EFFECT_DEFENSE = 26,
	SPELL_EFFECT_PERSISTENT_AREA_AURA = 27,
	SPELL_EFFECT_SUMMON = 28,
	SPELL_EFFECT_LEAP = 29,
	SPELL_EFFECT_ENERGIZE = 30,
	SPELL_EFFECT_WEAPON_PERCENT_DAMAGE = 31,
	SPELL_EFFECT_TRIGGER_MISSILE = 32,
	SPELL_EFFECT_OPEN_LOCK = 33,
	SPELL_EFFECT_SUMMON_CHANGE_ITEM = 34,
	SPELL_EFFECT_APPLY_AREA_AURA_PARTY = 35,
	SPELL_EFFECT_LEARN_SPELL = 36,
	SPELL_EFFECT_SPELL_DEFENSE = 37,
	SPELL_EFFECT_DISPEL = 38,
	SPELL_EFFECT_LANGUAGE = 39,
	SPELL_EFFECT_DUAL_WIELD = 40,
	SPELL_EFFECT_JUMP = 41,
	SPELL_EFFECT_JUMP_DEST = 42,
	SPELL_EFFECT_TELEPORT_UNITS_FACE_CASTER = 43,
	SPELL_EFFECT_SKILL_STEP = 44,
	SPELL_EFFECT_ADD_HONOR = 45,
	SPELL_EFFECT_SPAWN = 46,
	SPELL_EFFECT_TRADE_SKILL = 47,
	SPELL_EFFECT_STEALTH = 48,
	SPELL_EFFECT_DETECT = 49,
	SPELL_EFFECT_TRANS_DOOR = 50,
	SPELL_EFFECT_FORCE_CRITICAL_HIT = 51,
	SPELL_EFFECT_GUARANTEE_HIT = 52,
	SPELL_EFFECT_ENCHANT_ITEM = 53,
	SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY = 54,
	SPELL_EFFECT_TAMECREATURE = 55,
	SPELL_EFFECT_SUMMON_PET = 56,
	SPELL_EFFECT_LEARN_PET_SPELL = 57,
	SPELL_EFFECT_WEAPON_DAMAGE = 58,
	SPELL_EFFECT_CREATE_RANDOM_ITEM = 59,
	SPELL_EFFECT_PROFICIENCY = 60,
	SPELL_EFFECT_SEND_EVENT = 61,
	SPELL_EFFECT_POWER_BURN = 62,
	SPELL_EFFECT_THREAT = 63,
	SPELL_EFFECT_TRIGGER_SPELL = 64,
	SPELL_EFFECT_APPLY_AREA_AURA_RAID = 65,
	SPELL_EFFECT_CREATE_MANA_GEM = 66,
	SPELL_EFFECT_HEAL_MAX_HEALTH = 67,
	SPELL_EFFECT_INTERRUPT_CAST = 68,
	SPELL_EFFECT_DISTRACT = 69,
	SPELL_EFFECT_PULL = 70,
	SPELL_EFFECT_PICKPOCKET = 71,
	SPELL_EFFECT_ADD_FARSIGHT = 72,
	SPELL_EFFECT_UNTRAIN_TALENTS = 73,
	SPELL_EFFECT_APPLY_GLYPH = 74,
	SPELL_EFFECT_HEAL_MECHANICAL = 75,
	SPELL_EFFECT_SUMMON_OBJECT_WILD = 76,
	SPELL_EFFECT_SCRIPT_EFFECT = 77,
	SPELL_EFFECT_ATTACK = 78,
	SPELL_EFFECT_SANCTUARY = 79,
	SPELL_EFFECT_ADD_COMBO_POINTS = 80,
	SPELL_EFFECT_CREATE_HOUSE = 81,
	SPELL_EFFECT_BIND_SIGHT = 82,
	SPELL_EFFECT_DUEL = 83,
	SPELL_EFFECT_STUCK = 84,
	SPELL_EFFECT_SUMMON_PLAYER = 85,
	SPELL_EFFECT_ACTIVATE_OBJECT = 86,
	SPELL_EFFECT_GAMEOBJECT_DAMAGE = 87,
	SPELL_EFFECT_GAMEOBJECT_REPAIR = 88,
	SPELL_EFFECT_GAMEOBJECT_SET_DESTRUCTION_STATE = 89,
	SPELL_EFFECT_KILL_CREDIT = 90,
	SPELL_EFFECT_THREAT_ALL = 91,
	SPELL_EFFECT_ENCHANT_HELD_ITEM = 92,
	SPELL_EFFECT_FORCE_DESELECT = 93,
	SPELL_EFFECT_SELF_RESURRECT = 94,
	SPELL_EFFECT_SKINNING = 95,
	SPELL_EFFECT_CHARGE = 96,
	SPELL_EFFECT_CAST_BUTTON = 97,
	SPELL_EFFECT_KNOCK_BACK = 98,
	SPELL_EFFECT_DISENCHANT = 99,
	SPELL_EFFECT_INEBRIATE = 100,
	SPELL_EFFECT_FEED_PET = 101,
	SPELL_EFFECT_DISMISS_PET = 102,
	SPELL_EFFECT_REPUTATION = 103,
	SPELL_EFFECT_SUMMON_OBJECT_SLOT1 = 104,
	SPELL_EFFECT_SUMMON_OBJECT_SLOT2 = 105,
	SPELL_EFFECT_SUMMON_OBJECT_SLOT3 = 106,
	SPELL_EFFECT_SUMMON_OBJECT_SLOT4 = 107,
	SPELL_EFFECT_DISPEL_MECHANIC = 108,
	SPELL_EFFECT_RESURRECT_PET = 109,
	SPELL_EFFECT_DESTROY_ALL_TOTEMS = 110,
	SPELL_EFFECT_DURABILITY_DAMAGE = 111,
	SPELL_EFFECT_112 = 112,
	SPELL_EFFECT_RESURRECT_NEW = 113,
	SPELL_EFFECT_ATTACK_ME = 114,
	SPELL_EFFECT_DURABILITY_DAMAGE_PCT = 115,
	SPELL_EFFECT_SKIN_PLAYER_CORPSE = 116,
	SPELL_EFFECT_SPIRIT_HEAL = 117,
	SPELL_EFFECT_SKILL = 118,
	SPELL_EFFECT_APPLY_AREA_AURA_PET = 119,
	SPELL_EFFECT_TELEPORT_GRAVEYARD = 120,
	SPELL_EFFECT_NORMALIZED_WEAPON_DMG = 121,
	SPELL_EFFECT_122 = 122,
	SPELL_EFFECT_SEND_TAXI = 123,
	SPELL_EFFECT_PULL_TOWARDS = 124,
	SPELL_EFFECT_MODIFY_THREAT_PERCENT = 125,
	SPELL_EFFECT_STEAL_BENEFICIAL_BUFF = 126,
	SPELL_EFFECT_PROSPECTING = 127,
	SPELL_EFFECT_APPLY_AREA_AURA_FRIEND = 128,
	SPELL_EFFECT_APPLY_AREA_AURA_ENEMY = 129,
	SPELL_EFFECT_REDIRECT_THREAT = 130,
	SPELL_EFFECT_PLAY_SOUND = 131,
	SPELL_EFFECT_PLAY_MUSIC = 132,
	SPELL_EFFECT_UNLEARN_SPECIALIZATION = 133,
	SPELL_EFFECT_KILL_CREDIT2 = 134,
	SPELL_EFFECT_CALL_PET = 135,
	SPELL_EFFECT_HEAL_PCT = 136,
	SPELL_EFFECT_ENERGIZE_PCT = 137,
	SPELL_EFFECT_LEAP_BACK = 138,
	SPELL_EFFECT_CLEAR_QUEST = 139,
	SPELL_EFFECT_FORCE_CAST = 140,
	SPELL_EFFECT_FORCE_CAST_WITH_VALUE = 141,
	SPELL_EFFECT_TRIGGER_SPELL_WITH_VALUE = 142,
	SPELL_EFFECT_APPLY_AREA_AURA_OWNER = 143,
	SPELL_EFFECT_KNOCK_BACK_DEST = 144,
	SPELL_EFFECT_PULL_TOWARDS_DEST = 145,
	SPELL_EFFECT_ACTIVATE_RUNE = 146,
	SPELL_EFFECT_QUEST_FAIL = 147,
	SPELL_EFFECT_TRIGGER_MISSILE_SPELL_WITH_VALUE = 148,
	SPELL_EFFECT_CHARGE_DEST = 149,
	SPELL_EFFECT_QUEST_START = 150,
	SPELL_EFFECT_TRIGGER_SPELL_2 = 151,
	SPELL_EFFECT_SUMMON_RAF_FRIEND = 152,
	SPELL_EFFECT_CREATE_TAMED_PET = 153,
	SPELL_EFFECT_DISCOVER_TAXI = 154,
	SPELL_EFFECT_TITAN_GRIP = 155,
	SPELL_EFFECT_ENCHANT_ITEM_PRISMATIC = 156,
	SPELL_EFFECT_CREATE_ITEM_2 = 157,
	SPELL_EFFECT_MILLING = 158,
	SPELL_EFFECT_ALLOW_RENAME_PET = 159,
	SPELL_EFFECT_FORCE_CAST_2 = 160,
	SPELL_EFFECT_TALENT_SPEC_COUNT = 161,
	SPELL_EFFECT_TALENT_SPEC_SELECT = 162,
	SPELL_EFFECT_163 = 163,
	SPELL_EFFECT_REMOVE_AURA = 164,
	TOTAL_SPELL_EFFECTS = 165
};

enum SpellAttr0
{
	SPELL_ATTR0_UNK0 = 0x00000001,
	SPELL_ATTR0_REQ_AMMO = 0x00000002, // TITLE Treat as ranged attack DESCRIPTION Use ammo, ranged attack range modifiers, ranged haste, etc.
	SPELL_ATTR0_ON_NEXT_SWING = 0x00000004, // TITLE On next melee (type 1) DESCRIPTION Both "on next swing" attributes have identical handling in server & client
	SPELL_ATTR0_IS_REPLENISHMENT = 0x00000008, // TITLE Replenishment (client only)
	SPELL_ATTR0_ABILITY = 0x00000010, // TITLE Treat as ability DESCRIPTION Cannot be reflected, not affected by cast speed modifiers, etc.
	SPELL_ATTR0_TRADESPELL = 0x00000020, // TITLE Trade skill recipe DESCRIPTION Displayed in recipe list, not affected by cast speed modifiers
	SPELL_ATTR0_PASSIVE = 0x00000040, // TITLE Passive spell DESCRIPTION Spell is automatically cast on self by core
	SPELL_ATTR0_HIDDEN_CLIENTSIDE = 0x00000080, // TITLE Hidden in UI (client only) DESCRIPTION Not visible in spellbook or aura bar
	SPELL_ATTR0_HIDE_IN_COMBAT_LOG = 0x00000100, // TITLE Hidden in combat log (client only) DESCRIPTION Spell will not appear in combat logs
	SPELL_ATTR0_TARGET_MAINHAND_ITEM = 0x00000200, // TITLE Auto-target mainhand item (client only) DESCRIPTION Client will automatically select main-hand item as cast target
	SPELL_ATTR0_ON_NEXT_SWING_2 = 0x00000400, // TITLE On next melee (type 2) DESCRIPTION Both "on next swing" attributes have identical handling in server & client
	SPELL_ATTR0_UNK11 = 0x00000800,
	SPELL_ATTR0_DAYTIME_ONLY = 0x00001000, // TITLE Only usable during daytime (unused)
	SPELL_ATTR0_NIGHT_ONLY = 0x00002000, // TITLE Only usable during nighttime (unused)
	SPELL_ATTR0_INDOORS_ONLY = 0x00004000, // TITLE Only usable indoors
	SPELL_ATTR0_OUTDOORS_ONLY = 0x00008000, // TITLE Only usable outdoors
	SPELL_ATTR0_NOT_SHAPESHIFT = 0x00010000, // TITLE Not usable while shapeshifted
	SPELL_ATTR0_ONLY_STEALTHED = 0x00020000, // TITLE Only usable in stealth
	SPELL_ATTR0_DONT_AFFECT_SHEATH_STATE = 0x00040000, // TITLE Don't shealthe weapons (client only)
	SPELL_ATTR0_LEVEL_DAMAGE_CALCULATION = 0x00080000, // TITLE Scale with caster level DESCRIPTION For non-player casts, scale impact and power cost with caster's level
	SPELL_ATTR0_STOP_ATTACK_TARGET = 0x00100000, // TITLE Stop attacking after cast DESCRIPTION After casting this, the current auto-attack will be interrupted
	SPELL_ATTR0_IMPOSSIBLE_DODGE_PARRY_BLOCK = 0x00200000, // TITLE Prevent physical avoidance DESCRIPTION Spell cannot be dodged, parried or blocked
	SPELL_ATTR0_CAST_TRACK_TARGET = 0x00400000, // TITLE Automatically face target during cast (client only)
	SPELL_ATTR0_CASTABLE_WHILE_DEAD = 0x00800000, // TITLE Can be cast while dead DESCRIPTION Spells without this flag cannot be cast by dead units in non-triggered contexts
	SPELL_ATTR0_CASTABLE_WHILE_MOUNTED = 0x01000000, // TITLE Can be cast while mounted
	SPELL_ATTR0_DISABLED_WHILE_ACTIVE = 0x02000000, // TITLE Cooldown starts on expiry DESCRIPTION Spell is unusable while already active, and cooldown does not begin until the effects have worn off
	SPELL_ATTR0_NEGATIVE_1 = 0x04000000, // TITLE Is negative spell DESCRIPTION Forces the spell to be treated as a negative spell
	SPELL_ATTR0_CASTABLE_WHILE_SITTING = 0x08000000, // TITLE Can be cast while sitting
	SPELL_ATTR0_CANT_USED_IN_COMBAT = 0x10000000, // TITLE Cannot be used in combat
	SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY = 0x20000000, // TITLE Pierce invulnerability DESCRIPTION Allows spell to pierce invulnerability, unless the invulnerability spell also has this attribute
	SPELL_ATTR0_HEARTBEAT_RESIST_CHECK = 0x40000000, // TITLE Periodic resistance checks DESCRIPTION Periodically re-rolls against resistance to potentially expire aura early
	SPELL_ATTR0_CANT_CANCEL = 0x80000000  // TITLE Aura cannot be cancelled DESCRIPTION Prevents the player from voluntarily canceling a positive aura
};

#define MAX_CREATURE_SPELL_DATA_SLOT 4




/// Server rates
enum Rates
{
	RATE_HEALTH = 0,
	RATE_POWER_MANA,
	RATE_POWER_RAGE_INCOME,
	RATE_POWER_RAGE_LOSS,
	RATE_POWER_RUNICPOWER_INCOME,
	RATE_POWER_RUNICPOWER_LOSS,
	RATE_POWER_FOCUS,
	RATE_POWER_ENERGY,
	RATE_SKILL_DISCOVERY,
	RATE_DROP_ITEM_POOR,
	RATE_DROP_ITEM_NORMAL,
	RATE_DROP_ITEM_UNCOMMON,
	RATE_DROP_ITEM_RARE,
	RATE_DROP_ITEM_EPIC,
	RATE_DROP_ITEM_LEGENDARY,
	RATE_DROP_ITEM_ARTIFACT,
	RATE_DROP_ITEM_REFERENCED,
	RATE_DROP_ITEM_REFERENCED_AMOUNT,
	RATE_DROP_MONEY,
	RATE_XP_KILL,
	RATE_XP_BG_KILL,
	RATE_XP_QUEST,
	RATE_XP_EXPLORE,
	RATE_REPAIRCOST,
	RATE_REPUTATION_GAIN,
	RATE_REPUTATION_LOWLEVEL_KILL,
	RATE_REPUTATION_LOWLEVEL_QUEST,
	RATE_REPUTATION_RECRUIT_A_FRIEND_BONUS,
	RATE_CREATURE_NORMAL_HP,
	RATE_CREATURE_ELITE_ELITE_HP,
	RATE_CREATURE_ELITE_RAREELITE_HP,
	RATE_CREATURE_ELITE_WORLDBOSS_HP,
	RATE_CREATURE_ELITE_RARE_HP,
	RATE_CREATURE_NORMAL_DAMAGE,
	RATE_CREATURE_ELITE_ELITE_DAMAGE,
	RATE_CREATURE_ELITE_RAREELITE_DAMAGE,
	RATE_CREATURE_ELITE_WORLDBOSS_DAMAGE,
	RATE_CREATURE_ELITE_RARE_DAMAGE,
	RATE_CREATURE_NORMAL_SPELLDAMAGE,
	RATE_CREATURE_ELITE_ELITE_SPELLDAMAGE,
	RATE_CREATURE_ELITE_RAREELITE_SPELLDAMAGE,
	RATE_CREATURE_ELITE_WORLDBOSS_SPELLDAMAGE,
	RATE_CREATURE_ELITE_RARE_SPELLDAMAGE,
	RATE_CREATURE_AGGRO,
	RATE_REST_INGAME,
	RATE_REST_OFFLINE_IN_TAVERN_OR_CITY,
	RATE_REST_OFFLINE_IN_WILDERNESS,
	RATE_DAMAGE_FALL,
	RATE_AUCTION_TIME,
	RATE_AUCTION_DEPOSIT,
	RATE_AUCTION_CUT,
	RATE_HONOR,
	RATE_ARENA_POINTS,
	RATE_TALENT,
	RATE_CORPSE_DECAY_LOOTED,
	RATE_INSTANCE_RESET_TIME,
	RATE_DURABILITY_LOSS_ON_DEATH,
	RATE_DURABILITY_LOSS_DAMAGE,
	RATE_DURABILITY_LOSS_PARRY,
	RATE_DURABILITY_LOSS_ABSORB,
	RATE_DURABILITY_LOSS_BLOCK,
	RATE_MOVESPEED,
	RATE_MONEY_QUEST,
	RATE_MONEY_MAX_LEVEL_QUEST,
	MAX_RATES
};

enum ItemQualities
{
	ITEM_QUALITY_POOR = 0,                 //GREY
	ITEM_QUALITY_NORMAL = 1,                 //WHITE
	ITEM_QUALITY_UNCOMMON = 2,                 //GREEN
	ITEM_QUALITY_RARE = 3,                 //BLUE
	ITEM_QUALITY_EPIC = 4,                 //PURPLE
	ITEM_QUALITY_LEGENDARY = 5,                 //ORANGE
	ITEM_QUALITY_ARTIFACT = 6,                 //LIGHT YELLOW
	ITEM_QUALITY_HEIRLOOM = 7,
	MAX_ITEM_QUALITY
};


enum Team
{
	HORDE = 67,
	ALLIANCE = 469,
	//TEAM_STEAMWHEEDLE_CARTEL = 169,                       // not used in code
	//TEAM_ALLIANCE_FORCES     = 891,
	//TEAM_HORDE_FORCES        = 892,
	//TEAM_SANCTUARY           = 936,
	//TEAM_OUTLAND             = 980,
	TEAM_OTHER = 0                            // if ReputationListId > 0 && Flags != FACTION_FLAG_TEAM_HEADER
};



enum LockKeyType
{
	LOCK_KEY_NONE = 0,
	LOCK_KEY_ITEM = 1,
	LOCK_KEY_SKILL = 2
};

enum LockType
{
	LOCKTYPE_PICKLOCK = 1,
	LOCKTYPE_HERBALISM = 2,
	LOCKTYPE_MINING = 3,
	LOCKTYPE_DISARM_TRAP = 4,
	LOCKTYPE_OPEN = 5,
	LOCKTYPE_TREASURE = 6,
	LOCKTYPE_CALCIFIED_ELVEN_GEMS = 7,
	LOCKTYPE_CLOSE = 8,
	LOCKTYPE_ARM_TRAP = 9,
	LOCKTYPE_QUICK_OPEN = 10,
	LOCKTYPE_QUICK_CLOSE = 11,
	LOCKTYPE_OPEN_TINKERING = 12,
	LOCKTYPE_OPEN_KNEELING = 13,
	LOCKTYPE_OPEN_ATTACKING = 14,
	LOCKTYPE_GAHZRIDIAN = 15,
	LOCKTYPE_BLASTING = 16,
	LOCKTYPE_SLOW_OPEN = 17,
	LOCKTYPE_SLOW_CLOSE = 18,
	LOCKTYPE_FISHING = 19,
	LOCKTYPE_INSCRIPTION = 20,
	LOCKTYPE_OPEN_FROM_VEHICLE = 21
};


enum QuestSort
{
	QUEST_SORT_EPIC = 1,
	QUEST_SORT_WAILING_CAVERNS_OLD = 21,
	QUEST_SORT_SEASONAL = 22,
	QUEST_SORT_UNDERCITY_OLD = 23,
	QUEST_SORT_HERBALISM = 24,
	QUEST_SORT_BATTLEGROUNDS = 25,
	QUEST_SORT_ULDAMN_OLD = 41,
	QUEST_SORT_WARLOCK = 61,
	QUEST_SORT_WARRIOR = 81,
	QUEST_SORT_SHAMAN = 82,
	QUEST_SORT_FISHING = 101,
	QUEST_SORT_BLACKSMITHING = 121,
	QUEST_SORT_PALADIN = 141,
	QUEST_SORT_MAGE = 161,
	QUEST_SORT_ROGUE = 162,
	QUEST_SORT_ALCHEMY = 181,
	QUEST_SORT_LEATHERWORKING = 182,
	QUEST_SORT_ENGINEERING = 201,
	QUEST_SORT_TREASURE_MAP = 221,
	QUEST_SORT_SUNKEN_TEMPLE_OLD = 241,
	QUEST_SORT_HUNTER = 261,
	QUEST_SORT_PRIEST = 262,
	QUEST_SORT_DRUID = 263,
	QUEST_SORT_TAILORING = 264,
	QUEST_SORT_SPECIAL = 284,
	QUEST_SORT_COOKING = 304,
	QUEST_SORT_FIRST_AID = 324,
	QUEST_SORT_LEGENDARY = 344,
	QUEST_SORT_DARKMOON_FAIRE = 364,
	QUEST_SORT_AHN_QIRAJ_WAR = 365,
	QUEST_SORT_LUNAR_FESTIVAL = 366,
	QUEST_SORT_REPUTATION = 367,
	QUEST_SORT_INVASION = 368,
	QUEST_SORT_MIDSUMMER = 369,
	QUEST_SORT_BREWFEST = 370,
	QUEST_SORT_INSCRIPTION = 371,
	QUEST_SORT_DEATH_KNIGHT = 372,
	QUEST_SORT_JEWELCRAFTING = 373,
	QUEST_SORT_NOBLEGARDEN = 374,
	QUEST_SORT_PILGRIMS_BOUNTY = 375,
	QUEST_SORT_LOVE_IS_IN_THE_AIR = 376
};


enum SkillType
{
	SKILL_NONE = 0,

	SKILL_FROST = 6,
	SKILL_FIRE = 8,
	SKILL_ARMS = 26,
	SKILL_COMBAT = 38,
	SKILL_SUBTLETY = 39,
	SKILL_SWORDS = 43,
	SKILL_AXES = 44,
	SKILL_BOWS = 45,
	SKILL_GUNS = 46,
	SKILL_BEAST_MASTERY = 50,
	SKILL_SURVIVAL = 51,
	SKILL_MACES = 54,
	SKILL_2H_SWORDS = 55,
	SKILL_HOLY = 56,
	SKILL_SHADOW = 78,
	SKILL_DEFENSE = 95,
	SKILL_LANG_COMMON = 98,
	SKILL_RACIAL_DWARVEN = 101,
	SKILL_LANG_ORCISH = 109,
	SKILL_LANG_DWARVEN = 111,
	SKILL_LANG_DARNASSIAN = 113,
	SKILL_LANG_TAURAHE = 115,
	SKILL_DUAL_WIELD = 118,
	SKILL_RACIAL_TAUREN = 124,
	SKILL_ORC_RACIAL = 125,
	SKILL_RACIAL_NIGHT_ELF = 126,
	SKILL_FIRST_AID = 129,
	SKILL_FERAL_COMBAT = 134,
	SKILL_STAVES = 136,
	SKILL_LANG_THALASSIAN = 137,
	SKILL_LANG_DRACONIC = 138,
	SKILL_LANG_DEMON_TONGUE = 139,
	SKILL_LANG_TITAN = 140,
	SKILL_LANG_OLD_TONGUE = 141,
	SKILL_SURVIVAL2 = 142,
	SKILL_RIDING_HORSE = 148,
	SKILL_RIDING_WOLF = 149,
	SKILL_RIDING_TIGER = 150,
	SKILL_RIDING_RAM = 152,
	SKILL_SWIMING = 155,
	SKILL_2H_MACES = 160,
	SKILL_UNARMED = 162,
	SKILL_MARKSMANSHIP = 163,
	SKILL_BLACKSMITHING = 164,
	SKILL_LEATHERWORKING = 165,
	SKILL_ALCHEMY = 171,
	SKILL_2H_AXES = 172,
	SKILL_DAGGERS = 173,
	SKILL_THROWN = 176,
	SKILL_HERBALISM = 182,
	SKILL_GENERIC_DND = 183,
	SKILL_RETRIBUTION = 184,
	SKILL_COOKING = 185,
	SKILL_MINING = 186,
	SKILL_PET_IMP = 188,
	SKILL_PET_FELHUNTER = 189,
	SKILL_TAILORING = 197,
	SKILL_ENGINEERING = 202,
	SKILL_PET_SPIDER = 203,
	SKILL_PET_VOIDWALKER = 204,
	SKILL_PET_SUCCUBUS = 205,
	SKILL_PET_INFERNAL = 206,
	SKILL_PET_DOOMGUARD = 207,
	SKILL_PET_WOLF = 208,
	SKILL_PET_CAT = 209,
	SKILL_PET_BEAR = 210,
	SKILL_PET_BOAR = 211,
	SKILL_PET_CROCILISK = 212,
	SKILL_PET_CARRION_BIRD = 213,
	SKILL_PET_CRAB = 214,
	SKILL_PET_GORILLA = 215,
	SKILL_PET_RAPTOR = 217,
	SKILL_PET_TALLSTRIDER = 218,
	SKILL_RACIAL_UNDED = 220,
	SKILL_CROSSBOWS = 226,
	SKILL_WANDS = 228,
	SKILL_POLEARMS = 229,
	SKILL_PET_SCORPID = 236,
	SKILL_ARCANE = 237,
	SKILL_PET_TURTLE = 251,
	SKILL_ASSASSINATION = 253,
	SKILL_FURY = 256,
	SKILL_PROTECTION = 257,
	SKILL_PROTECTION2 = 267,
	SKILL_PET_TALENTS = 270,
	SKILL_PLATE_MAIL = 293,
	SKILL_LANG_GNOMISH = 313,
	SKILL_LANG_TROLL = 315,
	SKILL_ENCHANTING = 333,
	SKILL_DEMONOLOGY = 354,
	SKILL_AFFLICTION = 355,
	SKILL_FISHING = 356,
	SKILL_ENHANCEMENT = 373,
	SKILL_RESTORATION = 374,
	SKILL_ELEMENTAL_COMBAT = 375,
	SKILL_SKINNING = 393,
	SKILL_MAIL = 413,
	SKILL_LEATHER = 414,
	SKILL_CLOTH = 415,
	SKILL_SHIELD = 433,
	SKILL_FIST_WEAPONS = 473,
	SKILL_RIDING_RAPTOR = 533,
	SKILL_RIDING_MECHANOSTRIDER = 553,
	SKILL_RIDING_UNDEAD_HORSE = 554,
	SKILL_RESTORATION2 = 573,
	SKILL_BALANCE = 574,
	SKILL_DESTRUCTION = 593,
	SKILL_HOLY2 = 594,
	SKILL_DISCIPLINE = 613,
	SKILL_LOCKPICKING = 633,
	SKILL_PET_BAT = 653,
	SKILL_PET_HYENA = 654,
	SKILL_PET_BIRD_OF_PREY = 655,
	SKILL_PET_WIND_SERPENT = 656,
	SKILL_LANG_GUTTERSPEAK = 673,
	SKILL_RIDING_KODO = 713,
	SKILL_RACIAL_TROLL = 733,
	SKILL_RACIAL_GNOME = 753,
	SKILL_RACIAL_HUMAN = 754,
	SKILL_JEWELCRAFTING = 755,
	SKILL_RACIAL_BLOODELF = 756,
	SKILL_PET_EVENT_RC = 758,
	SKILL_LANG_DRAENEI = 759,
	SKILL_RACIAL_DRAENEI = 760,
	SKILL_PET_FELGUARD = 761,
	SKILL_RIDING = 762,
	SKILL_PET_DRAGONHAWK = 763,
	SKILL_PET_NETHER_RAY = 764,
	SKILL_PET_SPOREBAT = 765,
	SKILL_PET_WARP_STALKER = 766,
	SKILL_PET_RAVAGER = 767,
	SKILL_PET_SERPENT = 768,
	SKILL_INTERNAL = 769,
	SKILL_DK_BLOOD = 770,
	SKILL_DK_FROST = 771,
	SKILL_DK_UNHOLY = 772,
	SKILL_INSCRIPTION = 773,
	SKILL_PET_MOTH = 775,
	SKILL_RUNEFORGING = 776,
	SKILL_MOUNTS = 777,
	SKILL_COMPANIONS = 778,
	SKILL_PET_EXOTIC_CHIMAERA = 780,
	SKILL_PET_EXOTIC_DEVILSAUR = 781,
	SKILL_PET_GHOUL = 782,
	SKILL_PET_EXOTIC_SILITHID = 783,
	SKILL_PET_EXOTIC_WORM = 784,
	SKILL_PET_WASP = 785,
	SKILL_PET_EXOTIC_RHINO = 786,
	SKILL_PET_EXOTIC_CORE_HOUND = 787,
	SKILL_PET_EXOTIC_SPIRIT_BEAST = 788
};

#define MAX_SKILL_TYPE               789


constexpr SkillType SkillByLockType(LockType locktype)
{
	switch (locktype)
	{
	case LOCKTYPE_PICKLOCK:    return SKILL_LOCKPICKING;
	case LOCKTYPE_HERBALISM:   return SKILL_HERBALISM;
	case LOCKTYPE_MINING:      return SKILL_MINING;
	case LOCKTYPE_FISHING:     return SKILL_FISHING;
	case LOCKTYPE_INSCRIPTION: return SKILL_INSCRIPTION;
	default: break;
	}
	return SKILL_NONE;
}

constexpr uint32 SkillByQuestSort(int32 QuestSort)
{
	switch (QuestSort)
	{
	case QUEST_SORT_HERBALISM:      return SKILL_HERBALISM;
	case QUEST_SORT_FISHING:        return SKILL_FISHING;
	case QUEST_SORT_BLACKSMITHING:  return SKILL_BLACKSMITHING;
	case QUEST_SORT_ALCHEMY:        return SKILL_ALCHEMY;
	case QUEST_SORT_LEATHERWORKING: return SKILL_LEATHERWORKING;
	case QUEST_SORT_ENGINEERING:    return SKILL_ENGINEERING;
	case QUEST_SORT_TAILORING:      return SKILL_TAILORING;
	case QUEST_SORT_COOKING:        return SKILL_COOKING;
	case QUEST_SORT_FIRST_AID:      return SKILL_FIRST_AID;
	case QUEST_SORT_JEWELCRAFTING:  return SKILL_JEWELCRAFTING;
	case QUEST_SORT_INSCRIPTION:    return SKILL_INSCRIPTION;
	}
	return 0;
}

enum SkillCategory
{
	SKILL_CATEGORY_ATTRIBUTES = 5,
	SKILL_CATEGORY_WEAPON = 6,
	SKILL_CATEGORY_CLASS = 7,
	SKILL_CATEGORY_ARMOR = 8,
	SKILL_CATEGORY_SECONDARY = 9,                       // secondary professions
	SKILL_CATEGORY_LANGUAGES = 10,
	SKILL_CATEGORY_PROFESSION = 11,                      // primary professions
	SKILL_CATEGORY_GENERIC = 12
};


enum RemoveMethod : uint8
{
	GROUP_REMOVEMETHOD_DEFAULT = 0,
	GROUP_REMOVEMETHOD_KICK = 1,
	GROUP_REMOVEMETHOD_LEAVE = 2,
	GROUP_REMOVEMETHOD_KICK_LFG = 3
};



enum GameobjectTypes
{
	GAMEOBJECT_TYPE_DOOR = 0,
	GAMEOBJECT_TYPE_BUTTON = 1,
	GAMEOBJECT_TYPE_QUESTGIVER = 2,
	GAMEOBJECT_TYPE_CHEST = 3,
	GAMEOBJECT_TYPE_BINDER = 4,
	GAMEOBJECT_TYPE_GENERIC = 5,
	GAMEOBJECT_TYPE_TRAP = 6,
	GAMEOBJECT_TYPE_CHAIR = 7,
	GAMEOBJECT_TYPE_SPELL_FOCUS = 8,
	GAMEOBJECT_TYPE_TEXT = 9,
	GAMEOBJECT_TYPE_GOOBER = 10,
	GAMEOBJECT_TYPE_TRANSPORT = 11,
	GAMEOBJECT_TYPE_AREADAMAGE = 12,
	GAMEOBJECT_TYPE_CAMERA = 13,
	GAMEOBJECT_TYPE_MAP_OBJECT = 14,
	GAMEOBJECT_TYPE_MO_TRANSPORT = 15,
	GAMEOBJECT_TYPE_DUEL_ARBITER = 16,
	GAMEOBJECT_TYPE_FISHINGNODE = 17,
	GAMEOBJECT_TYPE_SUMMONING_RITUAL = 18,
	GAMEOBJECT_TYPE_MAILBOX = 19,
	GAMEOBJECT_TYPE_DO_NOT_USE = 20,
	GAMEOBJECT_TYPE_GUARDPOST = 21,
	GAMEOBJECT_TYPE_SPELLCASTER = 22,
	GAMEOBJECT_TYPE_MEETINGSTONE = 23,
	GAMEOBJECT_TYPE_FLAGSTAND = 24,
	GAMEOBJECT_TYPE_FISHINGHOLE = 25,
	GAMEOBJECT_TYPE_FLAGDROP = 26,
	GAMEOBJECT_TYPE_MINI_GAME = 27,
	GAMEOBJECT_TYPE_DO_NOT_USE_2 = 28,
	GAMEOBJECT_TYPE_CAPTURE_POINT = 29,
	GAMEOBJECT_TYPE_AURA_GENERATOR = 30,
	GAMEOBJECT_TYPE_DUNGEON_DIFFICULTY = 31,
	GAMEOBJECT_TYPE_BARBER_CHAIR = 32,
	GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING = 33,
	GAMEOBJECT_TYPE_GUILD_BANK = 34,
	GAMEOBJECT_TYPE_TRAPDOOR = 35
};

#define MAX_GAMEOBJECT_TYPE                  36             // sending to client this or greater value can crash client.
#define MAX_GAMEOBJECT_DATA                  24             // Max number of uint32 vars in gameobject_template data field

enum GameObjectFlags
{
	GO_FLAG_IN_USE = 0x00000001,                   // disables interaction while animated
	GO_FLAG_LOCKED = 0x00000002,                   // require key, spell, event, etc to be opened. Makes "Locked" appear in tooltip
	GO_FLAG_INTERACT_COND = 0x00000004,                   // cannot interact (condition to interact - requires GO_DYNFLAG_LO_ACTIVATE to enable interaction clientside)
	GO_FLAG_TRANSPORT = 0x00000008,                   // any kind of transport? Object can transport (elevator, boat, car)
	GO_FLAG_NOT_SELECTABLE = 0x00000010,                   // not selectable even in GM mode
	GO_FLAG_NODESPAWN = 0x00000020,                   // never despawn, typically for doors, they just change state
	GO_FLAG_AI_OBSTACLE = 0x00000040,                   // makes the client register the object in something called AIObstacleMgr, unknown what it does
	GO_FLAG_FREEZE_ANIMATION = 0x00000080,
	GO_FLAG_DAMAGED = 0x00000200,
	GO_FLAG_DESTROYED = 0x00000400
};

enum GameObjectDynamicLowFlags
{
	GO_DYNFLAG_LO_ACTIVATE = 0x01,                 // enables interaction with GO
	GO_DYNFLAG_LO_ANIMATE = 0x02,                 // possibly more distinct animation of GO
	GO_DYNFLAG_LO_NO_INTERACT = 0x04,                 // appears to disable interaction (not fully verified)
	GO_DYNFLAG_LO_SPARKLE = 0x08,                 // makes GO sparkle
	GO_DYNFLAG_LO_STOPPED = 0x10                  // Transport is stopped
};


// client side GO show states
enum GOState : uint8
{
	GO_STATE_ACTIVE = 0,                        // show in world as used and not reset (closed door open)
	GO_STATE_READY = 1,                        // show in world as ready (closed door close)
	GO_STATE_ACTIVE_ALTERNATIVE = 2                         // show in world as used in alt way and not reset (closed door open by cannon fire)
};




inline char const* localeNames[TOTAL_LOCALES] =
{
  "enUS",
  "koKR",
  "frFR",
  "deDE",
  "zhCN",
  "zhTW",
  "esES",
  "esMX",
  "ruRU"
};

inline LocaleConstant GetLocaleByName(const std::string& name)
{
	for (uint32 i = 0; i < TOTAL_LOCALES; ++i)
		if (name == localeNames[i])
			return LocaleConstant(i);

	return LOCALE_enUS;                                     // including enGB case
}


}
