# Minimal Plus 1.0.27

- New **Center accent line horizontally** toggle; line centers on the actual display, independent of mirror setting.
- New **Center weekday and date horizontally** toggle; both fields center as one combined text group while retaining separate font sizes, boldness, and colors.
- Toggles default off, preserving the familiar existing alignment for current users.
- New first-install background color: dark Oxford Blue (`#000055`) in both C and Clay. Previously saved backgrounds remain unchanged.
- Corrected migration to preserve independent weekday and date colors from v1.0.26.
- Saved-settings schema moves from 6 to 7 with append-only migration; existing preferences are retained.
- Expanded to 28 customization options and added position/migration regression tests.
- Existing rendering performance optimizations and compact battery display remain unchanged.

Build target: Pebble Time (Basalt). The actual Pebble SDK build still requires verification.
