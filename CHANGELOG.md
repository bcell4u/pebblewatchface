## 1.0.27

- Set first-install background color to dark Oxford Blue (`#000055`) in C and Clay, retaining existing saved colors.
- Fix migration condition so v6 weekday colors are never overwritten by the date color.

- Add independent CenterAccent and CenterDateRow toggles in Clay and AppMessage parser.
- Center the accent line on screen, or the combined weekday/date row, independent of mirror layout.
- Preserve old alignment with both toggles initially off.
- Append saved settings fields with version 7 and migrate v1–v6 records safely, keeping v6 weekday colors.
- Expand schema and regression checks to 28 options; retain date-layout caching and no-diagnostics release.

## 1.0.26

- Make battery icon and percentage visible at the top-right on fresh installations.
- Draw compact 7px battery percentage at 30–44% slider settings, with 2px visible gap under icon.
- Keep mirror, position, visibility, and color options independent and persistent.
- Preserve the 26 message keys and binary settings format.
- Add tiny-percent geometry tests and validate top-right/left-mirrored bounds.

## 1.0.25

- Cache unchanged weekday/date layout instead of measuring text each minute.
- Reduce redundant fill-color graphics calls when drawing smooth glyphs.
- Narrow safe glyph offsets to 16-bit fields, shrinking font metadata.
- Preserve 26 settings, persistent version 6, screenshot defaults and appearance.
- Remove obsolete store preview assets, fix package version/author metadata, and
  tighten compiler checks for unreferenced static functions.
- No diagnostic tracing; automatic PBW-only GitHub Releases unchanged.

## 1.0.24

- Remove all watch-side tracing and diagnostic log statements from the application code.
- Remove obsolete debug callback and the debug-logging configuration switch.
- Remove debug ELF files from GitHub release assets; publish only the PBW.
- Keep all existing settings, saved settings format, and 1.0.23 default values.

## 1.0.23

- Matched all 26 first-run defaults to the supplied screenshot.
- Selected closest Pebble Time palette colors for screenshot background and mint accents.
- No changes to drawing, message keys, persisted-settings schema, or release workflow.
- Added screenshot-default consistency tests.
