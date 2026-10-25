# Minimal Plus 1.0.25 — Screenshot defaults

These defaults were introduced in v1.0.23 and are retained in v1.0.25. No new AppMessage keys, fields, or fonts have been added. Date layout is now cached between relevant changes. All 26 controls, the working settings parser, and automatic PBW publishing to GitHub Releases remain unchanged.

## Default settings

| Control | Default |
|---|---|
| Clock layout | Stacked |
| Clock size | 100% |
| Hours/minutes gap | 2 px |
| AM/PM | On |
| Mirror | Off |
| Accent width | 60 px |
| Accent vertical offset | -4 px (downward) |
| Weekday size | 21 |
| Date size | 27 |
| Weekday bold / Date bold | Both off |
| Battery | Enabled, top-right |
| Battery icon | Shown, size 85% |
| Battery percentage | Hidden, text-size setting 30% |
| Background | Dark blue (`0x0055AA`) |
| Hours/minutes | White (`0xFFFFFF`) |
| AM/PM | Gray (`0xAAAAAA`) |
| Accent and battery icon | Pale mint (`0xAAFFAA`) |
| Weekday/date and hidden battery percentage | White (`0xFFFFFF`) |

Colors are matched to the screenshot using the nearest Pebble Time 64-color palette entries; screenshots do not reveal the exact saved hex values.

## Installation

**CloudPebble:** Replace `src/c/main.c` and `src/pkjs/config.js`, build for Basalt, and install. There are no new message keys.

**GitHub:** Replace the full project and push to main. The GitHub Actions workflow still publishes successful builds to Releases.

**Existing settings:** The watchface deliberately honors existing persistent settings, and Clay may retain saved phone-side values. The new defaults apply on a fresh install / clean settings. If you already use the screenshot values, nothing else is necessary. Updating the source does not forcibly reset existing users' choices.

**Testing:** Run `node tests/transport_test.js`, `python3 tests/defaults_test.py`, and the watch-side test and GCC/Clang compatibility checks. A real SDK build/device test is still recommended.
