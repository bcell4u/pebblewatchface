# Minimal Plus v1.0.25 — Rendering Efficiency

- Date layout is calculated only when its dependencies change, rather than
  every minute. More efficient for both native and large antialiased text.
- Less redundant graphics state-setting for smooth clock/date glyphs.
- Compact glyph metadata reduces embedded lookup table size.
- Removed outdated concept thumbnails and corrected release metadata.
- No visual or settings changes; retained persistent storage version 6.
- Automatic GitHub PBW-only Releases remain enabled.

These optimizations have not been benchmarked for battery life on physical
Pebble Time hardware. Host tests and visual output parity passed; an ARM SDK
build and hardware check are still required.
