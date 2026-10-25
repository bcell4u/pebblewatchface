'use strict';
var assert = require('assert');
var path = require('path');
var Module = require('module');
var originalLoad = Module._load;
var handlers = {};
var opened = null;
var deliveries = [];
var failuresBeforeSuccess = 1;
var claySettings = {};

function MockClay(config, custom, options) {
  assert(config.length > 0);
  assert.strictEqual(custom, null);
  assert.strictEqual(options.autoHandleEvents, false);
  this.generateUrl = function() { return 'data:text/html;base64,dGVzdA=='; };
  this.getSettings = function(response) {
    assert.strictEqual(response, 'SETTINGS_RESPONSE');
    return claySettings;
  };
}
Module._load = function(request, parent, isMain) {
  if (request === '@rebble/clay') return MockClay;
  return originalLoad.call(this, request, parent, isMain);
};

global.Pebble = {
  addEventListener: function(event, cb) { handlers[event] = cb; },
  openURL: function(u) { opened = u; },
  sendAppMessage: function(dict, success, failure) {
    if (failuresBeforeSuccess > 0) {
      failuresBeforeSuccess -= 1;
      failure('simulated temporary failure');
      return;
    }
    deliveries.push(dict);
    success();
  }
};
var target = require('../src/pkjs/index.js');
assert.equal(typeof handlers.showConfiguration, 'function');
assert.equal(typeof handlers.webviewclosed, 'function');
handlers.showConfiguration();
assert(opened.indexOf('data:text/html') === 0);

var messageKeys = require('../package.json').pebble.messageKeys;
messageKeys.forEach(function(key, i) { claySettings[key] = i; });
var batch = target.copySettingsIntoBatches(claySettings);
assert.equal(batch.length, 5);
batch.forEach(function(p) { assert(Object.keys(p).length <= 4); });
handlers.webviewclosed({response:'CANCELLED'});
assert.equal(deliveries.length, 0);
handlers.webviewclosed({response:'SETTINGS_RESPONSE'});
assert.equal(deliveries.length, 5);
var merged = {};
deliveries.forEach(function(d){ Object.assign(merged, d); });
assert.deepStrictEqual(merged,claySettings);
assert.equal(failuresBeforeSuccess,0);
console.log('PASS: Clay opens, all 17 settings delivered in 5 sequential packets, retries failures, handles cancel');
