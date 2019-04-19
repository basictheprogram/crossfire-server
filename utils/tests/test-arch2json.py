from collections import deque
import json
import os
from nose.tools import set_trace, raises

from arch2json import (
    Animation,
    Arch2Json,
    arc_to_face_png,
    arc_filename,
    created_timestamp,
    DjangoJsonDump,
    exists,
    Field,
    face_png_list,
    filter_walk,
    Items,
    Message,
    model_from_path,
    relative_path,
    updated_timestamp,
)

from arch2xml import Walk


def test_arc_filename1():
    k, v = arc_filename('/this/is/a/path/item.arc')

    assert k == 'arc_filename'
    assert v == 'item.arc'


def test_arc_filename2():
    k, v = arc_filename('/this/is/a/path/item.arc', 'foobar')

    assert k == 'foobar'
    assert v == 'item.arc'


def test_created_timestamp():
    k, v = created_timestamp()

    assert k == 'created'
    # How to test a datetime.datetime.now()?


def test_updated_timestamp():
    k, v = updated_timestamp()

    assert k == 'updated'
    # How to test a datetime.datetime.now()?


def test_model1():
    pass


def test_model2():
    parser = Arch2Json()
    parser.model = 'items.spells'

    assert(parser.model == 'items.spells')


def test_keys1():
    contents = ''.splitlines()
    parser = Arch2Json()
    results = parser.keys(contents)

    assert(results.len <= 0)


def test_comments1():
    contents = '# This is a single line comment'

    contents = contents.splitlines()
    parser = Arch2Json()
    results = parser.keys(contents)

    assert(results.len <= 0)


def test_comments2():
    contents = '''# This is a multi-line comment1
# This is a multi-line comment2
# This is a multi-line comment3
# This is a multi-line comment4'''

    contents = contents.splitlines()
    parser = Arch2Json()
    results = parser.keys(contents)

    assert(results.len <= 0)


def test_comments3():
    contents = '''Object spell_reincarnation
anim_suffix spellcasting
name reincarnation
name_pl reincarnation
face spell_reincarnation.111
type 101
subtype 1
level 25
value 250
grace 350
casting_time 30
path_attuned 256
skill praying
no_drop 1
invisible 1
exp 20
# tanneritems reincarnation_failure
race reincarnation_races
msg
Brings a slain character back to life, as with the resurrection spell, but without requiring a corpse, and usually as a different race, with all the benefits and penalties associated with it.  Gaea grants this spell, and it is only useful on permanent death servers.
endmsg
end'''

    # import pdb
    # pdb.set_trace()

    contents = contents.splitlines()
    parser = Arch2Json()
    fields = parser.keys(contents)

    assert(fields.find('object', 'spell_reincarnation'))
    assert(not fields.find_key('reincarnation_failure'))
    assert(not fields.find_key('tanneritems'))
    assert(not fields.find_key('#tanneritems'))
    assert(not fields.find_value('reincarnation_failure'))
    assert(not fields.find_value('tanneritems'))
    assert(not fields.find_value('#tanneritems'))


def test_keys4():
    contents = '''# Acid spehres are sort of odd creatures - they are really
# only a danger if the player is not careful and runs into
# them - they otherwise move slowly and are easily killed.
# But they do a lot of damage if a player does run into one.
Object acid_sphere
end'''

    contents = contents.splitlines()
    parser = Arch2Json()
    fields = parser.keys(contents)

    assert(fields.len == 1)
    assert(fields.find('object', 'acid_sphere'))


def test_keys5():
    contents = '''# Acid spehres are sort of odd creatures - they are really
# only a danger if the player is not careful and runs into
# them - they otherwise move slowly and are easily killed.
# But they do a lot of damage if a player does run into one.
Object acid_sphere
race slime
end'''

    contents = contents.splitlines()
    parser = Arch2Json()
    fields = parser.keys(contents)

    assert(fields.find('object', 'acid_sphere'))
    assert(fields.find('race', 'slime'))


def test_keys6():
    contents = '''# Acid spehres are sort of odd creatures - they are really
# only a danger if the player is not careful and runs into
# them - they otherwise move slowly and are easily killed.
# But they do a lot of damage if a player does run into one.
Object acid_sphere
race slime
name acid sphere
face acidsphere.111
animation acid_sphere
monster 1
move_type walk
sleep 1
Wis 5
alive 1
ac 5
wc 1
dam 50
weight 1
level 4
resist_fire 100
resist_electricity 100
resist_cold 100
resist_confusion 100
resist_acid 100
resist_drain 100
resist_weaponmagic 100
resist_ghosthit 100
resist_poison 100
resist_slow 100
resist_paralyze 100
resist_turn_undead 100
resist_blind 100
attacktype 64
hitback 1
hp 1
maxhp 1
speed 0.01
anim_speed 1
exp 100
one_hit 1
end'''

    contents = contents.splitlines()
    parser = Arch2Json()
    fields = parser.keys(contents)

    assert(fields.find('object', 'acid_sphere'))
    assert(fields.find('race', 'slime'))
    assert(fields.find('face', 'acidsphere.111'))
    assert(fields.find('resist_weaponmagic', '100'))
    assert(fields.find('speed', '0.01'))
    assert(fields.find('exp', '100'))
    assert(fields.find('one_hit', '1'))


def test_message1():
    contents = '''Object dwarf_wiz
race dwarf
tanneritems tanner_books
name dwarf wizard
animation dwarf_wiz
msg
@match *
I'm too busy to answer your queries.
endmsg
face dwarf_wiz.111
Pow 18
sp 50
maxsp 50
monster 1
move_type walk
unaggressive 1
alive 1
ac 10
wc 15
dam 5
hp 38
maxhp 38
exp 10
speed 0.06
weight 50000
level 15
can_cast_spell 1
end'''

    contents = contents.splitlines()
    parser = Arch2Json()
    fields = parser.keys(contents)
    msg = [
        "@match *",
        "I'm too busy to answer your queries."
    ]

    assert(fields.find('object', 'dwarf_wiz'))
    assert(fields.find('msg', msg))
    assert(fields.find('monster', '1'))
    assert(fields.find('maxsp', '50'))


def test_message2():
    contents = '''Object spell_magic_missile
anim_suffix spellcasting
name magic missile
name_pl magic missile
face spell_magic_missile.111
level 1
sp 1
casting_time 3
path_attuned 16
other_arch magic_missile
dam 9
dam_modifier 1
maxsp 20
skill sorcery
type 101
subtype 11
value 10
attacktype 2
no_drop 1
invisible 1
range 25
msg
Fires a target-tracking magical bolt that can turn to seek out enemies.  The bolt does not always track well and may run into walls or even return and strike the caster.
endmsg
end'''

    # import pdb
    # pdb.set_trace()

    contents = contents.splitlines()
    parser = Arch2Json()
    fields = parser.keys(contents)
    msg = [
        "Fires a target-tracking magical bolt that can turn to seek out enemies.  The bolt does not always track well and may run into walls or even return and strike the caster."
    ]

    assert(fields.find('object', 'spell_magic_missile'))
    assert(not fields.find_key('Fires'))
    assert(not fields.find_key('fires'))
    assert(not fields.find_value('Fires'))
    assert(not fields.find_value('fires'))
    assert(fields.find('msg', msg))


def test_message3():
    contents = '''Object magic_ear
type 29
face magic_ear.111
msg
@match sesame
Click.
endmsg
no_pick 1
invisible 1
end
'''

    # import pdb
    # pdb.set_trace()

    contents = contents.splitlines()
    parser = Arch2Json()
    fields = parser.keys(contents)
    msg = [
        '@match sesame',
        'Click.'
    ]

    assert(fields.find('object', 'magic_ear'))
    assert(not fields.find_key('Click.'))
    assert(not fields.find_key('click'))
    assert(not fields.find_value('Click.'))
    assert(not fields.find_value('click'))
    assert(fields.find('msg', msg))


def test_animation1():
    contents = '''Object werewolf
face 1728
anim
1728
1729
1728
1728
1730
1728
mina
monster 1
Wis 10
alive 1
hp 50
maxhp 50
Con 10
speed 0.10
exp 100
ac 5
dam 8
wc 1
level 5
protected 1
weight 80000
run_away 15
pick_up 64
can_use_weapon 1
attacktype 3
end'''

    contents = contents.splitlines()
    parser = Arch2Json()
    fields = parser.keys(contents)
    anim = [
        '1728',
        '1729',
        '1728',
        '1728',
        '1730',
        '1728'
    ]

    assert(fields.find('object', 'werewolf'))
    assert(fields.find('anim', anim))


def test_message5():
    contents = '''Object spell_magic_missile
msg
Fires a target-tracking magical bolt that can turn to seek out enemies.  The bolt does not always track well and may run into walls or even return and strike the caster.
endmsg
end'''

   # import pdb
   # pdb.set_trace()

    contents = contents.splitlines()
    parser = Arch2Json()
    fields = parser.keys(contents)
    msg = [
        "Fires a target-tracking magical bolt that can turn to seek out enemies.  The bolt does not always track well and may run into walls or even return and strike the caster."
    ]

    assert(fields.find('object', 'spell_magic_missile'))
    assert(not fields.find_key('Fires'))
    assert(not fields.find_key('fires'))
    assert(not fields.find_value('Fires'))
    assert(not fields.find_value('fires'))
    assert(fields.find('msg', msg))


def test_message6():
    contents = '''Object goldgrass
name drop 10 gold coins
slaying goldcoin
msg
Click!
endmsg
type 18
activate_on_push 1
activate_on_release 1
face goldgrass.111
food 10
animation goldgrass
no_pick 1
move_on walk
is_floor 1
end
'''

    #import pdb
    # pdb.set_trace()

    contents = contents.splitlines()
    parser = Arch2Json()
    fields = parser.keys(contents)
    msg = [
        'Click!',
    ]

    assert(fields.find('object', 'goldgrass'))
    assert(not fields.find_key('Click!'))
    assert(not fields.find_key('click!'))
    assert(fields.find('msg', msg))


def test_from_file1():
    BASE_PATH = os.path.dirname(os.path.dirname((os.path.abspath(__file__))))

    file = BASE_PATH + '/tests/dragonskin_boots.arc'
    with open(file, 'r') as arc:
        contents = arc.read().splitlines()

    parser = Arch2Json()
    fields = parser.keys(contents)

    assert(fields.find('object', 'dragonskin_boots'))
    assert(fields.find('name', 'dragonskin boots'))


def test_from_file2():
    BASE_PATH = os.path.dirname(os.path.dirname((os.path.abspath(__file__))))

    files = [
        '/tests/dragonskin_boots.arc',
        '/tests/elvenboots.arc',
        '/tests/golem_shoes.arc'
    ]

    parser = Arch2Json()

    for file in files:
        with open(BASE_PATH+file, 'r') as arc:
            contents = arc.read().splitlines()

        fields = parser.keys(contents)

    assert(fields.find('object', 'dragonskin_boots'))
    assert(fields.find('object', 'elvenboots'))
    assert(fields.find('object', 'golem_shoes'))


def test_multiple_objects():
    BASE_PATH = os.path.dirname(os.path.dirname((os.path.abspath(__file__))))
    file = BASE_PATH + '/tests/emptybottles.arc'

    parser = Arch2Json()

    with open(file, 'r') as arc:
        contents = arc.read().splitlines()

    fields = parser.keys(contents)

    assert(fields.find('object', 'coffee_empty'))
    assert(fields.find('object', 'w_glass_empty'))
    assert(fields.find('object', 'boozebottle_empty'))
    assert(fields.find('object', 'winebottle_empty'))
    assert(fields.find('object', 'wbottle_empty'))
    assert(fields.find('object', 'potion_empty'))
    assert(fields.find('object', 'vial_empty'))
    assert(fields.find('object', 'bag_empty'))

    #print(json.dumps(fields.items, indent=4))


def test_multiple_objects2():
    BASE_PATH = os.path.dirname(os.path.dirname((os.path.abspath(__file__))))
    file = BASE_PATH + '/tests/emptybottles.arc'

    parser = Arch2Json()

    with open(file, 'r') as arc:
        contents = arc.read().splitlines()

    items = parser.keys(contents)
    items.add_field('arch_filename', file)

    k, v = created_timestamp()
    items.add_field(k, v)

    k, v = updated_timestamp()
    items.add_field(k, v)

    assert(items.find('object', 'coffee_empty'))
    assert(items.find('object', 'w_glass_empty'))
    assert(items.find('object', 'boozebottle_empty'))
    assert(items.find('object', 'winebottle_empty'))
    assert(items.find('object', 'wbottle_empty'))
    assert(items.find('object', 'potion_empty'))
    assert(items.find('object', 'vial_empty'))
    assert(items.find('object', 'bag_empty'))
    assert(items.find('arch_filename', file))
    assert(items.find_key('created'))
    assert(items.find_key('updated'))

    fields = Field()
    fields.add('model', 'items.potion')

    assert(fields.find_key('fields'))
    assert(fields.find_key('model'))

    _list = []
    for item in items.items:
        fields.add_field(item)
        _list.append(fields.field)

    # print(json.dumps(_list, indent=4))


def add_items1():
    _dict = {
        'a': 1,
        'b': 2,
        'c': 3,
        'd': 4
    }
    _dict2 = {
        'e': 1,
        'f': 2,
        'g': 3,
        'h': 4
    }

    items = Items()
    items.add(_dict)
    items.add(_dict2)

    items.add_field('created', 'timestamp')
    assert(items.find('created', 'timestamp'))

    dict1 = items.item(0)
    dict2 = items.item(1)
    assert(dict1['created'] == 'timestamp')
    assert(dict2['created'] == 'timestamp')


def test_add_items2():
    BASE_PATH = os.path.dirname(os.path.dirname((os.path.abspath(__file__))))
    file = BASE_PATH + '/tests/emptybottles.arc'

    parser = Arch2Json()

    with open(file, 'r') as arc:
        contents = arc.read().splitlines()

    fields = parser.keys(contents)

    k, v = created_timestamp()
    fields.add_field(k, v)

    assert(fields.find('object', 'coffee_empty'))
    assert(fields.find('object', 'w_glass_empty'))
    assert(fields.find('object', 'boozebottle_empty'))
    assert(fields.find('object', 'winebottle_empty'))
    assert(fields.find('object', 'wbottle_empty'))
    assert(fields.find('object', 'potion_empty'))
    assert(fields.find('object', 'vial_empty'))
    assert(fields.find('object', 'bag_empty'))
    assert(fields.find_key('created'))

    item = fields.item(0)
    assert(item['object'] == 'coffee_empty')
    assert('created' in item)
    # print(json.dumps(fields.items, indent=4))


def test_fields1():
    field = Field()
    _dict = {
        'a': 1,
        'b': 2,
        'c': 3,
        'd': 4
    }

    field.add_field(_dict)
    assert(field.find_key('fields'))
    assert(not field.find_key('fields2'))

    # print(json.dumps(field.field, indent=4))


def test_list1():
    field = Field()
    field2 = Field()

    _dict = {
        'a': 1,
        'b': 2,
        'c': 3,
        'd': 4
    }
    _dict2 = {
        'e': 1,
        'f': 2,
        'g': 3,
        'h': 4
    }

    field.add_field(_dict)
    field.add('model', 'items.weapons')
    field2.add_field(_dict2)

    fields = []
    fields.append(field.field)
    fields.append(field2.field)

    assert(field.find_key('fields'))
    assert(field.find_key('model'))

    assert(field2.find_key('fields'))
    assert(not field2.find_key('model'))

    # print(json.dumps(fields, indent=4))


def test_from_file3():
    BASE_PATH = os.path.dirname(os.path.dirname((os.path.abspath(__file__))))

    file = BASE_PATH + '/tests/dragonskin_boots.arc'
    with open(file, 'r') as arc:
        contents = arc.read().splitlines()

    parser = Arch2Json()
    items = parser.keys(contents)
    items.add_field('arch_filename', file)

    k, v = created_timestamp()
    items.add_field(k, v)

    k, v = updated_timestamp()
    items.add_field(k, v)

    assert(items.find('object', 'dragonskin_boots'))
    assert(items.find('name', 'dragonskin boots'))
    assert(items.find('arch_filename', file))
    assert(items.find_key('created'))
    assert(items.find_key('updated'))

    field = Field()
    field.add('model', 'items.monster')

    for item in items.items:
        field.add_field(item)

    # print(json.dumps(field.field, indent=4))


def test_auto_model1():
    sub_folders = [
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/armour',
    ]

    for folder in sub_folders:
        model = model_from_path(folder, 'items')

    assert(model == 'items.armour')


def test_auto_model2():
    sub_folders = [
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/connect2',
    ]

    for folder in sub_folders:
        model = model_from_path(folder, 'items')
    assert(not model)


def test_auto_model3():
    sub_folders = [
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/connect/Gates/gate.arc'
    ]

    for folder in sub_folders:
        model = model_from_path(folder, 'items')

    assert(model == 'items.connect')


def test_auto_model4():
    sub_folders = [
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/weapon',
    ]

    for folder in sub_folders:
        model = model_from_path(folder, 'items')

    assert(model == 'items.weapon')


def test_auto_model5():
    sub_folders = [
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/monster/misc/serpmen/serp_priest.arc',
    ]

    for folder in sub_folders:
        model = model_from_path(folder, 'items')
    assert(model == 'items.monster')


def test_auto_model10():
    sub_folders = [
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/armour',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/connect',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/construct',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/dev',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/disease',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/door',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/exit',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/flesh',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/floor',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/food',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/gods',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/ground',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/indoor',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/inorganic',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/jewel',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/light',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/mapbuilding',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/misc',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/monster',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/planes',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/player',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/potion',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/random',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/readable',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/river',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/road',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/shop',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/skills',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/spell',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/system',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/talisman',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/transport',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/traps',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/wall',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/weapon',
    ]

    for folder in sub_folders:
        model = model_from_path(folder, 'items')
        assert(model)


def test_relative_path1():
    full_path = '/Users/tanner/projects/crossfire/crossfire-arch/trunk/armour/bracers/bracersdex.arc'
    trim_path = '/Users/tanner/projects/crossfire/crossfire-arch/trunk/'
    result = relative_path(full_path, trim_path)

    assert(result == 'armour/bracers/bracersdex.arc')


def test_relative_path2():
    full_path = '/Users/tanner/projects/crossfire/crossfire-arch/trunk/armour/bracers/bracersdex.arc'
    trim_path = '/Users/tanner/projects/crossfire/crossfire-arch/trunk/'
    face_filename = 'bracersdex.base.111'
    face_path = 'armour/bracers/' + face_filename + '.png'

    result = relative_path(full_path, trim_path)
    assert(result == 'armour/bracers/bracersdex.arc')

    face = arc_to_face_png(face_filename, result)
    assert(face == face_path)


def test_exists():
    assert(not exists('http://www.fakedomain.com/fakeImage.jpg'))
    assert(exists('http://www.google.com'))


def test_face_png_list1():
    trunk = '/Users/tanner/projects/crossfire/crossfire-arch/trunk'
    graphics = Walk(trunk, 1, '*.png', 1)

    obj = 'magnifier'
    face = '/misc/magnifier.111.png'

    png_list = face_png_list(face, graphics)
    #print('trunk', trunk)
    #print('png_list', png_list)

    assert(len(png_list) >= 0)

    # If a component is an absolute path, all previous components are thrown away
    # and joining continues from the absolute path component.
    #
    for png in png_list:
        assert(png == os.path.join(trunk, 'misc/magnifier.base.111.png'))


def test_face_png_list2():
    trunk = '/Users/tanner/projects/crossfire/crossfire-arch/trunk'
    graphics = filter_walk(trunk, 1, '*.png', 1, '/dev/')
    for g in graphics:
        if 'jessyb' in g:
            # print(g)
            pass

    obj = 'Balrog'
    #face = 'monster/demon/jessyb.x11.png'
    face = 'jessyb.x11.png'
    png_list = face_png_list(face, graphics)

    # png_list = [
    #    'monster/giant/JessyB/jessyb.base.x11.png',
    #    'monster/giant/JessyB/jessyb.clsc.x13.png',
    #    'monster/giant/JessyB/jessyb.clsc.x12.png',
    #    'monster/giant/JessyB/jessyb.base.x12.png',
    #    'monster/giant/JessyB/jessyb.clsc.x11.png',
    #    'monster/giant/JessyB/jessyb.base.x13.png',
    # ]

    assert(len(png_list) >= 0)
    print('xxx', png_list)


def test_face_png_list3():
    trunk = '/Users/tanner/projects/crossfire/crossfire-arch/trunk'
    graphics = Walk(trunk, 1, '*.png', 1)

    obj = 'egg_disease'
    face = ''

    png_list = face_png_list(face, graphics)
    #print('trunk', trunk)
    #print('png_list', png_list)

    assert(len(png_list) <= 0)


def test_face_png_list3():
    trunk = '/Users/tanner/projects/crossfire/crossfire-arch/trunk'
    graphics = filter_walk(trunk, 1, '*.png', 1, '/dev/')

    obj = 'dwarf_player'
    face = '/player/race/dwarf_p.151.png'

    dwarf_faces = [
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/player/race/dwarf_p.clsc.131.png',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/player/race/dwarf_p.base.132.png',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/player/race/dwarf_p.clsc.132.png',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/player/race/dwarf_p.base.131.png',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/player/race/dwarf_p.clsc.152.png',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/player/race/dwarf_p.base.151.png',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/player/race/dwarf_p.clsc.151.png',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/player/race/dwarf_p.base.152.png',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/player/race/dwarf_p.base.171.png',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/player/race/dwarf_p.clsc.172.png',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/player/race/dwarf_p.base.172.png',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/player/race/dwarf_p.clsc.171.png',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/player/race/dwarf_p.base.112.png',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/player/race/dwarf_p.clsc.111.png',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/player/race/dwarf_p.base.111.png',
        '/Users/tanner/projects/crossfire/crossfire-arch/trunk/player/race/dwarf_p.clsc.112.png'
    ]
    png_list = face_png_list(face, graphics)

    assert(len(png_list) > 1)
    assert(len(png_list) == len(dwarf_faces))

    png_set = set(png_list)
    dwarf_faces_set = set(dwarf_faces)

    assert(png_set == dwarf_faces_set)

    diff = png_set.difference(dwarf_faces_set)
    assert(diff == set())


def test_django_find1():
    django = DjangoJsonDump(model='items.facepng')
    django.create_fields_and_model('fields', 'items.facepng')
    field = django.find_key('fields')
    model = django.find_key('model')


@raises(ValueError)
def test_django_find2():
    django = DjangoJsonDump(model='items.facepng')
    field = django.find_key('nope')


def test_django_json1():
    expected = '[{"fields": {}, "model": "items.facepng"}]'
    django = DjangoJsonDump('items.facepng')
    django.create_fields_and_model('fields', 'items.facepng')
    result = json.dumps(django.list)
    assert(result == expected)


def test_django_json2():
    #import pdb
    # pdb.set_trace()
    expected = '[{"fields": {"obj": "dwarf"}, "model": "items.facepng"}]'
    django = DjangoJsonDump(model='items.facepng')
    django.create_fields_and_model('fields', 'items.facepng')
    django.add_kv_to_fields("obj", "dwarf")
    result = json.dumps(django.list)
    assert(result == expected)


def test_django_json3():
    expected = '[{"fields": {"obj": "dwarf", "face": "dwarf_p.151", "png": "player/race/dwarf_p.base.132.png"}, "model": "items.facepng"}]'
    django = DjangoJsonDump(model='items.facepng')
    django.create_fields_and_model('fields', 'items.facepng')
    django.add_kv_to_fields('obj', 'dwarf')
    django.add_kv_to_fields('face', 'dwarf_p.151')
    django.add_kv_to_fields('png', 'player/race/dwarf_p.base.132.png')
    result = json.dumps(django.list)
    assert(result == expected)


def test_django_json5():
    expected1 = '[{"model": "items.testing"}, {"fields": {}, "model": "items.facepng"}]'
    expected2 = '[{"model": "items.testing"}, {"fields": {}, "model": "items.testing2"}]'
    django = DjangoJsonDump(model='items.facepng')
    django.create_fields_and_model('fields', 'items.facepng')
    django.create_model('items.testing')
    result1 = json.dumps(django.list)
    assert(result1 == expected1)

    django.update_model(model='items.testing2', index=1)
    result2 = json.dumps(django.list)
    assert(result2 == expected2)


def test_remove_dev_from_walk1():
    root = '/Users/tanner/projects/crossfire/crossfire-arch/trunk/'
    files = Walk(root, 1, '*.arc', 1)
    result = [arc for arc in files if "/dev/" not in arc]
    assert("/dev/" not in result)
    assert("/Dev/" not in result)


def test_remove_dev_from_walk2():
    root = '/Users/tanner/projects/crossfire/crossfire-arch/trunk/'
    result = filter_walk(root, 1, '*.arc', 1, '/dev/')
    assert("/dev/" not in result)
    assert("/Dev/" not in result)

# Can we din the humanoid monster wizard in trunk/monster/humanoid/Human/wizard.arc


def test_remove_dev_from_walk3():
    root = '/Users/tanner/projects/crossfire/crossfire-arch/trunk/'
    expected = 'monster/humanoid/Human/wizard.arc'
    results = filter_walk(root, 1, '*.arc', 1, '/dev/')

    for result in results:
        assert("/dev/" not in result)
        assert("/Dev/" not in result)

    # If not found throws StopIteration
    next(x for x in results if expected in x)


def test_python1():
    BASE_PATH = os.path.dirname(os.path.dirname((os.path.abspath(__file__))))

    file = BASE_PATH + '/tests/temp_summon_fog.arc'
    with open(file, 'r') as arc:
        contents = arc.read().splitlines()

    #import pdb
    # pdb.set_trace()
    parser = Arch2Json()
    items = parser.keys(contents)
    items.add_field('arch_filename', file)

    k, v = created_timestamp()
    items.add_field(k, v)

    k, v = updated_timestamp()
    items.add_field(k, v)

    assert(items.find('arch_filename', file))
    assert(items.find_key('created'))
    assert(items.find_key('updated'))

    field = Field()
    field.add('model', 'items.ground')

    for item in items.items:
        field.add_field(item)

    #print(json.dumps(field.field, indent=4))


def test_double_ends():
    BASE_PATH = os.path.dirname(os.path.dirname((os.path.abspath(__file__))))
    file = BASE_PATH + '/tests/altarvalk.arc'

    parser = Arch2Json()

    with open(file, 'r') as arc:
        contents = arc.read().splitlines()

    #import pdb
    # pdb.set_trace()
    items = parser.keys(contents)

    assert(items.find('object', 'altar_valkyrie'))
    assert(items.find('name', 'Altar of Valkyrie'))
    assert(items.find('object', 'altar_valkyrie_pray_event'))
    assert(items.find('title', 'Python'))
    assert(items.find('slaying', '/python/gods/altar_valkyrie.py'))
    #print(json.dumps(items.items, indent=4))


def test_no_name():
    BASE_PATH = os.path.dirname(os.path.dirname((os.path.abspath(__file__))))
    file = BASE_PATH + '/tests/dragon_guild.arc'

    parser = Arch2Json()

    with open(file, 'r') as arc:
        contents = arc.read().splitlines()

    items = parser.keys(contents)

    assert(items.find('object', 'Dragon Guild'))
    assert(items.find('name', 'Dragon Guild'))
