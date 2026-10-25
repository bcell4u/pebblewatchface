# Minimal Plus v1.0.5 - Settings parser and compilation diagnostics

## What the real v1.0.3 logs proved

The Pebble Time emulator drew the entire watchface successfully and repeatedly logged `MPDIAG 229 canvas end`. Upon receiving a settings message, it logged `MPDIAG 300 inbox begin` and immediately faulted at `PC: 0x11ec`. It never logged `MPDIAG 310 inbox parsed`.

The failure is in the inbound-settings handling path (or a function it invokes), before persistence or redraw. This does **not** prove which specific expression is faulty. The earlier parser did not inspect each tuple's actual `length` and used repeated `dict_find()` lookups.

## What's different here

- One pass over the AppMessage dictionary with documented `dict_read_first` / `dict_read_next` APIs.
- Decode little-endian integers only when the tuple declares an actual length of 1, 2, or 4 bytes; sign extend signed 8-/16-bit values.
- Bound and verify Clay strings, including select options (`"0"`/`"1"`), booleans (`"true"`/`"false"`), and color strings (`"#RRGGBB"` or `"0xRRGGBB"`).
- Validate values before storing and redraw only when a setting actually changes.
- Additional `MPDIAG 301`, `302`, `303`, `305`, `306`, `310`, `320`, `329`, and `339` markers.

## No WSL required - use CloudPebble emulator

1. Update your CloudPebble project's source with `src/c/main.c` from this archive (and keep `package.json`, `config.js`, and `index.js` in sync).
2. Select **Basalt** under Build & Run and choose **Install and Run**.
3. Open **View App Logs**.
4. Save just one setting, e.g. **Battery Position: Top Right**, and look for `MPDIAG 310 inbox parsed ...` and `MPDIAG 339 applied ...`.
5. Confirm the position actually changes, and then test clock size **100%**. If the app crashes, copy the last 15-25 `MPDIAG` lines and the new `PC: 0x...` fault address.

## Log interpretation

- `300` but not `301`: fault before first dictionary read.
- `301` but not `302`: fault during `dict_read_first()`.
- `302` but not `303`: crash before accessing the first tuple header.
- `303` but not `305` or `306`: fault in tuple value conversion or key dispatch.
- `305` / `306` but not `310`: fault advancing the iterator or processing a later tuple.
- `310` but not `329`: crash while updating or persisting settings.
- `339` followed by `200`: settings applied and canvas redraw started.

### Build and release

The GitHub Actions workflow runs local C and JS tests first. If a Pebble SDK build then succeeds, it publishes the `.pbw` and matching `.elf` automatically to GitHub Releases. **An ELF file only identifies the address for the exact binary built with it**; it cannot reliably symbolize a different CloudPebble build.

This is a **candidate fix, not device-verified**. Do not submit to the Rebble store until it passes real watch testing.
