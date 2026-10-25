/*
 * Clay owns showConfiguration/webviewclosed and sends the settings to the watch.
 * IMPORTANT: don't register a second webviewclosed handler. The previous custom
 * packetization path wasn't end-to-end tested, and left devices unchanged.
 */
var Clay = require('@rebble/clay');
var config = require('./config');
var clay = new Clay(config);

// Keep the module alive; Clay registers both event handlers on construction.
module.exports = clay;
