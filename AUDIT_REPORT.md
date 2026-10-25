# Minimal Plus v1.0.25 — Source and Package Audit

## Audited components

- `src/c/main.c`: settings load/migration, safe message parsing, clock/date
  rendering, minute and battery callbacks, shutdown.
- `src/pkjs/config.js` and `index.js`: exactly 26 Clay controls and automatic
  events (no competing manual message sender).
- `package.json`: Basalt-only build, matching stable UUID, correct 1.0.25
  version and compatible appended message-key order.
- `.github/workflows/build.yml`: regression checks, normal SDK build, PBW-only
  release assets, no diagnostics or ELF publication.
- License and font attribution files retained.

## What changed

1. **Date layout cache:** Existing weekday/date fitting relied on costly
   native font-size measurement and up to 90 fitting iterations at *every*
   screen redraw. A small cache stores the result and compares only relevant
   settings, calendar text, and display dimensions. Any date/position/size
   change automatically invalidates it, including direct watch-side changes.
2. **Graphics state:** Redundant `graphics_context_set_fill_color` calls are
   avoided for consecutive runs of the same glyph color. The actual paint
   rectangles remain identical.
3. **Font metadata:** Glyph-offset fields narrow safely from 32 to 16 bits;
   tests check that each atlas offset plus glyph length is within its array.
4. **Release hygiene:** Removed obsolete v1.0.10 concept screenshots, updated
   outdated store copy/version references, and replaced the placeholder author.
5. **Dead-code audit:** Strict GCC/Clang unused-function checks pass. Pebble
   callbacks registered by pointer are required, not orphan functions.

## Measured evidence (host mocks, NOT hardware)

| Check | v1.0.24 | v1.0.25 |
|---|---:|---:|
| Native text layout measurements in 2,000 unchanged redraws | 8,000 | 4 |
| Graphics fill-color state calls in same workload | 1,576,000 | 1,322,000 |
| 26-setting full renderer trace | `0518e2bf52dd0e18` | `0518e2bf52dd0e18` |
| GCC -Os object `.text`+read-only data (bytes) | 56,675 | 56,763 |
| GCC object `.bss` (bytes) | 160 | 288 |

The optimized renderer makes substantially fewer redundant calls while keeping
pixel-output operations equivalent. The object gains **88 bytes of code/const
data** and **128 bytes of BSS** for caching; it is not smaller overall at the
compiled-code level. The package is leaner because stale previews are removed.
Actual watch energy/battery savings and Pebble SDK linking are unverified.

## Safety and compatibility

- Original app UUID, 26 message keys, and saved `SETTINGS_VERSION = 6` remain.
- Settings migration from older persisted layouts remains.
- No visual change, altered font data, network usage, new permissions, or
  added background timers.
- All host regression, sanitizer, and strict compilation checks passed.
- Font license retained for embedded Noto-derived outline data.
