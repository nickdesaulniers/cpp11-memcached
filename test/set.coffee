memjs = require 'memjs'
client = memjs.Client.create 'localhost:3000'

client.set 'hello', 'world', (err, val) ->
  console.log err, val
