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
    }
  }
}

static GColor color(int32_t hex) { return GColorFromHEX((uint32_t)hex); }

// Use built-in numeric fonts so no copyrighted font files need packaging.
// The scale setting maps to compatible discrete native font sizes.
static GFont hours_font(int scale) {
  if (scale < 57) return fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);
  if (scale < 68) return fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS);
  if (scale < 88) return fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD);
  return fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49);
}

static GFont minutes_font(int scale) {
  if (scale < 57) return fonts_get_system_font(FONT_KEY_LECO_28_LIGHT_NUMBERS);
  if (scale < 68) return fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS);
  return fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT);
}

static GFont inline_font(int scale) {
  if (scale < 60) return fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  if (scale < 80) return fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);
  if (scale < 95) return fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS);
  return fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD);
}

static GFont battery_font(int scale) {
  if (scale < 45) return fonts_get_system_font(FONT_KEY_GOTHIC_14);
  if (scale < 75) return fonts_get_system_font(FONT_KEY_GOTHIC_18);
  return fonts_get_system_font(FONT_KEY_GOTHIC_24);
}

static int measure_width(const char *text, GFont font) {
  GSize size = graphics_text_layout_get_content_size(text, font, GRect(0, 0, 200, 65),
                    GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft);
  return size.w;
}

static void render_text(GContext *ctx, const char *text, GFont font,
                        int x, int y, int w, int h, int32_t hex,
                        GTextAlignment align) {
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

  // Right-aligned group. Icon and percentage each have a separate size control.
  const int icon_scale = clamp_int(s_settings.battery_icon_size, 30, 100);
  const int text_scale = clamp_int(s_settings.battery_text_size, 30, 100);
  const int iw = 8 + (icon_scale * 17 / 100);   // 13..25 px
  const int ih = 5 + (icon_scale * 10 / 100);   // 8..15 px
  const int x = width - iw - 12;
  const int top = (s_settings.battery_position == 0);
  const int iy = top ? 11 : height - 48;
  const int text_y = top ? 27 : height - 29;

  if (s_settings.battery_icon_visible) {
    graphics_context_set_stroke_color(ctx, color(s_settings.battery_icon_color));
    graphics_context_set_fill_color(ctx, color(s_settings.battery_icon_color));
    // Body, positive terminal, and proportional interior charge fill.
    graphics_draw_rect(ctx, GRect(x, iy, iw - 3, ih));
    graphics_fill_rect(ctx, GRect(x + iw - 3, iy + ih / 3, 3, ih / 3 + 1), 0, GCornerNone);
    int fill_w = (iw - 6) * clamp_int(s_battery.charge_percent, 0, 100) / 100;
    if (fill_w > 0 && ih > 4) {
      graphics_fill_rect(ctx, GRect(x + 2, iy + 2, fill_w, ih - 4), 0, GCornerNone);
    }
  }
  if (s_settings.battery_percent_visible) {
    char pct[6];
    snprintf(pct, sizeof(pct), "%d%%", clamp_int(s_battery.charge_percent, 0, 100));
    render_text(ctx, pct, battery_font(text_scale),
                width - 49, text_y, 37, 26,
                s_settings.battery_text_color, GTextAlignmentRight);
  }
}

static void canvas_update(Layer *layer, GContext *ctx) {
  const GRect bounds = layer_get_bounds(layer);
  const int width = bounds.size.w;
  const int height = bounds.size.h;
  const int scale = clamp_int(s_settings.clock_size, 50, 100);
  const bool is_single = s_settings.clock_layout == 1;
  const bool upper_battery = s_settings.battery_enabled &&
    s_settings.battery_position == 0 &&
    (s_settings.battery_icon_visible || s_settings.battery_percent_visible);

  graphics_context_set_fill_color(ctx, color(s_settings.background_color));
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  // Reserve the bottom band for the date, accent, and optional battery.
  // Single-line clock is lowered when the battery occupies top-right.
  if (is_single) {
    int clock_y = upper_battery ? 48 : 28;
    int font_scale = scale;
    GFont font = inline_font(font_scale);
    char hour_colon[6];
    snprintf(hour_colon, sizeof(hour_colon), "%s:", s_hours);
    int first_w = measure_width(hour_colon, font);
    int second_w = measure_width(s_minutes, font);
    // Try smaller native type sizes when long strings would touch the bezel.
    while (first_w + second_w > width - 22 && font_scale > 50) {
      font_scale -= 10;
      font = inline_font(font_scale);
      first_w = measure_width(hour_colon, font);
      second_w = measure_width(s_minutes, font);
    }
    int x = (width - first_w - second_w) / 2;
    render_text(ctx, hour_colon, font, x, clock_y,
                first_w + 3, 60, s_settings.hours_color, GTextAlignmentLeft);
    render_text(ctx, s_minutes, font, x + first_w, clock_y,
                second_w + 3, 60, s_settings.minutes_color, GTextAlignmentLeft);
    if (s_settings.show_ampm) {
      render_text(ctx, s_ampm, fonts_get_system_font(FONT_KEY_GOTHIC_18),
                  width - 48, clock_y + 46, 37, 24,
                  s_settings.ampm_color, GTextAlignmentRight);
    }
  } else {
    // In stacked mode, hours and minutes are independently colorable.
    // Right-side battery may sit beside the hours without overlapping them.
    int hours_y = 8;
    int minutes_y = scale < 68 ? 54 : 62;
    render_text(ctx, s_hours, hours_font(scale), 11, hours_y,
                width - 37, 62, s_settings.hours_color, GTextAlignmentLeft);
    render_text(ctx, s_minutes, minutes_font(scale), 11, minutes_y,
                width - 56, 59, s_settings.minutes_color, GTextAlignmentLeft);
    if (s_settings.show_ampm) {
      render_text(ctx, s_ampm, fonts_get_system_font(FONT_KEY_GOTHIC_18),
                  width - 56, minutes_y + 26, 44, 24,
                  s_settings.ampm_color, GTextAlignmentLeft);
    }
  }

  graphics_context_set_fill_color(ctx, color(s_settings.accent_color));
  graphics_fill_rect(ctx, GRect(12, height - 44, 29, 3), 0, GCornerNone);
  render_text(ctx, s_date, fonts_get_system_font(FONT_KEY_GOTHIC_14),
              12, height - 35, width - 56, 26,
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
    return (int32_t)strtol(tuple->value->cstring, NULL, 0);
  }
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
    persist_write_data(SETTINGS_KEY, &s_settings, sizeof(s_settings));
    layer_mark_dirty(s_canvas);
  }
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
  app_message_open(512, 128);
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
}
