fs = require 'fs'
memjs = require 'memjs'

file = fs.readFileSync 'send_file.coffee'
client = memjs.Client.create 'localhost:3000'

console.log file.length
client.set 'hello', file, (err, val) ->
  console.log err, val
  setTimeout ->
    client.get 'hello', (err, val) ->
      console.log val.length
  , 5000
