# Diagnostic release v1.0.3 (not a verified fix)

This source adds `MPDIAG` logging around startup, settings, persistence and drawing; it does not repair an identified crash. Build workflow now publishes the matching `.elf` symbol file as well as the `.pbw`. Read [DIAGNOSE.md](DIAGNOSE.md) before testing.

# Minimal Plus — Pebble Time (Basalt)

Minimal Plus is a configurable C watchface for the **original Pebble Time (144 × 168 pixel, Basalt)**.

**Version 1.0.2** revisits the reported issue of settings not updating and crashes when adjusting clock size. It restores Clay's supported automatic AppMessage handling, removes dynamic text-width measurement from the watch's draw path, and adds stronger state-change tests. Every successful GitHub Actions build is published to **GitHub Releases** automatically.

## Your configurable controls

- **Clock:** 12-hour format, stacked (hour above minute) or single-line, adjustable clock size 50–100% (default 72%), optional AM/PM label.
- **Date:** abbreviated weekday, month and day (`MON APR 28`), with colored accent rule.
- **Battery:** indicator on/off; show icon and percentage separately; battery top-right/bottom-right (default bottom); icon size 30–100% (default 40%); text size 30–100% (default 50%).
- **Colors:** independent settings for the background, hours, minutes, AM/PM, accent, date, battery icon and battery text.
- **Persistence:** changes are saved to watch storage and to Clay's phone-side settings store.

**How to apply settings:** Open the gear/settings control for Minimal Plus in your phone's Pebble app, make your changes, then tap **Save Watchface Settings**. The watchface updates once the settings are transmitted. Moving a slider without tapping Save does not yet update the watch live.

Pebble system fonts are available in fixed sizes. The 50–100% controls select safe font-size steps, rather than scaling every font one percent at a time. The 100% option uses the supported Bitham 42 fonts for stacked mode and Bitham 34 for the single-line mode. For the stacked clock, size tiers are 50–59% (Gothic 24), 60–74% (Bitham 30), 75–89% (Bitham 34), and 90–100% (Bitham 42); intermediate percentages within a tier look the same. Single-line has three tiers. These limits are designed to avoid exceeding the 144-pixel width. The watch supports a 64-color display palette.

## GitHub Actions: build and publish to Releases

1. Create or open your GitHub repository for Minimal Plus.
2. Upload/replace **all the files in this archive**, including the hidden `.github/workflows/build.yml` file. Preserve the watchface `uuid` in `package.json` to update the same installed watchface.
3. Commit and push to **`main`** (or `master`). The **Build and release Pebble Time watchface** workflow starts automatically. You can also run it manually from **Actions → Build and release Pebble Time watchface → Run workflow**.
4. If *all* checks and the Pebble SDK build succeed, the workflow uploads the `.pbw` both as an Actions artifact and as an asset in a newly created GitHub **Release**. Open your repository's **Releases** tab to download it.

**Release behavior:**

- Each successful push or manual build on a branch creates a *prerelease* under a unique `build-<run number>` tag, e.g. `build-23`, and attaches the `.pbw`.
- Pushing a version tag, e.g. `v1.0.2`, creates a normal release with that tag and attaches the `.pbw`.
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

These test 100% clock rendering geometry (including 1,000 repeated draw calls), visible font/layout changes, all 17 preference keys applied together, independent colors, toggles, battery positions, saved-settings round trips, and Clay automatic-handler configuration. They do not emulate the phone-to-watch Bluetooth link. The tests run *before* Pebble compilation in GitHub Actions and block release publication if they fail.

**These are host-side regression tests, not real device/emulator tests.** Before submitting publicly, install a successful `.pbw` and test the configuration on your physical Pebble Time. A true SDK binary was not buildable in the source-generation environment because its network connection couldn't reach the SDK downloads.

## Troubleshooting settings

If a saved change does not appear, first confirm the `.pbw` actually came from a v1.0.2 build (Releases → latest build timestamp, not an older asset). In the watchface's configuration, make an unmistakable change such as turning the battery off or changing the background from black to white, then tap **Save Watchface Settings** (moving the slider alone does not transmit settings). The watch's developer log will print `Applied settings: layout=... size=...` if AppMessage reaches the watch, or `Settings AppMessage dropped` if the incoming message is rejected. If neither appears, inspect PebbleKit JS/Clay loading and phone connectivity. A new install may also require reopening the watchface before configuring it. An actual watch/emulator log is needed to distinguish any remaining device error from a transport error.

Because this release keeps the same watchface UUID and persisted settings structure, updating over v1.0.0 should preserve prior choices. Reopen the settings page and press Save once after updating; preferences remain stored on the watch unless the Pebble app clears its app data.

## Rebble Developer Portal

1. Compile and *test* the resulting `.pbw` for Basalt.
2. Open [Rebble Developer Portal](https://dev-portal.rebble.io/), create or update the watchface listing.
3. Use the `.pbw` file from the GitHub **Release**, not this source ZIP.
4. Submit a genuine 144 × 168 Pebble Time screenshot, not the concept mockup included under `store_assets/`.
5. Replace `author` in `package.json` with your preferred developer name before publishing.

## File map

- `src/c/main.c` — native drawing, safe fonts, time/date/battery, settings persistence and message reception.
- `src/pkjs/index.js` — Clay's supported automatic configuration and AppMessage delivery.
- `src/pkjs/config.js` — all phone-side controls.
- `.github/workflows/build.yml` — tests, SDK build, artifact upload and **GitHub Release publication**.
- `tests/` — native watch rendering/settings tests and JS configuration/schema checks.
- `STORE_LISTING.md` — draft portal listing and change notes.
- `store_assets/` — illustrations (not actual watch screenshots).

The compiled `taller_watchface.pbw` from the earlier conversation is not used or repackaged. Minimal Plus is an independent project with its own UUID.
