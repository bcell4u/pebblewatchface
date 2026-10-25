# Minimal Plus — Pebble Time (Basalt)

Minimal Plus is a configurable C watchface for the **original Pebble Time (144 × 168 pixel, Basalt)**.

**Version 1.0.1** fixes configuration delivery and a watch-layer bug that could cause the face to stop responding. It also sets a safe maximum native font for the 100% clock-size option and publishes every successful GitHub Actions build to **GitHub Releases** automatically.

## Your configurable controls

- **Clock:** 12-hour format, stacked (hour above minute) or single-line, adjustable clock size 50–100% (default 72%), optional AM/PM label.
- **Date:** abbreviated weekday, month and day (`MON APR 28`), with colored accent rule.
- **Battery:** indicator on/off; show icon and percentage separately; battery top-right/bottom-right (default bottom); icon size 30–100% (default 40%); text size 30–100% (default 50%).
- **Colors:** independent settings for the background, hours, minutes, AM/PM, accent, date, battery icon and battery text.
- **Persistence:** changes are saved to watch storage and to Clay's phone-side settings store.

**How to apply settings:** Open the gear/settings control for Minimal Plus in your phone's Pebble app, make your changes, then tap **Save Watchface Settings**. The watchface updates once the settings are transmitted. Moving a slider without tapping Save does not yet update the watch live.

Pebble system fonts are available in fixed sizes. The 50–100% controls select safe font-size steps, rather than scaling every font one percent at a time. The 100% option now caps at Bitham 42 instead of using a particularly large font. The watch supports a 64-color display palette.

## GitHub Actions: build and publish to Releases

1. Create or open your GitHub repository for Minimal Plus.
2. Upload/replace **all the files in this archive**, including the hidden `.github/workflows/build.yml` file. Preserve the watchface `uuid` in `package.json` to update the same installed watchface.
3. Commit and push to **`main`** (or `master`). The **Build and release Pebble Time watchface** workflow starts automatically. You can also run it manually from **Actions → Build and release Pebble Time watchface → Run workflow**.
4. If *all* checks and the Pebble SDK build succeed, the workflow uploads the `.pbw` both as an Actions artifact and as an asset in a newly created GitHub **Release**. Open your repository's **Releases** tab to download it.

**Release behavior:**

- Each successful push or manual build on a branch creates a *prerelease* under a unique `build-<run number>` tag, e.g. `build-23`, and attaches the `.pbw`.
- Pushing a version tag, e.g. `v1.0.1`, creates a normal release with that tag and attaches the `.pbw`.
- **Unsuccessful builds or failing tests do not create a release.**
- GitHub Actions requires permission to publish releases. This project includes `permissions: contents: write`; if your organization overrides that permission and blocks releases, an administrator may need to allow the action.

The first workflow run downloads the current Pebble CLI/SDK and the `@rebble/clay` package. GitHub's hosted runner needs network access for this. If a workflow fails, consult its Actions log to see whether the failure came from the tests, SDK installation, Pebble build or GitHub Releases upload.

## Building locally

Follow [Pebble SDK installation](https://developer.repebble.com/sdk/). Then run:

```bash
pebble package install @rebble/clay
pebble build
```

The compiled `.pbw` is written to `build/`. Install it with your connected Pebble Time via Pebble developer connection or test in the `basalt` emulator:

```bash
pebble install --emulator basalt
```

## Local regression checks (no Pebble SDK needed)

Run from the repository root:

```bash
clang -std=c11 -Wall -Wextra -Werror -Wno-unused-parameter -Itests/mocks -o /tmp/minimal-plus-watch-tests tests/watch_test.c
/tmp/minimal-plus-watch-tests
node tests/transport_test.js
```

These test 100% clock rendering geometry and layout switching, all 17 preference keys, toggles, battery positions, saved-settings round trips and sequential AppMessage delivery with a simulated transient failure. The tests run *before* Pebble compilation in GitHub Actions and block release publication if they fail.

**These are host-side regression tests, not real device/emulator tests.** Before submitting publicly, install a successful `.pbw` and test the configuration on your physical Pebble Time. A true SDK binary was not buildable in the source-generation environment because its network connection couldn't reach the SDK downloads.

## Troubleshooting settings

If a saved change does not appear, confirm the new `.pbw` was installed, reopen the settings page, and press the **Save Watchface Settings** button. You can inspect the PebbleKit JS logs for `Minimal Plus: all 5 settings packets delivered` or a delivery failure; watch logs note `Applied watchface settings` or `Settings AppMessage dropped`.

Because this release keeps the same watchface UUID and persisted settings structure, updating over v1.0.0 should preserve prior choices. If old phone-side settings conflict with the watch's stored settings, explicitly press Save once after updating.

## Rebble Developer Portal

1. Compile and *test* the resulting `.pbw` for Basalt.
2. Open [Rebble Developer Portal](https://dev-portal.rebble.io/), create or update the watchface listing.
3. Use the `.pbw` file from the GitHub **Release**, not this source ZIP.
4. Submit a genuine 144 × 168 Pebble Time screenshot, not the concept mockup included under `store_assets/`.
5. Replace `author` in `package.json` with your preferred developer name before publishing.

## File map

- `src/c/main.c` — native drawing, safe fonts, time/date/battery, settings persistence and message reception.
- `src/pkjs/index.js` — Clay page and sequential/retried settings transport.
- `src/pkjs/config.js` — all phone-side controls.
- `.github/workflows/build.yml` — tests, SDK build, artifact upload and **GitHub Release publication**.
- `tests/` — native watch behavior and JS transport regression tests.
- `STORE_LISTING.md` — draft portal listing and change notes.
- `store_assets/` — illustrations (not actual watch screenshots).

The compiled `taller_watchface.pbw` from the earlier conversation is not used or repackaged. Minimal Plus is an independent project with its own UUID.
