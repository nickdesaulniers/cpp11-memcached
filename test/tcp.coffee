net = require 'net'
buffer = require 'buffer'

set = new Buffer [
  0x80, 0x01, 0x00, 0x05,
  0x08, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x12,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0xde, 0xad, 0xbe, 0xef,
  0x00, 0x00, 0x0e, 0x10,
  0x48, 0x65, 0x6c, 0x6c,
  0x6f, 0x57, 0x6f, 0x72,
  0x6c, 0x6d
]

get = new Buffer [
  0x80, 0x00, 0x00, 0x05,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x05,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x48, 0x65, 0x6c, 0x6c,
  0x6f
]

s = net.connect 3000, ->
  console.log 'connected'
  s.write set
  setTimeout ->
    s.write get
  , 1000
s.on 'data', (d) ->
  console.log 'got data', d
s.on 'end', ->
  console.log 'end'
