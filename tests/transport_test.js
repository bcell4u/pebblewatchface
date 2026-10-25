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
assert.strictEqual((new Set(fields)).size, 26);
var gapField = config.flatMap(function(section) { return section.items || []; }).find(function(item) { return item.messageKey === "ClockSpacing"; });
assert(gapField && gapField.type === "slider" && gapField.min === 1 && gapField.max === 10 && gapField.defaultValue === 2);
var flat = config.flatMap(function(section) { return section.items || []; });
for (var name of ['WeekdaySize','DateSize']) {
  var slider = flat.find(function(item) { return item.messageKey === name; });
  assert(slider && slider.type==='slider' && slider.min===10 && slider.max===40);
}
for (var name of ['WeekdayBold','DateBold']) {
  var toggle = flat.find(function(item) { return item.messageKey === name; });
  assert(toggle && toggle.type==='toggle' && toggle.defaultValue===false);
}
var offsetField = flat.find(function(item) { return item.messageKey === 'AccentYOffset'; });
assert(offsetField && offsetField.type === 'slider' && offsetField.min === -12 && offsetField.max === 12 && offsetField.defaultValue === -4 && offsetField.step === 1);
assert(offsetField.description.includes("Positive values move the line UP"));
assert(keys.slice(-4).join(',')==='WeekdayBold,DateBold,AccentYOffset,WeekdayColor');
var screenshotDefaults = {"ClockLayout":"0","ClockSize":100,"ClockSpacing":2,"ShowAmPm":true,"MirrorLayout":false,"AccentWidth":60,"AccentYOffset":-4,"WeekdaySize":21,"DateSize":27,"WeekdayBold":false,"DateBold":false,"BatteryEnabled":true,"BatteryPosition":"0","BatteryIconVisible":true,"BatteryIconSize":85,"BatteryPercentVisible":false,"BatteryTextSize":30,"BackgroundColor":"0x0055AA","HoursColor":"0xFFFFFF","MinutesColor":"0xFFFFFF","AmPmColor":"0xAAAAAA","AccentColor":"0xAAFFAA","DateColor":"0xFFFFFF","WeekdayColor":"0xFFFFFF","BatteryIconColor":"0xAAFFAA","BatteryTextColor":"0xFFFFFF"};
for (var [key, expected] of Object.entries(screenshotDefaults)) {
  var setting = flat.find(x => x.messageKey === key);
  assert(setting && setting.defaultValue === expected,
    `Mismatch for ${key}: ${setting && setting.defaultValue} != ${expected}`);
}
var clock = flat.find(x => x.messageKey==='ClockSize');
assert(clock && clock.min===50 && clock.max===120);
var dayCol = flat.find(x => x.messageKey==='WeekdayColor');
var dateCol = flat.find(x => x.messageKey==='DateColor');
assert(dayCol && dateCol && dayCol.type==='color' && dateCol.type==='color');
assert(config.some(function(item) { return item.type === 'submit'; }), 'Clay save button required');
console.log('PASS: all 26 settings registered with matching AppMessage keys; Clay automatic events enabled');
