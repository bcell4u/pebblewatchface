/*
 * Minimal Plus phone-side settings transport.
 * Clay renders and persists the configuration page. We deliberately send its
 * settings in small, sequential AppMessage packets rather than relying on one
 * large dictionary, which is prone to failing on memory-constrained watches.
 */
var Clay = require('@rebble/clay');
var config = require('./config');
var clay = new Clay(config, null, { autoHandleEvents: false });

var BATCH_SIZE = 4;
var MAX_ATTEMPTS = 3;

function copySettingsIntoBatches(settings) {
  var keys = Object.keys(settings);
  var batches = [];
  var batch = {};
  var count = 0;

  for (var i = 0; i < keys.length; i++) {
    var key = keys[i];
    if (settings[key] === undefined || settings[key] === null) {
      continue;
    }
    batch[key] = settings[key];
    count += 1;
    if (count === BATCH_SIZE) {
      batches.push(batch);
      batch = {};
      count = 0;
    }
  }
  if (count > 0) {
    batches.push(batch);
  }
  return batches;
}

function sendBatches(batches, index, attempt) {
  if (index >= batches.length) {
    console.log('Minimal Plus: all ' + batches.length + ' settings packets delivered');
    return;
  }
  Pebble.sendAppMessage(batches[index], function() {
    sendBatches(batches, index + 1, 1);
  }, function(error) {
    console.log('Minimal Plus: settings packet ' + (index + 1) +
                ' failed on attempt ' + attempt + ': ' + JSON.stringify(error));
    if (attempt < MAX_ATTEMPTS) {
      // PebbleKit JS queues AppMessage calls. No artificial sleep is required;
      // retry only after the failure callback has been invoked.
      sendBatches(batches, index, attempt + 1);
    } else {
      console.log('Minimal Plus: unable to deliver the remaining settings');
    }
  });
}

Pebble.addEventListener('showConfiguration', function() {
  Pebble.openURL(clay.generateUrl());
});

Pebble.addEventListener('webviewclosed', function(event) {
  if (!event || !event.response || event.response === 'CANCELLED') {
    return;
  }

  try {
    // getSettings converts Clay toggles and colors to AppMessage-safe numbers.
    // It also retains Clay's local settings for the next configuration session.
    var settings = clay.getSettings(event.response);
    var batches = copySettingsIntoBatches(settings);
    if (batches.length > 0) {
      sendBatches(batches, 0, 1);
    }
  } catch (error) {
    console.log('Minimal Plus: invalid configuration response: ' + error);
  }
});

// Export only for local, dependency-free transport tests; Pebble ignores this.
module.exports = {
  copySettingsIntoBatches: copySettingsIntoBatches,
  sendBatches: sendBatches
};
