memjs = require 'memjs'
client = memjs.Client.create 'localhost:3000'

client.set 'hello', 'world', (err, val) ->
  console.log err, val
  setTimeout ->
    client.get 'hello', (err, val) ->
      console.log err, val
      setTimeout ->
        client.set 'hello', 'bye', (err, val) ->
          console.log err, val
      , 1000
  , 1000
