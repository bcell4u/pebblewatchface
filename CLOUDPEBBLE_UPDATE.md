# Minimal Plus v1.0.27 — CloudPebble update

1. In **Source Files**, replace `src/c/main.c` with the version included here.
2. Replace `src/pkjs/config.js` with the included new version.
3. In **Project Settings → PebbleKit JS Message Keys**, APPEND these two keys, in this order, after existing last key `WeekdayColor`:
   - `CenterAccent`
   - `CenterDateRow`
4. Do not reorder, rename, or remove any of the existing 26 keys. The two new keys must be appended as keys 27 and 28.
5. Save, build for **Basalt (Pebble Time)**, reinstall, and reopen the settings page on your phone.
6. Under **Accent and Date**, turn on either independent toggle as needed:
   - **Center accent line horizontally** — puts the line's midpoint at the screen midpoint.
   - **Center weekday and date horizontally** — centers the combined `MON OCT 28` text group; weekday/date still have separate size, weight and color.

Both toggles default to OFF to preserve previously selected left/right alignments. They work independently of Mirror Layout; mirror still affects the clock and battery.

## Important compatibility notes

- Previously stored v1.0.26 values (clock, colors, spacing, battery, etc.) are migrated without change. The two new toggles start off.
- The watchface's persistent schema is **SETTINGS_VERSION 7**, which is distinct from app version **1.0.27**. Do not manually substitute one for the other.
- Very large centered weekday/date text may share space with a battery group set to the bottom corner; select Battery Position → Top corner for more room.
- Replacing `config.js` alone will show toggles but not change on-watch alignment; `main.c` and both new message keys must also be updated.

## Dark blue default

The fresh-install background is Oxford Blue (`#000055`), set in both the watch C defaults and Clay's `BackgroundColor` default. This is one of the 64 Pebble Time colors. Previously saved colors are **not overwritten**. To select the new background on a watch already configured, choose the dark navy swatch in Background and save.
