from arch2xml import Walk
import re
import sys

sys.path.append("..")


mylist = Walk(
    '/Users/tanner/projects/crossfire/crossfire-arch/trunk', 1, '*.png', 1)

r = re.compile(".*a_helmet.*\.png")
newlist = list(filter(r.match, mylist))  # Read Note

for item in newlist:
    print(f'{item}')
