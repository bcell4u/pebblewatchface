# v1.0.5 build compatibility fix

The 1.0.4 code failed under the real Pebble SDK in CloudPebble because Pebble `Tuple.value[]` is a flexible array member that cannot be null-tested, and generated `MESSAGE_KEY_*` values are **not** C compile-time constants. They cannot be used as `case` labels.

`src/c/main.c` now checks only `tuple` and `out` for null; checks the tuple type and `length` before reading its value; and handles keys using `if / else if` comparisons instead of `switch`. Diagnostic checkpoints and the v1.0.4 parser validation are preserved.

This revision addresses compiler errors only. It is **not yet verified to fix the earlier device crash**; reproduce the settings change and inspect `MPDIAG` logs.

## CloudPebble steps

Replace your CloudPebble `src/c/main.c` with the `main.c` file from this project, save it, build the Basalt/Pebble Time emulator, start logs, then change Battery Position.

On failure, send the last 10 `MPDIAG` lines and the App fault line.

## GitHub

Upload the whole project, including `.github/workflows/build.yml`, for automatic `.pbw` and `.elf` GitHub Releases on successful builds.
