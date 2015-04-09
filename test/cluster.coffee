cluster = require 'cluster'
assert = require 'assert'
memjs = require 'memjs'
numCpus = require('os').cpus().length

client = memjs.Client.create 'localhost:3000'
id = if cluster.isMaster then null else cluster.worker.id

possibleKeys = [
  'world',
  'nick',
  'Ozymandias'
]

getRandomKey = ->
  index = Math.random() * possibleKeys.length | 0
  possibleKeys[index]

getOrSet = (cb) ->
  if Math.random() * 2 | 0
    client.get 'hello', cb
  else
    val = getRandomKey()
    client.set 'hello', val, cb
    console.log "worker #{id} set 'hello' to '#{val}'"

attackServer = ->
  getOrSet (err, val) ->
    console.log err, val.toString()
    assert possibleKeys.indexOf val.toString() > -1
    setTimeout attackServer, Math.random() * 100

if cluster.isMaster
  client.set 'hello', 'world', (err, val) ->
    cluster.fork() for i in [0...numCpus]
    cluster.on 'exit', (worker, code, signal) ->
      console.log "worker #{worker.process.pid} died"
else
  attackServer()

