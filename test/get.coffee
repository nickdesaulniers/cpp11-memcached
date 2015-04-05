memjs = require 'memjs'
client = memjs.Client.create 'localhost:3000'

client.get 'hello', (err, val) ->
  console.log err, val
