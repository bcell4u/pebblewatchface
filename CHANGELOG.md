# Changelog

## v1.0.1 — Settings and stability fix

- Removed a duplicated `layer_add_child` call that could destabilize repeated draws.
- Limited the 100% clock option to a smaller, supported native system font, with a bounded single-line fit pass.
- Replaced Clay's default one-shot settings send with sequential, four-key AppMessage batches and three-attempt retry logging.
- Added defensive number parsing, AppMessage receive failure logging, and persistence checks.
- Added unit/regression tests for all 17 settings, max-size drawing, saved preferences, and phone-to-watch transport.
- GitHub Actions now creates/updates a GitHub Release *after* a successful compile and uploads the `.pbw` asset. Branch/manual builds become uniquely tagged prereleases; `v*` tags become normal releases.

## v1.0.0 — Initial source release

- Initial Minimal Plus watchface and configurable Clay page.
