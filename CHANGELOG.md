# Changelog

## 1.0.10 — Accent Width / Independent Date Type / Mirrored Layout

- Added Accent Width slider (8–120 px), left anchored normally and right anchored in mirror mode.
- Added independent Weekday Size and Date Size sliders (10–24 px). Added small antialiased Lato glyph masks so every slider step changes the actual pixel height, not merely switching between three system fonts.
- Added Mirror Layout toggle: clock, accent and date right-align; battery and its percentage left-align. AM/PM moves to the opposite side of stacked minutes. Single-line clock also right-aligns in mirror mode.
- Added safe horizontal fitting for large weekday/date combinations with bottom battery.
- Upgraded persistent settings from v1 to v2 with backward-compatible migration, leaving all 17 original saved controls intact.
- Added regression tests for mirrored battery placement, accent widths, date-size independence, long weekday/month strings, single-line mirroring and v1 migration.
- Preserved v1.0.6's working defensive AppMessage parser and v1.0.9's antialiased clock numerals.
- Preserved GitHub Actions auto-publication of successful `.pbw` and matching `.elf` artifacts to GitHub Releases.

**Testing note:** Local compilation and emulator mock tests passed. A real Pebble SDK build and physical-watch visual verification remain outstanding.
