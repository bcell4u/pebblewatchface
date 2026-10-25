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
