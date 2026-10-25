/* Minimal Plus 1.0.25: Screenshot-matched first-install defaults. */
module.exports = [
  {
    "type": "heading",
    "defaultValue": "Minimal Plus"
  },
  {
    "type": "text",
    "defaultValue": "A quiet, highly customizable watchface for the original Pebble Time."
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Clock Layout & Typography"
      },
      {
        "type": "select",
        "messageKey": "ClockLayout",
        "label": "Clock layout",
        "defaultValue": "0",
        "options": [
          {
            "label": "Stacked: hours over minutes",
            "value": "0"
          },
          {
            "label": "Single line: 10:32",
            "value": "1"
          }
        ]
      },
      {
        "type": "slider",
        "messageKey": "ClockSize",
        "label": "Main clock size (%)",
        "defaultValue": 100,
        "min": 50,
        "max": 120,
        "step": 1,
        "description": "Up to 120%. Large stacked digits use spare space above the date; constrained when date is oversized."
      },
      {
        "type": "slider",
        "messageKey": "ClockSpacing",
        "label": "Hours / minutes spacing (px)",
        "defaultValue": 2,
        "min": 1,
        "max": 10,
        "step": 1,
        "description": "Gap between the stacked hours and minutes (only affects two-line layout)."
      },
      {
        "type": "toggle",
        "messageKey": "ShowAmPm",
        "label": "Show AM / PM",
        "defaultValue": true
      },
      {
        "type": "toggle",
        "messageKey": "MirrorLayout",
        "label": "Mirror layout (clock right, battery left)",
        "defaultValue": false,
        "description": "Right-justify clock, accent and date; move battery to the left. Text stays readable."
      }
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Accent and Date"
      },
      {
        "type": "slider",
        "messageKey": "AccentWidth",
        "label": "Accent line width (pixels)",
        "defaultValue": 60,
        "min": 8,
        "max": 120,
        "step": 1
      },
      {
        "type": "slider",
        "messageKey": "AccentYOffset",
        "label": "Accent vertical position (pixels)",
        "defaultValue": -4,
        "min": -12,
        "max": 12,
        "step": 1,
        "description": "Positive values move the line UP; negative values move it DOWN. 0 uses automatic placement. Large offsets may overlap text."
      },
      {
        "type": "slider",
        "messageKey": "WeekdaySize",
        "label": "Weekday text size (MON)",
        "defaultValue": 21,
        "min": 10,
        "max": 40,
        "step": 1,
        "description": "Actual 10–40 sizing: native Gothic up to 28, smooth larger characters above 28. Battery at top or hidden frees more room."
      },
      {
        "type": "slider",
        "messageKey": "DateSize",
        "label": "Date text size (APR 28)",
        "defaultValue": 27,
        "min": 10,
        "max": 40,
        "step": 1,
        "description": "Actual 10–40 sizing with anti-aliased large letters; effective size may decrease to fit if the battery is beside the date."
      },
      {
        "type": "toggle",
        "messageKey": "WeekdayBold",
        "label": "Bold weekday (MON)",
        "defaultValue": false
      },
      {
        "type": "toggle",
        "messageKey": "DateBold",
        "label": "Bold date (APR 28)",
        "defaultValue": false
      }
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Battery"
      },
      {
        "type": "toggle",
        "messageKey": "BatteryEnabled",
        "label": "Enable battery indicator",
        "defaultValue": true
      },
      {
        "type": "select",
        "messageKey": "BatteryPosition",
        "label": "Battery position",
        "defaultValue": "0",
        "description": "Top leaves the full bottom row available for larger weekday/date text. Bottom reserves room beside the date. Right normally; left when mirrored.",
        "options": [
          {
            "label": "Top corner",
            "value": "0"
          },
          {
            "label": "Bottom corner",
            "value": "1"
          }
        ]
      },
      {
        "type": "toggle",
        "messageKey": "BatteryIconVisible",
        "label": "Show battery icon",
        "defaultValue": true
      },
      {
        "type": "slider",
        "messageKey": "BatteryIconSize",
        "label": "Battery icon size (%)",
        "defaultValue": 85,
        "min": 30,
        "max": 100,
        "step": 5
      },
      {
        "type": "toggle",
        "messageKey": "BatteryPercentVisible",
        "label": "Show battery percentage",
        "defaultValue": false
      },
      {
        "type": "slider",
        "messageKey": "BatteryTextSize",
        "label": "Battery percentage size (%)",
        "defaultValue": 30,
        "min": 30,
        "max": 100,
        "step": 5
      }
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Individual Element Colors"
      },
      {
        "type": "color",
        "messageKey": "BackgroundColor",
        "label": "Background",
        "defaultValue": "0x0055AA"
      },
      {
        "type": "color",
        "messageKey": "HoursColor",
        "label": "Hours",
        "defaultValue": "0xFFFFFF"
      },
      {
        "type": "color",
        "messageKey": "MinutesColor",
        "label": "Minutes",
        "defaultValue": "0xFFFFFF"
      },
      {
        "type": "color",
        "messageKey": "AmPmColor",
        "label": "AM / PM",
        "defaultValue": "0xAAAAAA"
      },
      {
        "type": "color",
        "messageKey": "AccentColor",
        "label": "Accent line",
        "defaultValue": "0xAAFFAA"
      },
      {
        "type": "color",
        "messageKey": "DateColor",
        "label": "Date (month and day)",
        "defaultValue": "0xFFFFFF"
      },
      {
        "type": "color",
        "messageKey": "WeekdayColor",
        "label": "Weekday",
        "defaultValue": "0xFFFFFF"
      },
      {
        "type": "color",
        "messageKey": "BatteryIconColor",
        "label": "Battery icon",
        "defaultValue": "0xAAFFAA"
      },
      {
        "type": "color",
        "messageKey": "BatteryTextColor",
        "label": "Battery percentage",
        "defaultValue": "0xFFFFFF"
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Watchface Settings"
  }
];
