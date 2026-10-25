# Test checklist for v1.0.2 on an original Pebble Time

The host-side C tests cannot reproduce all PebbleOS exceptions. Before submitting
this as a public Rebble store update, install the .pbw from the latest GitHub
Release (NOT the source ZIP) and test the device itself.

1. Verify the PBW was built from this v1.0.2 project. Do not install the earlier
   v1.0.1 GitHub Release from an outdated workflow run.
2. Open the watchface Settings in the Pebble mobile app. Change Background from
   Black to White, then press **Save Watchface Settings**.
   - Expected: background changes from black to white almost immediately.
   - If not, the issue is phone/Clay/AppMessage transport (not clock rendering).
3. Set Background back to Black. Toggle **Enable battery indicator** OFF and
   save; expected: both icon and percentage disappear.
4. Turn battery on. Switch Top right / Bottom right, save each time.
5. Disable just the icon; verify percentage remains. Re-enable and disable
   just percentage; verify the icon remains.
6. Set Clock layout to Single line and back to Stacked, saving after each.
7. Change clock size from 50% to 100% and save; watchface should stay open.
   Note: system fonts use several tiers rather than an exact 1% size scale.
8. Adjust battery icon size from 30% to 100%, battery text from 30% to 100%,
   plus change their colors, and save.
9. Exit/reopen the face and verify the last saved settings persist.

If any change fails:
- If the face remains unchanged, check phone/Clay logs and watch logs for
  `Applied settings: layout=... size=...` (watch received the message) or
  `Settings AppMessage dropped` (transport failure).
- If it crashes, capture the Pebble OS crash report or the last watch log lines,
  plus the exact selected settings and firmware version. A mock rendering test
  alone cannot determine the watch's root exception.

GitHub Actions still builds the watchface and automatically attaches `.pbw`
files to GitHub Releases after successful tests and compilation.
