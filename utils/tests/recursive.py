import os
import pprint
import copy


def keys(contents, item_list):
    end = False
    item_dict = {}
    pp = pprint.PrettyPrinter(indent=4)

    for line in contents:
        xp = line.split()

        # Throw out blank lines
        if (len(xp) <= 0):
            continue

        key = xp[0].lower()

        if end:
            end = False

        if key == 'end':
            item_list.append(copy.deepcopy(item_dict))
            end = True
        else:
            value = ' '.join(xp[1:])
            item_dict[key] = value

    return item_list


pp = pprint.PrettyPrinter(indent=4)
with open('emptybottles.arc', 'r') as arc:
    contents = arc.read().splitlines()

    item_list = []

    items = keys(contents, item_list)

    pp.pprint(items)
