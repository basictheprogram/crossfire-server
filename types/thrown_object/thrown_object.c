/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2007 Mark Wedel & Crossfire Development Team
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

    The authors can be reached via e-mail at crossfire-devel@real-time.com
*/

/** @file thrown_object.c
 * The implementation of the Thrown Object class of objects.
 */
#include <global.h>
#include <ob_methods.h>
#include <ob_types.h>
#include <sounds.h>

/**
 * Initializer for the THROWN_OBJ object type.
 */
void init_type_thrown_object()
{
    register_move_on(THROWN_OBJ, thrown_object_type_move_on);
}
/**
 * Move on this Thrown Object object.
 * @param context The method context
 * @param trap The Thrown Object we're moving on
 * @param victim The object moving over this one
 * @param originator The object that caused the move_on event
 * @return METHOD_OK
 */
method_ret thrown_object_type_move_on(ob_methods* context, object* trap,
    object* victim, object* originator)
{
    if (common_pre_ob_move_on(trap, victim, originator)==METHOD_ERROR)
        return METHOD_OK;
    if (trap->inv == NULL)
    {
        common_post_ob_move_on(trap, victim, originator);
        return METHOD_OK;
    }
    /* bad bug: monster throw a object, make a step forwards, step on object ,
     * trigger this here and get hit by own missile - and will be own enemy.
     * Victim then is his own enemy and will start to kill herself (this is
     * removed) but we have not synced victim and his missile. To avoid senseless
     * action, we avoid hits here
     */
    if ((QUERY_FLAG (victim, FLAG_ALIVE) && trap->speed) &&
        trap->owner != victim)
        hit_with_arrow(trap, victim);
    common_post_ob_move_on(trap, victim, originator);
    return METHOD_OK;
}
