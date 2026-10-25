'use strict';
// Validates the actual configuration schema and the use of Clay's documented
// event-handling mode. Pebble/Clay run-time behavior still needs device tests.
var assert = require('assert');
var Module = require('module');
var load = Module._load;
var created = 0;
var configured = null;
function FakeClay(config, custom, options) {
  created++;
  configured = config;
  assert.strictEqual(custom, undefined);
  assert.strictEqual(options, undefined, 'Clay must use automatic event handling');
}
Module._load = function(request, parent, isMain) {
  if (request === '@rebble/clay') return FakeClay;
  return load.call(this, request, parent, isMain);
};

global.Pebble = { addEventListener: function() {
  throw Error('No manual Pebble handlers should compete with Clay');
}};
var clay = require('../src/pkjs/index.js');
assert(clay instanceof FakeClay);
assert.strictEqual(created, 1);

var config = require('../src/pkjs/config.js');
var fields = [];
function walk(items) {
  items.forEach(function(item) {
    if (item.messageKey) {
      assert(['select','slider','toggle','color'].indexOf(item.type) !== -1, item.messageKey);
      fields.push(item.messageKey);
      if (item.type === 'slider') {
        assert(item.min <= item.defaultValue && item.defaultValue <= item.max);
      }
    }
    if (item.items) walk(item.items);
  });
}
walk(config);
var keys = require('../package.json').pebble.messageKeys;
assert.deepStrictEqual(fields.slice().sort(), keys.slice().sort());
assert.strictEqual((new Set(fields)).size, 21);
assert(config.some(function(item) { return item.type === 'submit'; }), 'Clay save button required');
console.log('PASS: all 21 settings registered with matching AppMessage keys; Clay automatic events enabled');
