#!/usr/bin/env python
#
# Copyright (C) 2019 Real Time Enterprises, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
from __future__ import print_function

import argparse
import copy
import datetime
import getopt
import html
import json
import operator
import os
import pprint
import re
import requests
import sys

from arch2xml import Walk
from collections import deque, OrderedDict

from functools import reduce

# https://pypi.org/project/Deprecated/ from deprecated import deprecated


def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)


def filter_walk(root, recurse=0, pattern='*', return_folders=0, exclude=None):
    files = Walk(root, recurse, pattern, return_folders)
    if filter:
        files = [arc for arc in files if exclude not in arc]

    return files


def face_png_list(face, graphics):
    new_list = list()
    path_file = os.path.split(face)
    _path = path_file[0]
    _file = path_file[1]

    if len(_file) <= 0:
        return new_list

    _file_parts = _file.split('.')

    regex = '.*'
    regex += _path
    regex += '/'
    regex += _file_parts[0]
    regex += '.*'
    regex += _file_parts[2]

    r = re.compile(regex)
    new_list = list(filter(r.match, graphics))
    return new_list


def arc_filename(filename, override_key=None):
    # Default to arc_filename for key
    k = 'arc_filename'

    # Allow over ride of the key
    if override_key:
        k = override_key

    v = os.path.basename(filename)

    return k, v


def arch_timestamp(key, format):
    k = key
    v = datetime.datetime.now().strftime(format)

    return k, v


def created_timestamp(override_key=None, format='%Y-%m-%dT%H:%M:%S.%fZ'):
    # Default to created for key
    k = 'created'

    if override_key:
        k = override_key

    k, v = arch_timestamp(k, format)

    return k, v


def exists(path):
    r = requests.head(path)
    return r.status_code == requests.codes.ok


def arc_to_face_png(face_filename, arc_relative):
    arc_filename = os.path.basename(arc_relative)
    face_relative = arc_relative.replace(arc_filename, face_filename)
    return face_relative + '.png'


def relative_path(full_path, trim_path):
    relative = full_path.replace(trim_path, '')
    return relative


def model_from_path(pathname, app, ext='.'):
    # This is nasty logic but works. Anyone have a better idea?
    #
    patterns = [
        'armour',
        'connect',
        'construct',
        'dev',
        'disease',
        'door',
        'exit',
        'flesh',
        'floor',
        'food',
        'gods',
        'ground',
        'indoor',
        'inorganic',
        'jewel',
        'light',
        'mapbuilding',
        'misc',
        'monster',
        'planes',
        'player',
        'potion',
        'random',
        'readable',
        'river',
        'road',
        'shop',
        'skills',
        'spell',
        'system',
        'talisman',
        'transport',
        'traps',
        'wall',
        'weapon',
    ]

    # import pdb
    # pdb.set_trace()

    order = dict()

    for pattern in patterns:
        index = pathname.find(pattern)
        match = None

        # Did not find pattern in pathname
        #
        if (index == -1):
            continue

        if (index != -1):

            length = len(pattern)
            word = pathname[index:index+length]
            word_after = pathname[index:index+length+1]

            # print(f'word_after {word_after}')

            if word_after.endswith(''):
                # print('word_after ends with blank')
                pass

            if word_after.endswith('/'):
                # print('word_after ends with /')
                match = app + ext + pattern
                order[index] = match

            if pattern == word_after:
                # print('match')
                match = app + ext + pattern
                order[index] = match

    # Handle pathname that have more then 1 pattern match, like /path/to/monster/misc/serpentman.arc
    # See test_auto_model5() for an example
    #
    one = OrderedDict(sorted(order.items(), key=lambda t: t[0]))
    if (one):
        k, v = one.popitem(False)
        return v

    return None


def updated_timestamp(override_key=None, format='%Y-%m-%dT%H:%M:%S.%fZ'):
    # Default to created for key
    k = 'updated'

    if override_key:
        k = override_key

    k, v = arch_timestamp(k, format)

    return k, v


class DjangoJsonDump():
    def __init__(self, model):
        self._list = list()
        #self.create_fields_and_model('fields', model)

    def add_field(self, key, value):
        self.add_kv_to_field(key=key, value=value)

    def add_kv_to_fields(self, key, value):
        index = self.find_key('fields')
        self._list[index]['fields'][key] = value

#    def create_field(self, field='field', index=0):
#            self._list.insert(index, {field: dict()})

    def create_fields_and_model(self, fields, model, index=0):
        self._list.insert(index, {
            fields: dict(),
            'model': model
        })

    def create_model(self, model, index=0):
        # Always add model to the begining of the list
        #
        if index == 0:
            self._list.insert(index, {
                'model': model
            })
        else:
            list_entry = self._list[index]
            list_entry['model'] = model

    def find_key(self, key):
        for i, dic in enumerate(self._list):
            if key in dic:
                return i

        raise ValueError(f'Did not find "{key}" in list')

    def update_model(self, model, index):
        self.create_model(model=model, index=index)

    @property
    def list(self):
        return self._list


class Animation():
    def __init__(self):
        self._anim = {}
        self.clear()

    def clear(self):
        self._anim.clear()

    @property
    def anim(self):
        return self._anim

    @anim.setter
    def anim(self, line):
        line = line.strip()

        if 'anim' in self._anim:
            self._anim['anim'].append(line)
        else:
            self._anim['anim'] = [line]


class Message():
    def __init__(self):
        self._message = {}
        self.clear()

    def clear(self):
        self._message.clear()

    @property
    def msg(self):
        return self._message

    @msg.setter
    def msg(self, line):
        line = line.strip()

        if 'msg' in self._message:
            self._message['msg'].append(line)
        else:
            self._message['msg'] = [line]


class Field():
    def __init__(self):
        self._fields = {'fields': None}

    def clear(self):
        self._fields.clear()

    def add(self, key, value):
        self._fields[key] = value

    def add_field(self, value):
        self._fields['fields'] = value

    def find_key(self, key):
        for dict_ in (x for x in self._fields if key in x):
            return dict_

    @property
    def field(self):
        return self._fields


class Items():
    def __init__(self):
        self._items = []
        self.clear()

    def clear(self):
        self._items.clear()

    def add(self, item):
        self._items.append(copy.deepcopy(item))

    # https://stackoverflow.com/questions/8653516/python-list-of-dictionaries-search
    #
    def find(self, key, value):
        return next((item for item in self._items if item.get(key) and item[key] == value), None)

    def find_key(self, key):
        for dict_ in (x for x in self._items if key in x):
            return dict_

    def find_value(self, value):
        for dict_ in (x for x in self._items if value in x.values()):
            return dict_

    def add_field(self, key, value):
        for dict_ in (x for x in self._items):
            dict_[key] = value

    @property
    def items(self):
        return self._items

    def item(self, index):
        return self._items[index]

    @property
    def len(self):
        return len(self._items)


class Arch2Json():
    def __init__(self):
        self.items = Items()

        self.clear()

    def clear(self):
        self.items.clear()

    def keys(self, contents):
        animation = Animation()
        message = Message()

        msg = False
        anim = False
        end = False
        arch = False

        item_dict = {}

        for line in contents:
            xp = line.split()

            # Throw out blank lines
            if (len(xp) <= 0):
                continue

            # Throw out comments
            if (xp[0].startswith('#')):
                continue

            if end:
                end = False
                item_dict.clear()

            key = xp[0].lower()

            if len(xp) == 1:

                if key == 'end':
                    if arch:
                        arch = False
                        continue

                    self.items.add(item_dict)
                    end = True

                elif key == 'more':
                    # eprint('Saw the more tag?')
                    continue

                elif key == 'msg':
                    msg = True
                    continue

                elif key == 'endmsg':
                    msg = False
                    continue

                elif key == 'anim':
                    anim = True
                    continue

                elif key == 'mina':
                    anim = False
                    continue

                elif (not msg) and (not anim):
                    key = '%s' % (key)
                    item_dict[key] = key

            if key == 'arch':
                arch = True

            if (len(xp) > 1) and (not msg) and (not anim):
                value = ' '.join(xp[1:])
                item_dict[key] = value

            # Handle msg
            if msg:
                message.msg = line
                item_dict.update(message.msg)

            # Handle anim
            if anim:
                animation.anim = line
                item_dict.update(animation.anim)

            # Side case processing
            # http://mailman.metalforge.org/pipermail/crossfire/2019-April/013601.html
            if not ('name' in item_dict):
                item_dict['name'] = item_dict['object']

        return self.items


def face_to_png(arc_dir, _list, trim_path):
    graphics = filter_walk(arc_dir, 1, '*.png', 1, '/dev/')
    django = DjangoJsonDump('items.facepng')

    graphics = [relative_path(x, trim_path) for x in graphics]

    for fields in _list:
        field = fields['fields']
        obj = field['object']

        try:
            face = field['face']
        except KeyError:
            face = ''

        png_list = face_png_list(face, graphics)
        for png in png_list:
            django.create_fields_and_model('fields', 'items.facepng')
            django.add_kv_to_fields('obj', obj)
            django.add_kv_to_fields('png', png)

    return django


def check_python():
    __min_version__ = (3, 0)

    if sys.version_info < (__min_version__):
        eprint('Error: cannot find a suitable python interpreter')
        eprint(' Need python %d.%d or later' % __min_version__)
        eprint(' Found python %d.%d' %
               (sys.version_info[0], sys.version_info[1]))
        eprint('  CRITICAL - Cannot find suitable python interpreter')
        raise SystemExit(1)


def parse_cli(argv, release):
    parser = argparse.ArgumentParser(description='Crossfire Arctype file to JSON converter',
                                     epilog='Send email to XXXXX if you have questions regarding this software.')

    group = parser.add_mutually_exclusive_group()
    group.add_argument('--onefile', dest='onefile', required=False,
                       help='Dump all archtypes to one JSON file.')
    group.add_argument('--splitfiles', dest='splitfiles', required=False, action='store_true',
                       help='Dump archtypes to individual JSON file. NOT IMPLEMENTED.')
    parser.add_argument('--version', action='version', version=release,
                        help='print version information')
    parser.add_argument('--django', dest='django', required=False,
                        help='Convert arc file to JSON for a "python manage.py loaddata --format json" import.')
    parser.add_argument('--indent', dest='indent', type=int, default=4, required=False,
                        help='Indentation on pretty print.')
    parser.add_argument('--trimpath', dest='trim_path', required=False,
                        help='Trim this path to create a relative path to the .arc files.')
    parser.add_argument('--facepng', dest='face_png', required=False, action='store_true',
                        help='Generate JSON for faces PNG database table.')
    parser.add_argument('--monolithic', dest='monolithic', required=False,
                        help='Testing the monolithic model.')
    parser.add_argument('arc_dir',
                        help='The crossfire archtype directory.')

    args = parser.parse_args()
    return args


def main(args):
    parser = Arch2Json()
    fields = Field()

    _list = []

    # Should be the local path to the trunk of the Arch files
    #
    arc_dir = args.arc_dir

    # If the trim_path is not set default to the arc_dir
    #
    trim_path = args.trim_path
    if not trim_path:
        trim_path = arc_dir

    files = filter_walk(arc_dir, 1, '*.arc', 1, '/dev/')

    for file in files:
        parser.clear()

        with open(file, 'r') as arc:
            contents = arc.read().splitlines()

        items = parser.keys(contents)

        relative = relative_path(file, trim_path)
        items.add_field('arc_filename', relative)

        old_face = items.find_key('face')
        if old_face:
            face = arc_to_face_png(old_face['face'], relative)
            items.add_field('face', face)
            #faces = populate_face_png(items, graphics)

        # Add created timestamp to item
        k, v = created_timestamp()
        items.add_field(k, v)

        # Add updated timestamp to item
        k, v = updated_timestamp()
        items.add_field(k, v)

        # Add django model
        if (args.django):
            if (args.monolithic):
                monolithic = args.django + '.' + args.monolithic
                fields.add('model', monolithic)
            else:
                model = model_from_path(file, args.django)
                fields.add('model', model)

        for item in items.items:
            fields.add_field(item)
            _list.append(copy.deepcopy(fields.field))

    if (args.face_png):
        django = face_to_png(arc_dir, _list, trim_path)
        _list += django.list

    # Dump archtypes to one files identified by --onefile
    #
    if (args.onefile):
        with open(args.onefile, 'w') as one_file:
            one_file.write(json.dumps(_list, indent=args.indent))
    else:
        # Default to dumping archtypes to stdout
        #
        print(json.dumps(_list, indent=args.indent))


if __name__ == '__main__':
    try:
        check_python()
        args = parse_cli(sys.argv, '0.1')
        main(args)
    except Exception as e:
        print(f'Error: {e}')
        raise SystemExit(1)
