# Changelog

## v1.0.2 — Configuration and layout hardening

- Replaced the custom phone-to-watch packetizer with `@rebble/clay`'s documented automatic configuration events and settings delivery. This eliminates potential incomplete multi-packet updates.
- Removed `graphics_text_layout_get_content_size()` from the watchface's update callback, and replaced dynamic width-fitting with deterministic, fixed rectangle placements for the 144 x 168 display.
- Capped single-line fonts at Bitham 34; stacked layout at Bitham 42. Both use safe fixed native sizes rather than allocating additional font objects or measuring text during each redraw.
- Validated saved size and visibility preferences when reloading settings from persistent storage.
- Improved regression tests: all 17 keys received as one dictionary, visible layout/size/color/battery changes, persistence, and 1,000 maximum-size redraws with bounds checking.
- Kept automated GitHub Actions builds and attaching successful `.pbw` artifacts to GitHub Releases.

**Limitations:** These are host-side tests with an SDK mock. Physical Pebble hardware / Bluetooth delivery has not been tested in this environment.

## v1.0.1 — Settings and stability fix

- Removed a duplicated `layer_add_child` call that could destabilize repeated draws.
- Limited the 100% clock option to a smaller, supported native system font, with a bounded single-line fit pass.
- Replaced Clay's default one-shot settings send with sequential, four-key AppMessage batches and three-attempt retry logging.
- Added defensive number parsing, AppMessage receive failure logging, and persistence checks.
- Added unit/regression tests for all 17 settings, max-size drawing, saved preferences, and phone-to-watch transport.
- GitHub Actions now creates/updates a GitHub Release *after* a successful compile and uploads the `.pbw` asset. Branch/manual builds become uniquely tagged prereleases; `v*` tags become normal releases.

## v1.0.0 — Initial source release

- Initial Minimal Plus watchface and configurable Clay page.
