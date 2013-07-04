#!/usr/bin/env python

import os

# domains
# form:
# A = ("A", "net/mask")
# snippet: domain

#
# domains = [A,B,C]

#

# hws
# form:
# name = ("name", {...})
# snippet: hw

#
# hws = [pc1, r1, r2, pc2]

#




# Here the functions for generating the lab.conf
# the hw directories, startup files and abolish abbr

domain2hw = {}

for hw in hws:
    for key in hw[1]:
        collision_name = hw[1][key][0][0]
        net = hw[1][key][0][1]
        ip = net[:net.rfind('.')+1] + hw[1][key][1]
        if not domain2hw.has_key(collision_name):
            domain2hw[collision_name] = []
        domain2hw[collision_name].append((hw[0], ip))


lab = open('lab.conf', 'w')
abolish = open('abolish_abbr.vim', 'w')

# write lab.conf
for hw in hws:
    for key in hw[1]:
        collision_name = hw[1][key][0][0]
        lab.write(hw[0] + '[' + str(key) + ']=' + collision_name + '\n')
    lab.write('\n')

lab.close()

# write hw.startup
for hw in hws:
    os.mkdir(hw[0])
    startup = open(hw[0]+'.startup', 'w')
    for key in hw[1]:
        net = hw[1][key][0][1]
        ip = hw[1][key][1]
        ip = net[:net.rfind('.')+1] + ip
        shortnet = net[net.rfind('/'):]

        startup.write('ifconfig eth' + str(key) + ' ' + ip + shortnet + ' up\n')

    startup.close()

for hw in hws:
    for key in hw[1]:
        collision_name = hw[1][key][0][0]
        net = hw[1][key][0][1]
        ip = hw[1][key][1]
        ip = net[:net.rfind('.')+1] + ip
        shortnet = net[net.rfind('/'):]

        # print 'Abolish ' + hw[0] + 'ip' + collision_name + ' ' + ip
        abolish.write('Abolish ' + hw[0] + 'ip' + collision_name + ' ' + ip + '\n')

for domain in domains:
    abolish.write('Abolish domain' + domain[0] + ' ' + domain[1]+'\n')

abolish.close()
