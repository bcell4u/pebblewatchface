# Minimal Plus v1.0.25 — Performance Audit Release

Pebble Time (`basalt`) minimalist watchface with 26 customization settings. This
version improves rendering efficiency without changing the visual appearance,
saved preferences, or the configuration message keys.

## For CloudPebble

Replace **only** `src/c/main.c` in your existing project, then rebuild for
**Basalt**. Keep `src/pkjs/config.js` and your 26 PebbleKit JS message keys.
This is a source update; a compiled PBW is not bundled here.

## For GitHub

Push all files, including `.github/workflows/build.yml`. GitHub Actions runs
the tests, compiles the PBW and automatically attaches the **PBW only** to a
GitHub Release after a successful build. An explicit `v1.0.25` tag creates a
normal release; ordinary pushes create prereleases.

## Runtime changes

- Cache weekday/date fitting until a relevant setting, calendar string, or
  display dimension changes. Clock minute updates no longer measure static
  date text every time.
- Avoid redundant fill-color changes inside antialiased glyph runs.
- Store safe glyph offsets in 16 bits instead of 32 bits.
- Keep existing once-per-minute updates, battery change checks, and
  lightweight integer-only clock layout.
- No diagnostic logs or unnecessary background polling.

All 26 options, the screenshot-matched defaults, original watchface UUID, and
saved-settings format `SETTINGS_VERSION = 6` remain unchanged. Existing user
preferences are not forcibly reset.

## Verification

- `clang -std=c11 -O2 -Wall -Wextra -Werror -Wno-unused-parameter -Itests/mocks tests/watch_test.c -o /tmp/mp-test && /tmp/mp-test`
- `node tests/transport_test.js`
- `python3 tests/defaults_test.py`
- `gcc -std=c99 -Os -Wall -Wextra -Werror -Wno-unused-parameter -Itests/sdk_compat -c src/c/main.c -o /tmp/mp.o`

The package has been checked using host C compilers and sanitizers, and its
rendered-draw-call trace matches v1.0.24. The actual ARM Pebble SDK build must
still be verified with CloudPebble or the GitHub workflow.

## Rebble submission

Use screenshots captured from the actual watchface/emulator. Old concept
previews have intentionally been removed to prevent accidentally uploading
images that do not match the current release. Replace the generic
`package.json` author with your publishing identity if you wish.

See `AUDIT_REPORT.md`, `CHANGELOG.md`, and `FONT_ATTRIBUTION.md`.
