memjs = require 'memjs'
client = memjs.Client.create 'localhost:3000'

vals = [
  1e1,
  2e2,
  3e3,
  4e4,
  5e5,
  6e6
]

rand = ->
  Math.random() * vals.length | 0

k = 0
for i in [0..1e1]
  if i % 3 is 0
    k = vals[rand()].toString()
    p = k
    console.log "setting k to #{k}"
    client.set 'hello', k, (err, val) ->
      console.log err, val
  else
    client.get 'hello', (err, val) ->
      console.log err, val
