import json

item_list = []

item_dict = {}
item_dict['a'] = 'a'
item_dict['c'] = 'b'
item_dict['c'] = 'c'
item_dict['b'] = 'b'
item_dict['d'] = 1
item_dict['e'] = 2
item_dict['f'] = 3

item_list.append(item_dict)
print('1')
print(json.dumps(item_list))

item_dict['g'] = 'a'
item_dict['h'] = 'b'
item_dict['i'] = 'c'
item_dict['j'] = 'b'
item_dict['k'] = 1
item_dict['l'] = 2
item_dict['m'] = 3

item_list.append(item_dict)
print('2')
print(json.dumps(item_list))

item_list = []
print('3')
print(json.dumps(item_list))
