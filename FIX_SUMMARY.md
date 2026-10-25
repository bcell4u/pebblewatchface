# 1.0.9 fixes

**Battery text size resets:** The watch-side parser retains `battery_text_size` when only `BatteryPosition` changes. Tests cover both positions at 30% and 100% icon sizes while keeping battery text at 95%. If the phone resends an old text-size value as part of a full settings payload, `MPDIAG 326` records what was actually received so we can distinguish a Clay UI issue from a drawing issue.

**Battery right margin:** Both icon (including terminal) and percentage text share right edge x=134 on a 144px Pebble Time screen, independent of icon dimensions.

**Jagged clock:** Replaced single-color hard-edged rectangle shapes with 4-level antialiased glyph coverage that Pebble Time can actually display.

**Minutes clipping:** Transparent vertical safety margin prevents cut-looking tops and bottoms. The smaller visual gap between clock rows is preserved.
