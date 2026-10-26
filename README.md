# Minimal Plus v1.0.27 — Independent Horizontal Centering

A customizable watchface for the original 144×168 Pebble Time (Basalt) display.

## New in 1.0.27

Two independent Clay toggles in **Accent and Date**:

- **Center accent line horizontally**: centers the 8–120 px accent by its actual width, regardless of mirror layout.
- **Center weekday and date horizontally**: centers the *combined* weekday and month/day group; retains independent date/weekday sizes, weights and colors.

The first-install background defaults to **Oxford Blue (#000055)**, a dark-blue Pebble 64-color swatch. Existing saved background choices remain intact.

Both default to **Off**, so the original left-justified or mirrored-right alignment remains the initial appearance. The rest of the clock and battery layout is unaffected. The date layout cache incorporates the new centering flag, preventing unnecessary re-measurement on ordinary minute redraws.

The watchface retains 28 settings, including the compact 7px battery percentage beneath the top-corner icon at the smallest text size, scalable clock, accent vertical adjustment, and all prior color choices.

## CloudPebble

See [CLOUDPEBBLE_UPDATE.md](CLOUDPEBBLE_UPDATE.md). Replace `main.c` and `config.js`, then **append** `CenterAccent` and `CenterDateRow` after `WeekdayColor` in CloudPebble's PebbleKit JS Message Keys. Never reorder previously defined keys. Build for Basalt and reinstall.

## GitHub Releases

The `.github/workflows/build.yml` workflow runs compatibility and settings tests before building and publishing a `.pbw`. On a successful push, it publishes a prerelease; pushing the tag `v1.0.27` creates a versioned release.

## Saved preferences

Stored data migrates from persistent schema versions 1–6 to version 7. Existing values are retained; only the new toggles default off. The version of the *app* is 1.0.27, while the binary saved-settings schema is 7.

## Verification

Host GCC and Clang compiler tests, integration tests, and Clay schema/defaults checks are provided in `tests/`. The actual Pebble SDK/ARM compilation and on-device visual behavior must be verified separately in CloudPebble or GitHub Actions.
