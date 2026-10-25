# Minimal Plus v1.0.3 diagnostic test

This build deliberately does not change how the watchface draws or processes settings. It adds `MPDIAG` checkpoints around startup, rendering, settings parsing and persistence. The `.elf` released alongside the `.pbw` allows the precise fault address to be resolved.

## No WSL required

1. Replace the repository contents with this source, commit, and push to `main`.
2. After GitHub Actions succeeds, open GitHub **Releases** and download the `.pbw` plus the **basalt** `pebble-app.elf` (if GitHub lists the ELF by that basename). If the action cannot find an ELF, it fails without publishing a misleading release.
3. **For a browser-only test**, update your project in CloudPebble to this diagnostic source (the `src/c/main.c` file is the key change). Build & Run on the **Basalt (Pebble Time)** emulator. Open **View App Logs**, and reproduce one change (e.g. Battery Bottom Right → Top Right).
4. Copy the `MPDIAG` messages plus the fault's `PC: 0x...` value and send those here. The last completed checkpoint can help isolate the failure even without an ELF.
5. **Important**: the `.elf` from GitHub Actions will only symbolicate a crash from the `.pbw` compiled by that **same GitHub Actions build**, not one built separately by CloudPebble. For symbolication, reproduce the crash with the GitHub-built `.pbw` on your real watch, or retrieve the matching ELF from the CloudPebble build if CloudPebble offers it. Send that matching `.elf` and its crash PC here.
6. If no `MPDIAG` messages appear, mention that specifically: it may mean the previous version is still installed, or the log viewer filtered or did not capture app logs.

## Interpret the last completed checkpoint

- `300` only: crash while parsing incoming settings.
- `310` only: crash while entering persistence.
- `320` without `329`: crash inside persistence.
- `330` without `339`: crash on `layer_mark_dirty`.
- `200` without `205`: initial graphics setup.
- `205` without `209`: clock drawing.
- `209` without `229`: date or battery drawing.
- `229`: entire drawing callback returned.

PC symbolication is more reliable than checkpoint inference. Remember log buffering may omit the very last checkpoint.

## Notes

Do not submit this verbose diagnostic build to the Rebble store. The normal automatic release step remains, but these releases are for debugging. The original functionality and Clay settings remain unchanged.
