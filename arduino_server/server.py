# -*- coding: utf-8 -*-
__author__ = 'wangqi'

import sys

reload(sys)
sys.setdefaultencoding('utf8')

# from __future__ import print_function
from gevent.server import StreamServer
import MySQLdb

conn = MySQLdb.connect(
    host='localhost',
    port=3306,
    user='root',
    passwd='root',
    db='arduino_controller',
    charset='utf8',
)
cur = conn.cursor()

def getCommand(token):
    sql = 'select action_list.id, action_list.action_mode, action_list.speed, action_list.direction, action_list.frequency from action_list join token where action_list.token_id = token.id and token.token = "%s" order by create_time' % token
    print sql
    cur.execute(sql)
    action_list = cur.fetchall()
    print action_list
    if len(action_list) == 0:
        return 0, 0, 0, 0, 0
    else:
        return action_list[0][0], action_list[0][1], action_list[0][2], action_list[0][3], action_list[0][4]

def delCommand(command_id):
    sql = 'delete from action_list where id = %d' % command_id
    print sql
    cur.execute(sql)
    conn.commit()


# this handler will be run for each incoming connection in a dedicated greenlet
def echo(socket, address):
    print 'New connection from %s:%s' % address
    # socket.sendall(b'Welcome to the echo server! Type quit to exit.\r\n')
    # using a makefile because we want to use readline()
    rfileobj = socket.makefile(mode='rb')
    while True:
        line = rfileobj.readline()
        # print(line)
        if not line:
            print "client disconnected"
            break
        if line.strip().lower() == b'quit':
            print "client quit"
            break
        token = line.split('=')[1].strip()
        print token
        command_id, action_mode, speed, direction, frequency = getCommand(token)
        print "====", command_id, action_mode, speed, direction, frequency
        line = "*0#"
        if action_mode == 1:
            line = "*%d|%d|%d#" % (action_mode, speed, direction)
        elif action_mode == 2:
            line = "*%d|%d#" % (action_mode, frequency) 

        print 'line', line
        socket.sendall(line)
        delCommand(command_id)
        # print("echoed %r" % line)
    rfileobj.close()

if __name__ == '__main__':
    # to make the server use SSL, pass certfile and keyfile arguments to the constructor
    server = StreamServer(('0.0.0.0', 5000), echo)
    # to start the server asynchronously, use its start() method;
    # we use blocking serve_forever() here because we have no other jobs
    print 'Starting echo server on port 5000'
    server.serve_forever()
