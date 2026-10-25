# Minimal Plus — Original Pebble Time (Basalt)

Minimal Plus is a native, low-distraction, configurable watchface for the **original Pebble Time (144 x 168 pixels)**. It targets **basalt only**; it is not intended for Pebble 2 or Pebble Time Round.

## Included features

- 12-hour clock with optional AM/PM label.
- Two layouts: stacked (hours above minutes), or single-line `10:32`.
- Main clock size: 50–100%, default **72%**. Pebble's built-in fonts provide discrete effective sizes, and single-line text shrinks automatically to fit.
- Date: `MON APR 28`, with thin accent rule.
- Battery on/off, battery icon on/off, percentage text on/off.
- Battery in top-right or bottom-right, default **bottom-right**.
- Independent battery icon size **30–100%** (default **40%**) and percentage font size **30–100%** (default **50%**). Text size maps to supported Pebble system fonts.
- Independent colors: background, hours, minutes, AM/PM, date, accent, battery icon and battery percentage.
- The watch stores settings across restarts; Clay stores them on the phone as well.
- Full native C watchface; no external weather, tracking, hosted settings website or other network dependencies during ordinary use.

## Build a real `.pbw` package

The source ZIP **is not itself the `.pbw` app binary**. The Rebble Developer Portal requires a compiled `.pbw` created with the Pebble SDK.

### Easiest: GitHub Actions (no local SDK required)

1. Create a new empty GitHub repository and upload **all contents of this project**, including the hidden `.github/` directory.
2. In the repository, open **Actions → Build Pebble Time watchface → Run workflow** (or push to `main`).
3. When the workflow succeeds, download the **MinimalPlus-PebbleTime-PBW** artifact from the workflow run.
4. Extract the artifact ZIP; the contained file ending in `.pbw` is the binary to upload to [Rebble Developer Portal](https://dev-portal.rebble.io/).

The first build fetches the current Pebble SDK, Pebble CLI, and `@rebble/clay` dependency, so GitHub must have network access. You can find exact build errors in the Actions log if tool versions change.

### Local SDK

Follow the [official Pebble SDK install guide](https://developer.repebble.com/sdk/). With `pebble` installed and the SDK fetched:

```sh
cd minimal_plus_rebble
pebble package install @rebble/clay
pebble build
```

The resulting file will be in `build/`. For testing: `pebble install --emulator basalt` or install on a connected Pebble Time through the Pebble developer connection.

## Submit to Rebble

1. Build, **install**, and check the `.pbw` using an actual Pebble Time or the **basalt** emulator; test the settings page and battery top/bottom modes before public release.
2. In [Rebble Developer Portal](https://dev-portal.rebble.io/), choose **Submit a new app or face → Watchface**.
3. Title: `Minimal Plus`; description and release notes are in `STORE_LISTING.md`.
4. Upload **at least one actual emulator/device screenshot for Basalt**, then the compiled `.pbw` under App Binary.
5. Update `author` in `package.json` to your preferred developer name. The UUID is pre-generated for this *new* watchface; preserve it for future updates. Set the source URL if you publish your repository.

The included preview is an **illustrative design mockup**, not a screenshot captured from the Pebble emulator. A genuine emulator/device capture is recommended for submission.

## Notes and constraints

- Pebble Time has a 64-color hardware palette, so Clay's color picker uses the supported palette. Mint is represented as `0x55FFAA`.
- Clock and battery font sizes are discretized to built-in Pebble fonts; icon drawing scales continuously by pixel dimensions.
- This source was authored and checked for static syntax, but could **not be compiled or tested on the Pebble emulator** in the creation environment (Pebble SDK unavailable). The GitHub Actions workflow is provided to perform the required genuine build and test flow.
- The `taller_watchface.pbw` uploaded as inspiration is **not embedded or rebranded**. Minimal Plus is an independent implementation and uses its own UUID.

## Project files

- `src/c/main.c` — watchface drawing, calendar, battery, AppMessage, persistence.
- `src/pkjs/index.js` — PebbleKit JS Clay setup.
- `src/pkjs/config.js` — all phone-side user preferences.
- `wscript`, `package.json` — SDK build definitions.
- `.github/workflows/build.yml` — GitHub build producing a `.pbw` artifact.
- `store_assets/` — a rendered **preview**, for reference (not a device screenshot).
- `STORE_LISTING.md` — text for the developer portal.
