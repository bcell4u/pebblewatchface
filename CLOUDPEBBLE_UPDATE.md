# Updating CloudPebble to Minimal Plus v1.0.25

1. Replace the **entire** `src/c/main.c` source in CloudPebble.
2. Keep your existing `src/pkjs/config.js`, project settings, and all 26 message keys.
3. Build for **Basalt (original Pebble Time)** and install.
4. Verify the saved appearance, battery position, mirror layout, color selectors,
   and 50–120% clock size. Existing saved settings are retained.

No new message keys or firmware permissions are required. The update removes no
customization controls and includes no watchface diagnostic logging.
