/*
 * static char *rcsid_object_h =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2000 Mark Wedel
    Copyright (C) 1992 Frank Tore Johansen

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    The author can be reached via e-mail to mwedel@scruz.net
*/

#define BUFADD(xyz,abc) { strcat(buf,variable_const[abc]);strcat(buf,((xyz)?" 1\n":" 0\n")); }

enum varnrs {
  V_OBJECT,V_NAME,V_RACE,V_SLAYING,V_MSG,V_MSGEND,
  V_INVENTORY,V_ARCH,V_OTHER_ARCH,V_MORE,
  V_ANIMATION,V_END,V_LAST_HEAL,V_LAST_SP,V_LAST_GRACE,V_LAST_EAT,
  V_SPEED,V_SPEED_LEFT,V_SLOW_MOVE,
  V_FACE,V_STR,V_DEX,V_CON,V_WIS,V_CHA,V_INT,V_POW,V_HP,V_MAXHP,V_SP,V_MAXSP,
  V_GRACE,V_MAXGRACE,
  V_EXP,V_FOOD,V_DAM,V_WC,V_AC,V_X,V_Y,V_NROF,V_LEVEL,V_DIRECTION,
  V_TYPE,V_MATERIAL,V_VALUE,V_WEIGHT,V_CARRYING,
  V_IMMUNE,V_PROTECTED,V_ATTACKTYPE,V_VULNERABLE,
  V_PATH_ATTUNED, V_PATH_REPELLED, V_PATH_DENIED,
  V_INVISIBLE,V_MAGIC,V_STATE,V_ALIVE,V_APPLIED,V_UNPAID,V_NEED_AN,V_NEED_IE,
  V_NO_PICK,V_NO_PASS,V_WALK_ON,V_WALK_OFF,V_FLY_ON,V_FLY_OFF,V_IS_ANIMATED,
  V_FLYING,V_MONSTER,V_FRIENDLY,V_GENERATOR,V_IS_THROWN,V_AUTO_APPLY,V_TREASURE,
  V_APPLY_ONCE,V_SEE_INVISIBLE,V_CAN_ROLL,V_IS_TURNING,V_IS_TURNABLE,
  V_IS_USED_UP,V_IDENTIFIED,V_REFLECTING,V_CHANGING,V_SPLITTING,V_HITBACK,
  V_STARTEQUIP,V_BLOCKSVIEW,V_EDITABLE,V_UNDEAD,V_SCARED,V_UNAGGRESSIVE,
  V_REFLECT_MISSILE,V_REFLECT_SPELL,V_NO_MAGIC,
  V_WIZ,V_WAS_WIZ,V_NO_FIX_PLAYER,V_TEAR_DOWN,V_LUCK,V_RUN_AWAY,V_PASS_THRU,
  V_CAN_PASS_THRU,V_PICK_UP,V_ANIM_SPEED,V_CONTAINER,V_NO_DROP,
  V_NO_PRETEXT,V_WILL_APPLY,V_RANDOM_MOVEMENT,V_CAN_APPLY,
  V_CAN_CAST_SPELL,V_CAN_USE_SCROLL,V_CAN_USE_WAND,V_CAN_USE_BOW,
  V_CAN_USE_ARMOUR,V_CAN_USE_WEAPON,V_CAN_USE_RING,V_HAS_READY_WAND,
  V_HAS_READY_BOW,V_XRAYS,V_IS_FLOOR,V_LIFESAVE,V_NO_STRENGTH,
  V_SLEEP,V_STAND_STILL,V_RANDOM_MOVE,V_ONLY_ATTACK,V_ARMOUR,
  V_ATT_MOVE,V_MOVE_STATUS,V_CONFUSED,V_STEALTH,V_CONNECTED,
  V_CURSED,V_DAMNED,V_SEE_ANYWHERE,V_KNOWN_MAGICAL,V_KNOWN_CURSED,
  V_CAN_USE_SKILL,V_BEEN_APPLIED,V_TITLE,V_HAS_READY_ROD,V_CAN_USE_ROD,
  V_HAS_READY_HORN,V_CAN_USE_HORN,V_EXPMUL,V_UNIQUE,V_MAKE_INVIS,V_INV_LOCKED,
  V_IS_WOODED,V_IS_HILLY,V_HAS_READY_SKILL,V_HAS_READY_WEAPON,
  V_NO_SKILL_IDENT,V_GLOW_RADIUS,V_BLIND,V_SEE_IN_DARK,
  V_IS_CAULDRON,V_RANDOMITEMS,V_DUST,V_NO_STEAL, V_ONE_HIT, V_BERSERK, 
  NR_OF_VARIABLES
};

#define LL_IGNORED  -1
#define LL_EOF	    0
#define LL_MORE	    1
#define LL_NORMAL   2

/* see loader.l for more details on this */
#define LO_REPEAT   0
#define LO_LINEMODE 1
#define LO_NEWFILE  2
#define	LO_NOREAD   3

extern char *variables[NR_OF_VARIABLES];

extern char **xbm_names;
extern int *xbm_values;
extern int nroffiles, nrofpixmaps;
