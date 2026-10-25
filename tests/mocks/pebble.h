/* Minimal host-side Pebble API shim for deterministic layout/settings tests.
   Not included in the actual SDK watchface build. */
#ifndef TEST_PEBBLE_H
#define TEST_PEBBLE_H
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

typedef struct Layer { int dummy; } Layer;
typedef struct Window { int dummy; } Window;
typedef struct GContext { int dummy; } GContext;
typedef const char *GFont;
typedef unsigned int GColor;
typedef struct {int16_t x,y;} GPoint;
typedef struct {int16_t w,h;} GSize;
typedef struct {GPoint origin;GSize size;} GRect;
#define GRect(x,y,w,h) ((GRect){ .origin={(x),(y)}, .size={(w),(h)} })
typedef enum {GTextAlignmentLeft=0,GTextAlignmentCenter=1,GTextAlignmentRight=2} GTextAlignment;
typedef enum {GTextOverflowModeTrailingEllipsis=0} GTextOverflowMode;
typedef enum {GCornerNone=0} GCornerMask;
#define FONT_KEY_BITHAM_30_BLACK "BITHAM30"
#define FONT_KEY_BITHAM_34_MEDIUM_NUMBERS "BITHAM34"
#define FONT_KEY_BITHAM_42_BOLD "BITHAM42B"
#define FONT_KEY_BITHAM_42_LIGHT "BITHAM42L"
#define FONT_KEY_LECO_28_LIGHT_NUMBERS "LECO28"
#define FONT_KEY_GOTHIC_24_BOLD "GOTHIC24B"
#define FONT_KEY_GOTHIC_24 "GOTHIC24"
#define FONT_KEY_GOTHIC_18 "GOTHIC18"
#define FONT_KEY_GOTHIC_14 "GOTHIC14"
typedef struct {int charge_percent;bool is_charging;bool is_plugged;} BatteryChargeState;
typedef enum {TUPLE_BYTE_ARRAY=0,TUPLE_CSTRING=1,TUPLE_UINT=2,TUPLE_INT=3} TupleType;
typedef union {uint8_t data[32];char cstring[32];int32_t int32;uint32_t uint32;} TupleValue;
typedef struct {uint32_t key;TupleType type;uint16_t length;TupleValue *value;} Tuple;
typedef struct {Tuple *tuples;int count;int cursor;} DictionaryIterator;
typedef enum {APP_MSG_OK=0} AppMessageResult;
typedef enum {APP_LOG_LEVEL_ERROR,APP_LOG_LEVEL_WARNING,APP_LOG_LEVEL_INFO} AppLogLevel;
#define APP_LOG(level,fmt,...) ((void)0)
enum {
  MESSAGE_KEY_ClockLayout, MESSAGE_KEY_ClockSize, MESSAGE_KEY_ShowAmPm,
  MESSAGE_KEY_BatteryEnabled, MESSAGE_KEY_BatteryPosition,
  MESSAGE_KEY_BatteryIconVisible, MESSAGE_KEY_BatteryPercentVisible,
  MESSAGE_KEY_BatteryIconSize, MESSAGE_KEY_BatteryTextSize,
  MESSAGE_KEY_BackgroundColor, MESSAGE_KEY_HoursColor,
  MESSAGE_KEY_MinutesColor, MESSAGE_KEY_AmPmColor, MESSAGE_KEY_AccentColor,
  MESSAGE_KEY_DateColor, MESSAGE_KEY_BatteryIconColor,
  MESSAGE_KEY_BatteryTextColor,
  MESSAGE_KEY_AccentWidth, MESSAGE_KEY_WeekdaySize, MESSAGE_KEY_DateSize, MESSAGE_KEY_MirrorLayout
};
typedef struct {void (*load)(Window *);void (*unload)(Window *);} WindowHandlers;
#define MINUTE_UNIT 1
typedef int TimeUnits;
extern int fake_dirty_count;
extern int fake_child_count;
extern int fake_draw_count;
extern int fake_bad_draw;
extern int fake_save_count;
GColor GColorFromHEX(uint32_t hex);
bool gcolor_equal(GColor a, GColor b);
GFont fonts_get_system_font(const char *key);
GSize graphics_text_layout_get_content_size(const char *text,GFont font,GRect rect,GTextOverflowMode overflow,GTextAlignment align);
void graphics_context_set_text_color(GContext *ctx,GColor c);
void graphics_context_set_fill_color(GContext *ctx,GColor c);
void graphics_context_set_stroke_color(GContext *ctx,GColor c);
void graphics_draw_text(GContext *ctx,const char *text,GFont font,GRect rect,GTextOverflowMode overflow,GTextAlignment align,void *attrs);
void graphics_fill_rect(GContext *ctx,GRect rect,int radius,GCornerMask corners);
void graphics_draw_rect(GContext *ctx,GRect rect);
GRect layer_get_bounds(Layer *l);
void layer_mark_dirty(Layer *l);
Layer *layer_create(GRect r);
void layer_set_update_proc(Layer *l,void (*proc)(Layer*,GContext*));
void layer_add_child(Layer *root,Layer *child);
void layer_destroy(Layer *l);
Layer *window_get_root_layer(Window *w);
Window *window_create(void);
void window_set_window_handlers(Window *w,WindowHandlers handlers);
void window_stack_push(Window *w,bool animated);
void window_destroy(Window *w);
BatteryChargeState battery_state_service_peek(void);
void tick_timer_service_subscribe(TimeUnits units,void (*callback)(struct tm*,TimeUnits));
void tick_timer_service_unsubscribe(void);
void battery_state_service_subscribe(void (*callback)(BatteryChargeState));
void battery_state_service_unsubscribe(void);
void app_message_register_inbox_received(void (*handler)(DictionaryIterator*,void*));
void app_message_register_inbox_dropped(void (*handler)(AppMessageResult,void*));
AppMessageResult app_message_open(uint32_t inbound,uint32_t outbound);
void app_message_deregister_callbacks(void);
Tuple *dict_read_first(DictionaryIterator *iter);
Tuple *dict_read_next(DictionaryIterator *iter);
bool persist_exists(const uint32_t key);
int persist_get_size(const uint32_t key);
int persist_read_data(const uint32_t key,void *buffer,size_t size);
int persist_write_data(const uint32_t key,const void *buffer,size_t size);
void app_event_loop(void);
#endif
