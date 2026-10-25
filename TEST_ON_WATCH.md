# Minimal Plus v1.0.10 — CloudPebble / device test checklist

1. Replace **three files** in your existing CloudPebble project: `src/c/main.c`, `src/pkjs/config.js`, and `package.json`. Save all three and rebuild for Basalt.
2. Confirm the four new settings appear: Accent Width, Weekday Size, Date Size, Mirror Layout.
3. Set Accent Width to 8, 29 and 120. Confirm it changes and stays anchored at the appropriate aligned edge.
4. Increase Weekday Size with Date Size unchanged; then adjust Date Size with Weekday Size unchanged. At large sizes, horizontal fitting should prevent collision with the bottom battery.
5. Toggle Mirror Layout: the clock and date should switch from left to right; battery icon and text should switch to the left with a fixed 10px margin regardless of icon size.
6. Repeat mirror switching with Battery Position set to **Top** and **Bottom**, and with the battery icon/text individually hidden. Text size should never reset.
7. Test Clock Layout in both **Stacked** and **Single line** with mirror mode on/off, and clock size at 50%, 72% and 100%.
8. Change a color, clock size and battery text size. Restart the watchface and confirm all prior preferences remain. Users upgrading from v1.0.9 should retain the old settings.
9. On GitHub, push to `main`; after a successful workflow check the `.pbw` and `.elf` in the new **Releases** prerelease.

If anything crashes, send the last `MPDIAG` messages and `App fault! PC` line. Run the tests before publishing to Rebble.
