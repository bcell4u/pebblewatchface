#include <pebble.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SETTINGS_KEY 100
#define SETTINGS_VERSION 1

// Hex colors are stored so every display element can be customized separately.
typedef struct {
  uint32_t version;
  int32_t clock_layout;       // 0 = stacked; 1 = single-line
  int32_t clock_size;         // 50..100, default 72
  int32_t show_ampm;
  int32_t battery_enabled;
  int32_t battery_position;   // 0 = top-right; 1 = bottom-right
  int32_t battery_icon_visible;
  int32_t battery_percent_visible;
  int32_t battery_icon_size;  // 30..100, default 40
  int32_t battery_text_size;  // 30..100, default 50
  int32_t background_color;
  int32_t hours_color;
  int32_t minutes_color;
  int32_t ampm_color;
  int32_t accent_color;
  int32_t date_color;
  int32_t battery_icon_color;
  int32_t battery_text_color;
} FaceSettings;

static Window *s_window;
static Layer *s_canvas;
static FaceSettings s_settings;
static BatteryChargeState s_battery;
static char s_hours[4];
static char s_minutes[4];
static char s_single[8];
static char s_date[24];
static char s_ampm[4];

static int clamp_int(int value, int minval, int maxval) {
  if (value < minval) return minval;
  if (value > maxval) return maxval;
  return value;
}

static void defaults(void) {
  memset(&s_settings, 0, sizeof(s_settings));
  s_settings.version = SETTINGS_VERSION;
  s_settings.clock_layout = 0;
  s_settings.clock_size = 72;
  s_settings.show_ampm = 1;
  s_settings.battery_enabled = 1;
  s_settings.battery_position = 1;
  s_settings.battery_icon_visible = 1;
  s_settings.battery_percent_visible = 1;
  s_settings.battery_icon_size = 40;
  s_settings.battery_text_size = 50;
  s_settings.background_color = 0x000000;
  s_settings.hours_color = 0xFFFFFF;
  s_settings.minutes_color = 0xFFFFFF;
  s_settings.ampm_color = 0xAAAAAA;
  s_settings.accent_color = 0x55FFAA; // nearest Pebble-compatible mint
  s_settings.date_color = 0xFFFFFF;
  s_settings.battery_icon_color = 0x55FFAA;
  s_settings.battery_text_color = 0xFFFFFF;
}

static void load_settings(void) {
  defaults();
  if (persist_exists(SETTINGS_KEY) && persist_get_size(SETTINGS_KEY) == sizeof(s_settings)) {
    FaceSettings restored;
    if (persist_read_data(SETTINGS_KEY, &restored, sizeof(restored)) == sizeof(restored) &&
        restored.version == SETTINGS_VERSION) {
      s_settings = restored;
      s_settings.clock_layout = clamp_int(s_settings.clock_layout, 0, 1);
      s_settings.clock_size = clamp_int(s_settings.clock_size, 50, 100);
      s_settings.show_ampm = clamp_int(s_settings.show_ampm, 0, 1);
      s_settings.battery_enabled = clamp_int(s_settings.battery_enabled, 0, 1);
      s_settings.battery_position = clamp_int(s_settings.battery_position, 0, 1);
      s_settings.battery_icon_visible = clamp_int(s_settings.battery_icon_visible, 0, 1);
      s_settings.battery_percent_visible = clamp_int(s_settings.battery_percent_visible, 0, 1);
      s_settings.battery_icon_size = clamp_int(s_settings.battery_icon_size, 30, 100);
      s_settings.battery_text_size = clamp_int(s_settings.battery_text_size, 30, 100);
    }
  }
}

static GColor color(int32_t hex) { return GColorFromHEX((uint32_t)hex); }

// Deterministic font selection. Never allocate fonts, measure glyph widths, or
// perform iterative text layout inside a display refresh. Pebble Time has a
// small render budget and doing those operations at the large end is fragile.
// The 50..100% slider maps to discrete, safe system-font size tiers.
static GFont clock_font(int scale, bool minutes, bool inline_clock) {
  if (inline_clock) {
    if (scale < 65) return fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
    if (scale < 85) return fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);
    // This is the largest inline size guaranteed to fit "12:59" on Basalt.
    return fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS);
  }
  if (scale < 60) return fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  if (scale < 75) return fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);
  if (scale < 90) return fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS);
  return fonts_get_system_font(minutes ? FONT_KEY_BITHAM_42_LIGHT : FONT_KEY_BITHAM_42_BOLD);
}

static GFont battery_font(int scale) {
  if (scale < 45) return fonts_get_system_font(FONT_KEY_GOTHIC_14);
  if (scale < 80) return fonts_get_system_font(FONT_KEY_GOTHIC_18);
  return fonts_get_system_font(FONT_KEY_GOTHIC_24);
}

static void render_text(GContext *ctx, const char *text, GFont font,
                        int x, int y, int w, int h, int32_t hex,
                        GTextAlignment align) {
  // Fixed, display-bounded rectangles: no width measurement and no overflow.
  if (!text || !font || w <= 0 || h <= 0) return;
  graphics_context_set_text_color(ctx, color(hex));
  graphics_draw_text(ctx, text, font, GRect(x, y, w, h),
                     GTextOverflowModeTrailingEllipsis, align, NULL);
}

static void update_time(void) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  if (!t) return;
  int h = t->tm_hour % 12;
  if (h == 0) h = 12;
  snprintf(s_hours, sizeof(s_hours), "%d", h);
  snprintf(s_minutes, sizeof(s_minutes), "%02d", t->tm_min);
  snprintf(s_single, sizeof(s_single), "%d:%02d", h, t->tm_min);
  strftime(s_date, sizeof(s_date), "%a %b %d", t);
  for (unsigned int i = 0; s_date[i]; ++i) {
    s_date[i] = (char)toupper((unsigned char)s_date[i]);
  }
  snprintf(s_ampm, sizeof(s_ampm), "%s", t->tm_hour < 12 ? "AM" : "PM");
  if (s_canvas) layer_mark_dirty(s_canvas);
}

static void battery_event(BatteryChargeState state) {
  s_battery = state;
  if (s_canvas) layer_mark_dirty(s_canvas);
}

static void minute_event(struct tm *tick_time, TimeUnits units_changed) {
  (void)tick_time;
  (void)units_changed;
  update_time();
}

static void draw_battery(GContext *ctx, int width, int height) {
  if (!s_settings.battery_enabled ||
      (!s_settings.battery_icon_visible && !s_settings.battery_percent_visible)) return;

  const int icon_scale = clamp_int(s_settings.battery_icon_size, 30, 100);
  const int text_scale = clamp_int(s_settings.battery_text_size, 30, 100);
  const int iw = 7 + icon_scale * 15 / 100;  // 11..22 px
  const int ih = 4 + icon_scale * 10 / 100;  // 7..14 px
  const int x = width - iw - 11;
  const bool top = (s_settings.battery_position == 0);
  const int iy = top ? 9 : height - 47;
  const int pct_y = top ? 28 : height - 29;

  if (s_settings.battery_icon_visible) {
    graphics_context_set_stroke_color(ctx, color(s_settings.battery_icon_color));
    graphics_context_set_fill_color(ctx, color(s_settings.battery_icon_color));
    graphics_draw_rect(ctx, GRect(x, iy, iw - 3, ih));
    graphics_fill_rect(ctx, GRect(x + iw - 3, iy + ih / 3, 3, 3), 0, GCornerNone);
    int fill_w = (iw - 6) * clamp_int(s_battery.charge_percent, 0, 100) / 100;
    if (fill_w > 0 && ih > 4) {
      graphics_fill_rect(ctx, GRect(x + 2, iy + 2, fill_w, ih - 4), 0, GCornerNone);
    }
  }
  if (s_settings.battery_percent_visible) {
    char pct[6];
    snprintf(pct, sizeof(pct), "%d%%", clamp_int(s_battery.charge_percent, 0, 100));
    render_text(ctx, pct, battery_font(text_scale), width - 48, pct_y, 37, 27,
                s_settings.battery_text_color, GTextAlignmentRight);
  }
}

static void canvas_update(Layer *layer, GContext *ctx) {
  const GRect bounds = layer_get_bounds(layer);
  const int width = bounds.size.w;
  const int height = bounds.size.h;
  const int scale = clamp_int(s_settings.clock_size, 50, 100);
  const bool is_single = s_settings.clock_layout == 1;
  const bool top_battery = s_settings.battery_enabled &&
    s_settings.battery_position == 0 &&
    (s_settings.battery_icon_visible || s_settings.battery_percent_visible);

  graphics_context_set_fill_color(ctx, color(s_settings.background_color));
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  if (is_single) {
    // Two fixed slots keep the independently-colored hour/minute groups
    // aligned without allocating or measuring any text during redraws.
    char hours_colon[6];
    snprintf(hours_colon, sizeof(hours_colon), "%s:", s_hours);
    const int y = top_battery ? 46 : 35;
    GFont font = clock_font(scale, false, true);
    render_text(ctx, hours_colon, font, 7, y, 70, 52,
                s_settings.hours_color, GTextAlignmentRight);
    render_text(ctx, s_minutes, font, 77, y, 58, 52,
                s_settings.minutes_color, GTextAlignmentLeft);
    if (s_settings.show_ampm) {
      render_text(ctx, s_ampm, fonts_get_system_font(FONT_KEY_GOTHIC_18),
                  100, y + 47, 33, 23, s_settings.ampm_color, GTextAlignmentRight);
    }
  } else {
    // Two lines. Text remains inside the 144x168 drawable area at 100%.
    GFont big = clock_font(scale, false, false);
    GFont small = clock_font(scale, true, false);
    const int minute_y = scale < 65 ? 57 : 63;
    render_text(ctx, s_hours, big, 11, 6, 110, 60,
                s_settings.hours_color, GTextAlignmentLeft);
    render_text(ctx, s_minutes, small, 11, minute_y, 92, 57,
                s_settings.minutes_color, GTextAlignmentLeft);
    if (s_settings.show_ampm) {
      render_text(ctx, s_ampm, fonts_get_system_font(FONT_KEY_GOTHIC_18),
                  101, minute_y + 27, 33, 23,
                  s_settings.ampm_color, GTextAlignmentLeft);
    }
  }

  graphics_context_set_fill_color(ctx, color(s_settings.accent_color));
  graphics_fill_rect(ctx, GRect(12, height - 44, 29, 3), 0, GCornerNone);
  render_text(ctx, s_date, fonts_get_system_font(FONT_KEY_GOTHIC_14),
              12, height - 35, width - 56, 25,
              s_settings.date_color, GTextAlignmentLeft);
  draw_battery(ctx, width, height);
}

static int32_t tuple_number(const Tuple *tuple);

#define APPLY_INT(field, key, low, high) do { \
  Tuple *t = dict_find(iter, MESSAGE_KEY_##key); \
  if (t) { s_settings.field = clamp_int(tuple_number(t), low, high); changed = true; } \
} while (0)

#define APPLY_COLOR(field, key) do { \
  Tuple *t = dict_find(iter, MESSAGE_KEY_##key); \
  if (t) { s_settings.field = tuple_number(t) & 0xFFFFFF; changed = true; } \
} while (0)

static int32_t tuple_number(const Tuple *tuple) {
  if (tuple->type == TUPLE_CSTRING) {
    const char *raw = tuple->value->cstring;
    if (!raw) return 0;
    if (strcmp(raw, "true") == 0) return 1;
    if (strcmp(raw, "false") == 0) return 0;
    // Normally Clay sends colors as int32; tolerate #RRGGBB strings too.
    if (*raw == '#') return (int32_t)strtoul(raw + 1, NULL, 16);
    return (int32_t)strtol(raw, NULL, 0);
  }
  if (tuple->type == TUPLE_UINT) return (int32_t)tuple->value->uint32;
  return tuple->value->int32;
}

static void message_received(DictionaryIterator *iter, void *context) {
  (void)context;
  bool changed = false;
  APPLY_INT(clock_layout, ClockLayout, 0, 1);
  APPLY_INT(clock_size, ClockSize, 50, 100);
  APPLY_INT(show_ampm, ShowAmPm, 0, 1);
  APPLY_INT(battery_enabled, BatteryEnabled, 0, 1);
  APPLY_INT(battery_position, BatteryPosition, 0, 1);
  APPLY_INT(battery_icon_visible, BatteryIconVisible, 0, 1);
  APPLY_INT(battery_percent_visible, BatteryPercentVisible, 0, 1);
  APPLY_INT(battery_icon_size, BatteryIconSize, 30, 100);
  APPLY_INT(battery_text_size, BatteryTextSize, 30, 100);
  APPLY_COLOR(background_color, BackgroundColor);
  APPLY_COLOR(hours_color, HoursColor);
  APPLY_COLOR(minutes_color, MinutesColor);
  APPLY_COLOR(ampm_color, AmPmColor);
  APPLY_COLOR(accent_color, AccentColor);
  APPLY_COLOR(date_color, DateColor);
  APPLY_COLOR(battery_icon_color, BatteryIconColor);
  APPLY_COLOR(battery_text_color, BatteryTextColor);
  if (changed) {
    s_settings.version = SETTINGS_VERSION;
    int written = persist_write_data(SETTINGS_KEY, &s_settings, sizeof(s_settings));
    if (written != (int)sizeof(s_settings)) {
      APP_LOG(APP_LOG_LEVEL_WARNING, "Settings persistence failed: %d", written);
    }
    if (s_canvas) layer_mark_dirty(s_canvas);
    APP_LOG(APP_LOG_LEVEL_INFO, "Applied settings: layout=%ld size=%ld battery=%ld position=%ld", 
            (long)s_settings.clock_layout, (long)s_settings.clock_size,
            (long)s_settings.battery_enabled, (long)s_settings.battery_position);
  }
}

static void message_dropped(AppMessageResult reason, void *context) {
  (void)context;
  APP_LOG(APP_LOG_LEVEL_WARNING, "Settings AppMessage dropped: %d", (int)reason);
}

static void window_load(Window *window) {
  Layer *root = window_get_root_layer(window);
  s_canvas = layer_create(layer_get_bounds(root));
  layer_set_update_proc(s_canvas, canvas_update);
  layer_add_child(root, s_canvas);
  update_time();
}

static void window_unload(Window *window) {
  (void)window;
  layer_destroy(s_canvas);
  s_canvas = NULL;
}

static void init(void) {
  load_settings();
  s_battery = battery_state_service_peek();
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
  window_stack_push(s_window, true);
  tick_timer_service_subscribe(MINUTE_UNIT, minute_event);
  battery_state_service_subscribe(battery_event);
  app_message_register_inbox_received(message_received);
  app_message_register_inbox_dropped(message_dropped);
  // Clay sends the complete 17-key dictionary (~200-300 bytes),
  // so 512 bytes leaves headroom without allocating an oversized buffer.
  AppMessageResult message_status = app_message_open(512, 128);
  if (message_status != APP_MSG_OK) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "AppMessage initialization failed: %d", (int)message_status);
  }
  update_time();
}

static void deinit(void) {
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  app_message_deregister_callbacks();
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
  return 0;
}
