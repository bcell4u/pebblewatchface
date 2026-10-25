# Minimal Plus 1.0.10 — Pebble Time (Basalt)

Minimal Plus is a minimal, configurable watchface for the original 144×168 color Pebble Time. It uses the safe Clay settings receiver from v1.0.6, the smoothed numeral glyphs and compact battery layout from v1.0.9, and adds the following **four** persisted options:

- **Accent Width** (8–120 pixels; default 29). The line grows inward from its aligned edge.
- **Weekday Size** (10–24 pixels; default 14). Controls `MON` independently.
- **Date Size** (10–24 pixels; default 14). Controls `APR 28` independently.
- **Mirror Layout** (off by default). Clock and accent align to the right, weekday/date become right-aligned as a group, and the battery moves to the **left**. The battery icon and percentage both use the same left margin. Characters are never reversed.

The weekday and date stay on one line. Their glyphs scale at each slider increment; if the two words become too wide, the watchface compresses them horizontally to stay within the remaining width, particularly when the battery is at the bottom. The watchface retains all original color controls and battery position/size/visibility settings, and supports both stacked and one-line clocks.

## CloudPebble (easiest)

1. Open your existing project at https://cloudpebble.repebble.com/.
2. Replace `src/c/main.c` with the **whole** `main.c` in this project.
3. Replace `src/pkjs/config.js` and `package.json` too. **This is required**: all three files must match for the four new settings to appear and work.
4. Do not change `src/pkjs/index.js` or the app UUID.
5. Build for **Basalt**, install, and open the settings page again. You may need to close and reopen settings to refresh Clay's cached configuration.

**Replacing only `main.c` is not enough in this release.** The new sliders/toggle must also be registered in the JavaScript configuration and `package.json` AppMessage keys.

## GitHub Releases

Upload this entire project, including hidden `.github/workflows/build.yml`, to your GitHub repository and push to the default `main` branch. The workflow runs local tests and Pebble SDK compilation, uploads the `.pbw` and matching `.elf`, and creates a prerelease for successful automatic builds. A version tag (e.g. `v1.0.10`) creates a normal release.

## Upgrading from v1.0.9

Persisted settings v1 are automatically read and migrated to v2 by appending four new settings with the defaults above. Existing clock size, color, battery size and position preferences are preserved. The original app UUID has not changed, so you can update the existing install.

## Validation

- `node tests/transport_test.js`
- `gcc -std=c11 -Wall -Wextra -Werror -Wno-unused-parameter -Itests/mocks -o /tmp/watch-test tests/watch_test.c && /tmp/watch-test`
- `clang -std=c99 -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-function -Itests/sdk_compat -c src/c/main.c -o /tmp/watch-sdk.o`

The local tests also cover 21 settings, short Clay strings, date fitting, mirror geometry in both layouts, migration of existing persistent preferences and 1000 maximum-scale redraws. The code has not yet been compiled with the actual Pebble SDK or verified on a physical device in this environment.

`store_assets/v1_0_10_alignment_preview.png` is a composite of watchface rectangles drawn by a local C harness plus illustrative AM/PM and percentage text. It is a **concept preview**, not a screenshot from CloudPebble or the real watch. For store submission, capture a real Basalt emulator screenshot.
