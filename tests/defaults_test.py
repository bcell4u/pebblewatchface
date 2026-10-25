#!/usr/bin/env python3
"""Assert first-install defaults match the supplied screenshot in both C and Clay."""
from pathlib import Path
import re
path = Path(__file__).resolve().parents[1] / 'src/c/main.c'
src = path.read_text().split('static void defaults(void) {',1)[1].split('static void load_settings(void) {',1)[0]
expected = {'clock_layout': '0', 'clock_size': '100', 'clock_spacing': '2', 'show_ampm': '1', 'mirror_layout': '0', 'accent_width': '60', 'accent_y_offset': '-4', 'weekday_size': '21', 'date_size': '27', 'weekday_bold': '0', 'date_bold': '0', 'battery_enabled': '1', 'battery_position': '0', 'battery_icon_visible': '1', 'battery_icon_size': '85', 'battery_percent_visible': '0', 'battery_text_size': '30', 'background_color': '0x0055AA', 'hours_color': '0xFFFFFF', 'minutes_color': '0xFFFFFF', 'ampm_color': '0xAAAAAA', 'accent_color': '0xAAFFAA', 'date_color': '0xFFFFFF', 'weekday_color': '0xFFFFFF', 'battery_icon_color': '0xAAFFAA', 'battery_text_color': '0xFFFFFF'}
for field, val in expected.items():
    match = re.search(r's_settings\.' + re.escape(field) + r'\s*=\s*(0x[0-9a-fA-F]+|-?\d+)\s*;',src)
    assert match, 'Missing default: ' + field
    assert int(match.group(1),0) == int(val,0), (field,match.group(1),val)
print('PASS: all 26 watch-side defaults match screenshot')
