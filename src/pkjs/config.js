/* Pebble Time 64-color configuration. All fields are saved by Clay. */
module.exports = [
  {"type": "heading", "defaultValue": "Minimal Plus"},
  {"type": "text", "defaultValue": "A quiet, highly customizable watchface for the original Pebble Time."},
  {"type": "section", "items": [
    {"type": "heading", "defaultValue": "Clock Layout & Typography"},
    {"type": "select", "messageKey": "ClockLayout", "label": "Clock layout",
     "defaultValue": "0", "options": [
       {"label": "Stacked: hours over minutes", "value": "0"},
       {"label": "Single line: 10:32", "value": "1"}
     ]},
    {"type": "slider", "messageKey": "ClockSize", "label": "Main clock size (%)",
     "defaultValue": 72, "min": 50, "max": 100, "step": 1,
     "description": "Continuously scales the numeric clock, with a tight gap between stacked hours and minutes."},
    {"type": "toggle", "messageKey": "ShowAmPm", "label": "Show AM / PM",
     "defaultValue": true},
    {"type": "toggle", "messageKey": "MirrorLayout", "label": "Mirror layout (clock right, battery left)",
     "defaultValue": false, "description": "Right-justify clock, accent and date; move battery to the left. Text stays readable."}
  ]},
  {"type": "section", "items": [
    {"type": "heading", "defaultValue": "Accent and Date"},
    {"type": "slider", "messageKey": "AccentWidth", "label": "Accent line width (pixels)",
     "defaultValue": 29, "min": 8, "max": 120, "step": 1},
    {"type": "slider", "messageKey": "WeekdaySize", "label": "Weekday text size (MON)",
     "defaultValue": 14, "min": 10, "max": 24, "step": 1,
     "description": "Independently change the weekday height in pixels."},
    {"type": "slider", "messageKey": "DateSize", "label": "Date text size (APR 28)",
     "defaultValue": 14, "min": 10, "max": 24, "step": 1,
     "description": "Independently change the month and day-of-month height in pixels. Text may fit horizontally when battery occupies the bottom corner."}
  ]},
  {"type": "section", "items": [
    {"type": "heading", "defaultValue": "Battery"},
    {"type": "toggle", "messageKey": "BatteryEnabled", "label": "Enable battery indicator",
     "defaultValue": true},
    {"type": "select", "messageKey": "BatteryPosition", "label": "Battery position",
     "defaultValue": "1", "description": "Right side normally; left side when Mirror Layout is enabled.", "options": [
       {"label": "Top corner", "value": "0"},
       {"label": "Bottom corner", "value": "1"}
     ]},
    {"type": "toggle", "messageKey": "BatteryIconVisible", "label": "Show battery icon",
     "defaultValue": true},
    {"type": "slider", "messageKey": "BatteryIconSize", "label": "Battery icon size (%)",
     "defaultValue": 40, "min": 30, "max": 100, "step": 5},
    {"type": "toggle", "messageKey": "BatteryPercentVisible", "label": "Show battery percentage",
     "defaultValue": true},
    {"type": "slider", "messageKey": "BatteryTextSize", "label": "Battery percentage size (%)",
     "defaultValue": 50, "min": 30, "max": 100, "step": 5}
  ]},
  {"type": "section", "items": [
    {"type": "heading", "defaultValue": "Individual Element Colors"},
    {"type": "color", "messageKey": "BackgroundColor", "label": "Background", "defaultValue": "0x000000"},
    {"type": "color", "messageKey": "HoursColor", "label": "Hours", "defaultValue": "0xFFFFFF"},
    {"type": "color", "messageKey": "MinutesColor", "label": "Minutes", "defaultValue": "0xFFFFFF"},
    {"type": "color", "messageKey": "AmPmColor", "label": "AM / PM", "defaultValue": "0xAAAAAA"},
    {"type": "color", "messageKey": "AccentColor", "label": "Accent line", "defaultValue": "0x55FFAA"},
    {"type": "color", "messageKey": "DateColor", "label": "Weekday + date", "defaultValue": "0xFFFFFF"},
    {"type": "color", "messageKey": "BatteryIconColor", "label": "Battery icon", "defaultValue": "0x55FFAA"},
    {"type": "color", "messageKey": "BatteryTextColor", "label": "Battery percentage", "defaultValue": "0xFFFFFF"}
  ]},
  {"type": "submit", "defaultValue": "Save Watchface Settings"}
];
