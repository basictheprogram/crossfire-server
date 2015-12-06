/**
 * @file
 * Object structure, the core of Crossfire.
 */

#include "dialog.h"

#ifndef OBJECT_H
#define OBJECT_H

/** Object tag, unique during the whole game. */
typedef uint32_t tag_t;
#define NUM_BODY_LOCATIONS      13  /**< Number of body locations. */
#define BODY_ARMS               1   /**< This should be the index of the arms. */

/**
 * One body location.
 * See common/item.c.
 */
typedef struct body_locations_struct {
    const char *save_name;      /**< Name used to load/save it to disk */
    const char *use_name;       /**< Name used when describing an item we can use */
    const char *nonuse_name;    /**< Name to describe objects we can't use */
} body_locations_struct;

extern body_locations_struct body_locations[NUM_BODY_LOCATIONS];

extern const char *const move_name[];

/**
 * Each object (this also means archetypes!) could have a few of these
 * "dangling" from it; this could also end up containing 'parse errors'.
 *
 * key and value are shared-strings.
 *
 * Please use object_get_value(), object_set_value() from object.c rather than
 * accessing the list directly.
 * Exception is if you want to walk this list for some reason.
 */
typedef struct _key_value {
    const char *key;          /**< Name of the key. */
    const char *value;        /**< Key's value. */
    struct _key_value *next;  /**< Next key in the list. */
} key_value;

/**
 * @defgroup WILL_APPLY_xxx What monsters apply
 * How monsters should handle some items they have or things on the ground.
 * Those flags correspond to the object::will_apply field.
 */
/*@{*/
#define WILL_APPLY_HANDLE       0x1   /**< Apply handles and triggers. */
#define WILL_APPLY_TREASURE     0x2   /**< Open chests. */
#define WILL_APPLY_EARTHWALL    0x4   /**< Destroy earthwalls. */
#define WILL_APPLY_DOOR         0x8   /**< Open non-locked doors. */
#define WILL_APPLY_FOOD         0x10  /**< Eat food (not drinks). */
/*@}*/

/**
 * Checks if an object still exists.
 * @param op
 * object to check
 * @param old_tag
 * old tag of the object.
 * @return
 * true if the object was destroyed, 0 otherwise
 */
#define object_was_destroyed(op, old_tag) \
    (op->count != old_tag || QUERY_FLAG(op, FLAG_FREED))


/**
 * Defines default size of the *spell_tags pointer.
 * The OB_SPELL_TAG_HASH is a simple mechanism to get/set the
 * spell tags based on a simple hash - it should change if the tag size
 * also changes.  Note that since count is used for this, this value
 * is effectively random or at least fairly evenly distributed, at
 * least in the low bits.  And a size of 16 lets us do a very
 * fast operation.
 */
#define SPELL_TAG_SIZE 16
/**
 * Get the hash on an object for a specified count.
 * @param op what to check.
 * @param count item to check the hash for.
 */
#define OB_SPELL_TAG_HASH(op, count)    (op->spell_tags[count&0xf])
/**
 * Check whether a tag matches in the tags.
 * @param op item to check against.
 * @param count tag to check.
 */
#define OB_SPELL_TAG_MATCH(op, count)   (op->spell_tags[count&0xf] == count)

/**
 * Main Crossfire structure, one ingame object.
 *
 * Note that the ordering of this structure is sort of relevent -
 * object_copy() copies everything over beyond 'name' using memcpy.
 * Thus, values that need to be copied need to be located beyond that
 * point.
 *
 * However, if you're keeping a pointer of some sort, you probably
 * don't just want it copied, so you'll need to add to common/object.c,
 * e.g. copy-object
 *
 * I've tried to clean up this structure a bit (in terms of formatting)
 * by making it more consistent.  I've also tried to locate some of the fields
 * more logically together (put the item related ones together, the monster
 * related ones, etc.
 * This structure is best viewed with about a 100 width screen.
 * MSW 2002-07-05
 *
 * See the @ref page_object "documentation page" for more details.
 */
typedef struct obj {
    /* These variables are not changed by object_copy() */
    struct pl   *contr;         /**< Pointer to the player which control this object */
    struct obj  *next;          /**< Pointer to the next object in the free/used list */
    struct obj  *prev;          /**< Pointer to the previous object in the free/used list*/
    struct obj  *active_next;   /**< Next object in the 'active' list
                                 * This is used in process_events
                                 * so that the entire object list does not
                                 * need to be gone through.*/
    struct obj  *active_prev;   /**< Previous object in the 'active list
                                 * This is used in process_events
                                 * so that the entire object list does not
                                 * need to be gone through. */
    struct obj  *below;         /**< Pointer to the object stacked below this one */
    struct obj  *above;         /**< Pointer to the object stacked above this one */
                                /* Note: stacked in the *same *environment*/
    struct obj  *inv;           /**< Pointer to the first object in the inventory */
    struct obj  *container;     /**< Current container being used.  I think this
                                 * is only used by the player right now. */
    struct obj  *env;           /**< Pointer to the object which is the environment.
                                 * This is typically the container that the object is in. */
    struct obj  *more;          /**< Pointer to the rest of a large body of objects */
    struct obj  *head;          /**< Points to the main object of a large body */
    struct mapdef *map;         /**< Pointer to the map in which this object is present */

    tag_t       count;          /**< Unique object number for this object */
    struct struct_dialog_information *dialog_information; /**< Parsed dialog information for this object.
                                                           * Valid if FLAG_DIALOG_PARSED is set (but can be NULL). */

    /* These get an extra add_refcount(), after having been copied by memcpy().
     * All fields below this point are automatically copied by memcpy.  If
     * adding something that needs a refcount updated, make sure you modify
     * object_copy() to do so.  Everything below here also gets cleared
     * by object_clear()
     */
    sstring     artifact;       /**< If set, the item is the artifact with this name and the matching type. */
    const char  *name;          /**< The name of the object, obviously... */
    const char  *name_pl;       /**< The plural name of the object */
    const char  *anim_suffix;   /**< Used to determine combined animations */
    const char  *title;         /**< Of foo, etc */
    const char  *race;          /**< Human, goblin, dragon, etc */
    const char  *slaying;       /**< Which race to do double damage to.
                                 * If this is an exit, this is the filename */
    const char  *skill;         /**< Name of the skill this object uses/grants */
    const char  *msg;           /**< If this is a book/sign/magic mouth/etc */
    const char  *lore;          /**< Obscure information about this object,
                                 * to get put into books and the like. */

    int16_t      x, y;           /**< Position in the map for this object */
    int16_t      ox, oy;         /**< For debugging: Where it was last inserted */
    float       speed;          /**< The overall speed of this object */
    float       speed_left;     /**< How much speed is left to spend this round */
    float       weapon_speed;   /**< The overall speed of this object */
    float       weapon_speed_left; /**< How much speed is left to spend this round */
    const New_Face    *face;    /**< Face with colors */
    uint32_t      nrof;           /**< How many of the objects */
    int8_t       direction;      /**< Means the object is moving that way. */
    int8_t       facing;         /**< Object is oriented/facing that way. */

    /* This next big block are basically used for monsters and equipment */
    uint8_t       type;           /**< PLAYER, BULLET, etc.  See define.h */
    uint8_t       subtype;        /**< Subtype of object */
    uint16_t      client_type;    /**< Public type information.  see doc/Developers/objects */
    int16_t      resist[NROFATTACKS]; /**< Resistance adjustments for attacks */
    uint32_t      attacktype;     /**< Bitmask of attacks this object does */
    uint32_t      path_attuned;   /**< Paths the object is attuned to */
    uint32_t      path_repelled;  /**< Paths the object is repelled from */
    uint32_t      path_denied;    /**< Paths the object is denied access to */
    const char  *materialname;  /**< Specific material name */
    uint16_t      material;       /**< What materials this object consist of */
    int8_t       magic;          /**< Any magical bonuses to this item */
    uint8_t       state;          /**< How the object was last drawn (animation) */
    int32_t      value;          /**< How much money it is worth (or contains) */
    int16_t      level;          /**< Level of creature or object */

    /* Note that the last_.. values are sometimes used for non obvious
     * meanings by some objects, eg, sp penalty, permanent exp.
     */
    int32_t      last_eat;       /**< How long since we last ate */
    int32_t      last_heal;      /**< Last healed. Depends on constitution */
    int32_t      last_sp;        /**< As last_heal, but for spell points */
    int16_t      last_grace;     /**< As last_sp, except for grace */
    int16_t      invisible;      /**< How much longer the object will be invis */
    uint8_t       pick_up;        /**< See crossfire.doc */
    int8_t       item_power;     /**< Power rating of the object */
    int8_t       gen_sp_armour;  /**< Sp regen penalty this object has (was last_heal)*/
    int8_t       glow_radius;    /**< indicates the glow radius of the object */
    int32_t      weight;         /**< Attributes of the object */
    int32_t      weight_limit;   /**< Weight-limit of object */
    int32_t      carrying;       /**< How much weight this object contains */
    living      stats;          /**< Str, Con, Dex, etc */
    int64_t      perm_exp;       /**< Permanent exp */
    struct obj  *current_weapon; /**< Pointer to the weapon currently used */
    uint32_t      weapontype;     /**< Type of weapon */
    int8_t       body_info[NUM_BODY_LOCATIONS];  /**< Body info as loaded from the file */
    int8_t       body_used[NUM_BODY_LOCATIONS];  /**< Calculated value based on items equipped */
                                /* See the doc/Developers/objects for more info about body locations */

    /* Following mostly refers to fields only used for monsters */
    struct obj  *owner;         /**< Pointer to the object which controls this one.
                                 * Owner should not be referred to directly -
                                 * object_get_owner() should be used instead. */
    tag_t       ownercount;     /**< What count the owner had (in case owner has been freed) */
    struct obj  *enemy;         /**< Monster/player to follow even if not closest */
    struct obj  *attacked_by;   /**< This object start to attack us! only player & monster */
    tag_t       attacked_by_count; /**< The tag of attacker, so we can be sure */
    uint8_t       run_away;          /**< Monster runs away if it's hp goes below this percentage. */
    struct treasureliststruct *randomitems; /**< Items to be generated */
    struct obj  *chosen_skill;  /**< The skill chosen to use */
    uint8_t      hide;           /**< The object is hidden, not invisible */
    /* changes made by kholland@sunlab.cit.cornell.edu */
    /* allows different movement patterns for attackers */
    int32_t      move_status;    /**< What stage in attack mode */
    uint16_t      attack_movement;/**< What kind of attack movement */
    uint8_t       will_apply;     /**< See crossfire.doc and @ref WILL_APPLY_xxx */
    int8_t       sound_chance;   /**< Probability, 0 to 100, of the object emitting a sound. */
    struct obj  *spellitem;     /**< Spell ability monster is choosing to use */
    double      expmul;         /**< needed experience = (calc_exp*expmul) - means some
                                 * races/classes can need less/more exp to gain levels */

    /* Spell related information, may be useful elsewhere
     * Note that other fields are used - these files are basically
     * only used in spells.
     */
    int16_t      casting_time;   /**< Time left before spell goes off */
    int16_t      duration;       /**< How long the spell lasts */
    uint8_t       duration_modifier; /**< how level modifies duration */
    int8_t       range;          /**< Range of the spell */
    uint8_t       range_modifier; /**< How going up in level affects range  */
    uint8_t       dam_modifier;   /**< How going up in level affects damage */
    struct obj  *spell;         /**< Spell that was being cast */
    char        *spellarg;      /**< Optional argument when casting obj::spell. */

    /* Following are values used by any object */
    struct archt *arch;         /**< Pointer to archetype */
    struct archt *other_arch;   /**< Pointer used for various things - mostly used for what
                                 * this objects turns into or what this object creates */
    uint32_t      flags[4];       /**< Various flags */
    uint16_t      animation_id;   /**< An index into the animation array */
    uint8_t       anim_speed;     /**< Ticks between animation-frames */
    uint8_t       last_anim;      /**< Last sequence used to draw face */
    uint16_t      temp_animation_id; /**< An index into the temporary animation array */
    uint8_t       temp_anim_speed; /**< Ticks between temporary animation-frames */
    uint8_t       smoothlevel;    /**< how to smooth this square around*/
    uint8_t       map_layer;      /**< What level to draw this on the map */

    MoveType    move_type;      /**< Type of movement this object uses */
    MoveType    move_block;     /**< What movement types this blocks */
    MoveType    move_allow;     /**< What movement types explicitly allowed */
    MoveType    move_on;        /**< Move types affected moving on to this space */
    MoveType    move_off;       /**< Move types affected moving off this space */
    MoveType    move_slow;      /**< Movement types this slows down */
    float       move_slow_penalty; /**< How much this slows down the object */

    const char  *custom_name;   /**< Custom name assigned by player */
    key_value   *key_values;    /**< Fields not explictly known by the loader. */

    int16_t      *discrete_damage; /**< damage values, based on each attacktype. */
    tag_t       *spell_tags;      /**< Tags used for spell effect merging. */
} object;

/**
 * Used to link together several objects
 */
typedef struct oblnk {
    object *ob;         /**< Item to link to. */
    struct oblnk *next; /**< Next item to link to. */
    tag_t id;           /**< ob's tag, in case it is removed. */
} objectlink;

/**
 * Used to link together several object links
 */
typedef struct oblinkpt {
    struct oblnk *link;   /**< Items for this value. */
    long value;           /**< Used as connected value in buttons/gates */
    struct oblinkpt *next;/**< Next value in the list. */
} oblinkpt;

/**
 * The archetype structure is a set of rules on how to generate and manipulate
 * objects which point to archetypes.
 * This probably belongs in arch.h, but there really doesn't appear to
 * be much left in the archetype - all it really is is a holder for the
 * object and pointers.  This structure should get removed, and just replaced
 * by the object structure
 */
typedef struct archt {
    const char *name;       /**< More definite name, like "generate_kobold" */
    struct archt *next;     /**< Next archetype in a linked list */
    struct archt *head;     /**< The main part of a linked object */
    struct archt *more;     /**< Next part of a linked object */
    object clone;           /**< An object from which to do object_copy() */
    int8_t tail_x, tail_y;   /**< Where the lower right most portion of the object is
                             * in comparison to the head. */
    int reference_count;    /**< How many times this temporary archetype is used. If 0, "permanent" archetype. */
} archetype;

extern object *objects;
extern object *active_objects;
extern object *free_objects;
extern object objarray[STARTMAX];

extern int nrofallocobjects;
extern int nroffreeobjects;

static inline int compare_flags(const object *p, const object *q) {
    return ((p)->flags[0] == (q)->flags[0]) &&
        ((p)->flags[1] == (q)->flags[1]) &&
        ((p)->flags[2] == (q)->flags[2]) &&
        ((p)->flags[3] == (q)->flags[3]);
}

static inline int query_flag(const object *op, int flag) {
    return op->flags[flag / 32] & (1U << (flag % 32));
}

static inline void clear_flag(object *op, int flag) {
    op->flags[flag / 32] &= ~(1U << (flag % 32));
}

static inline void set_flag(object *op, int flag) {
    op->flags[flag / 32] |= (1U << (flag % 32));
}

/**
 * This returns TRUE if the object is something that
 * should be displayed in the look window
 */
#define LOOK_OBJ(ob) (!ob->invisible && ob->type != PLAYER && ob->type != EVENT_CONNECTOR)

/**
 * @defgroup UP_OBJ_xxx Object update flags
 *
 * Used by object_update() to know if the object being passed is
 * being added or removed.
 */
/*@{*/
#define UP_OBJ_INSERT   1   /**< Object was inserted. */
#define UP_OBJ_REMOVE   2   /**< Object was removed. */
#define UP_OBJ_CHANGE   3   /**< Object changed. */
#define UP_OBJ_FACE     4   /**< Only thing that changed was the face. In this case,
                             * we always update everything as that is easier than trying
                             * to look at what may have changed. */
/*@}*/

/**
 * @defgroup FREE_OBJ_xxx Object free flags
 *
 * Used by object_free2() to specify options.
 */
/*@{*/
#define FREE_OBJ_FREE_INVENTORY      1 /**< Free inventory objects; if not set, drop inventory. */
#define FREE_OBJ_NO_DESTROY_CALLBACK 2 /**< Do not run the destroy callback. */
#define FREE_OBJ_DROP_ABOVE_FLOOR    4 /**< If FREE_OBJ_FREE_INVENTORY is not set, drop inventory just above ground instead on top. */
/*@}*/

/**
 * @defgroup INS_xxx Object insertion flags.
 *
 * These are flags passed to object_insert_in_map() and
 * object_insert_in_ob().  Note that all flags may not be meaningful
 * for both functions.
 * Most are fairly explanatory:
 * - INS_NO_MERGE: don't try to merge inserted object with ones alrady
 *    on space.
 * - INS_ABOVE_FLOOR_ONLY: Put object immediatly above the floor.
 * - INS_NO_WALK_ON: Don't call check_walk_on against the
 *    originator - saves cpu time if you know the inserted object
 *    is not meaningful in terms of having an effect.
 * - INS_ON_TOP: Always put object on top.  Generally only needed when loading
 *     files from disk and ordering needs to be preserved.
 * - INS_BELOW_ORIGINATOR: Insert new object immediately below originator -
 *     Use for treasure chests so the new object is the highest thing
 *     beneath the player, but not actually above it.  Note - the
 *     map and x,y coordinates for the object to be inserted must
 *     match the originator.
 * - INS_MAP_LOAD: disable lots of checkings done at insertion to
 *     speed up map loading process, as we assume the ordering in
 *     loaded map is correct.
 *
 * Note that INS_BELOW_ORIGINATOR, INS_ON_TOP, INS_ABOVE_FLOOR_ONLY
 * are mutually exclusive.  The behaviour for passing more than one
 * should be considered undefined - while you may notice what happens
 * right now if you pass more than one, that could very well change
 * in future revisions of the code.
 */
/*@{*/
#define INS_NO_MERGE            0x0001  /**< Don't try to merge with other items. */
#define INS_ABOVE_FLOOR_ONLY    0x0002  /**< Put object immediatly above the floor. */
#define INS_NO_WALK_ON          0x0004  /**< Don't call check_walk_on against the originator. */
#define INS_ON_TOP              0x0008  /**< Always put object on top. */
#define INS_BELOW_ORIGINATOR    0x0010  /**< Insert new object immediately below originator. */
#define INS_MAP_LOAD            0x0020  /**< Disable lots of checkings. */
/*@}*/

#define ARCH_SINGULARITY        "singularity"   /**< Archetype for singularity. */
#define ARCH_SINGULARITY_LEN    11              /**< Length of ::ARCH_SINGULARITY. */
#define ARCH_DETECT_MAGIC       "detect_magic"  /**< Archetype for detect magic spell. */
#define ARCH_DEPLETION          "depletion"     /**< Archetype for depletion. */
#define ARCH_SYMPTOM            "symptom"       /**< Archetype for disease symptom. */
#define ARCH_PORTAL_FAILED      "town_portal_failed"    /**< Archetype for town portal failure. */
#define ARCH_SPELL_BLOCKED      "spell_blocked" /**< Archetype when a spell is blocked (unholy ground or non magic). */
#define ARCH_SPELL_BUNGLE       "spell_bungle"  /**< Archetype when player bungles a spell. */

/**
 * Returns the head part of an object. For single-tile objects returns the
 * object itself.
 *
 * @param op
 * the object
 * @return
 * the head object
 */
#define HEAD(op) ((op)->head != NULL ? (op)->head : (op))

#endif /* OBJECT_H */
