# pip install python-binary-memcached
import logging
import bmemcached

#logging.basicConfig()
#logging.getLogger('bmemcached').setLevel(logging.DEBUG)

client = bmemcached.Client('127.0.0.1:3000')

client.set('hello', 'world')
print client.get('hello')

client.set('hello', 1)
print client.get('hello')

client.set('hello', ['world1', 'world2'])
a = client.get('hello')
print a
print len(a)
print isinstance(a, list)
print isinstance(a, basestring)

