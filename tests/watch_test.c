/* Build: cc -std=c11 -Itests/mocks -Wall -Wextra -o /tmp/test tests/watch_test.c */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pebble.h"

int fake_dirty_count=0, fake_child_count=0, fake_draw_count=0, fake_bad_draw=0, fake_save_count=0;
static char stored[256];
static size_t stored_size=0;
static Layer root_layer;
static GContext fake_context;
static Window fake_window;
static Layer fake_canvas;

GColor GColorFromHEX(uint32_t h){return h;}
GFont fonts_get_system_font(const char *k){return k;}
GSize graphics_text_layout_get_content_size(const char *text,GFont f,GRect rect,GTextOverflowMode o,GTextAlignment a){
  (void)rect;(void)o;(void)a;
  int advance=9;
  if (strstr(f,"42")) advance=17;
  else if (strstr(f,"34")) advance=13;
  else if (strstr(f,"30")) advance=12;
  else if (strstr(f,"24")) advance=10;
  return (GSize){.w=(int16_t)(strlen(text)*advance),.h=55};
}
void graphics_context_set_text_color(GContext*c,GColor col){(void)c;(void)col;}
void graphics_context_set_fill_color(GContext*c,GColor col){(void)c;(void)col;}
void graphics_context_set_stroke_color(GContext*c,GColor col){(void)c;(void)col;}
static void check_rect(GRect r) {
  if(r.size.w<=0||r.size.h<=0||r.origin.x<0||r.origin.x+r.size.w>144||r.origin.y<0||r.origin.y+r.size.h>168)fake_bad_draw++;
}
void graphics_draw_text(GContext*c,const char*t,GFont f,GRect r,GTextOverflowMode o,GTextAlignment a,void*attrs){
  (void)c;(void)t;(void)f;(void)o;(void)a;(void)attrs;
  fake_draw_count++;check_rect(r);
}
void graphics_fill_rect(GContext*c,GRect r,int radius,GCornerMask co){(void)c;(void)radius;(void)co;check_rect(r);}
void graphics_draw_rect(GContext*c,GRect r){(void)c;check_rect(r);}
GRect layer_get_bounds(Layer*l){(void)l;return GRect(0,0,144,168);}
void layer_mark_dirty(Layer*l){assert(l);fake_dirty_count++;}
Layer*layer_create(GRect r){(void)r;return &fake_canvas;}
void layer_set_update_proc(Layer*l,void(*p)(Layer*,GContext*)){(void)l;(void)p;}
void layer_add_child(Layer*parent,Layer*child){(void)parent;assert(child);fake_child_count++;}
void layer_destroy(Layer*l){(void)l;}
Layer*window_get_root_layer(Window*w){(void)w;return &root_layer;}
Window*window_create(void){return &fake_window;}
void window_set_window_handlers(Window*w,WindowHandlers h){(void)w;(void)h;}
void window_stack_push(Window*w,bool b){(void)w;(void)b;}
void window_destroy(Window*w){(void)w;}
BatteryChargeState battery_state_service_peek(void){return (BatteryChargeState){80,false,false};}
void tick_timer_service_subscribe(TimeUnits u,void(*p)(struct tm*,TimeUnits)){(void)u;(void)p;}
void tick_timer_service_unsubscribe(void){}
void battery_state_service_subscribe(void(*p)(BatteryChargeState)){(void)p;}
void battery_state_service_unsubscribe(void){}
void app_message_register_inbox_received(void(*p)(DictionaryIterator*,void*)){(void)p;}
void app_message_register_inbox_dropped(void(*p)(AppMessageResult,void*)){(void)p;}
AppMessageResult app_message_open(uint32_t a,uint32_t b){assert(a>=128&&b>=64);return APP_MSG_OK;}
void app_message_deregister_callbacks(void){}
Tuple*dict_find(DictionaryIterator*it,uint32_t key){
  for(int i=0;i<it->count;i++)if(it->tuples[i].key==key)return &it->tuples[i];
  return NULL;
}
bool persist_exists(const uint32_t key){(void)key;return stored_size>0;}
int persist_get_size(const uint32_t key){(void)key;return stored_size;}
int persist_read_data(const uint32_t key,void*b,size_t n){(void)key;if(n!=stored_size)return -1;memcpy(b,stored,n);return n;}
int persist_write_data(const uint32_t key,const void*b,size_t n){(void)key;assert(n<=sizeof(stored));memcpy(stored,b,n);stored_size=n;fake_save_count++;return n;}
void app_event_loop(void){}

#define main watchface_original_main
#include "../src/c/main.c"
#undef main

static void apply_num(uint32_t key,int32_t num){
  TupleValue val={.int32=num};
  Tuple t={.key=key,.type=TUPLE_INT,.value=&val};
  DictionaryIterator it={.tuples=&t,.count=1};
  message_received(&it,NULL);
}
static void apply_string(uint32_t key,char *val){
  TupleValue v={.cstring=val};
  Tuple t={.key=key,.type=TUPLE_CSTRING,.value=&v};
  DictionaryIterator it={.tuples=&t,.count=1};
  message_received(&it,NULL);
}

int main(void){
  defaults();
  window_load(&fake_window);
  assert(fake_child_count==1); // regression: never add same Layer twice
  s_battery.charge_percent=80;
  strcpy(s_hours,"12");strcpy(s_minutes,"59");strcpy(s_ampm,"PM");
  strcpy(s_date,"MON OCT 25");
  apply_num(MESSAGE_KEY_ClockSize,100);
  assert(s_settings.clock_size==100);
  canvas_update(s_canvas,&fake_context);
  assert(fake_bad_draw==0);
  apply_num(MESSAGE_KEY_ClockLayout,1);
  canvas_update(s_canvas,&fake_context);
  assert(fake_bad_draw==0);
  apply_num(MESSAGE_KEY_BatteryPosition,0);
  canvas_update(s_canvas,&fake_context);
  assert(fake_bad_draw==0);
  apply_num(MESSAGE_KEY_BatteryEnabled,0);
  assert(s_settings.battery_enabled==0);
  apply_num(MESSAGE_KEY_BatteryEnabled,1);
  apply_num(MESSAGE_KEY_BatteryIconVisible,0);
  apply_num(MESSAGE_KEY_BatteryPercentVisible,0);
  assert(!s_settings.battery_icon_visible&&!s_settings.battery_percent_visible);
  apply_num(MESSAGE_KEY_BatteryIconSize,30);
  apply_num(MESSAGE_KEY_BatteryTextSize,100);
  apply_num(MESSAGE_KEY_ShowAmPm,0);
  assert(s_settings.battery_icon_size==30);
  assert(s_settings.battery_text_size==100);
  assert(s_settings.show_ampm==0);
  apply_string(MESSAGE_KEY_BackgroundColor,"#55FFAA");
  assert(s_settings.background_color==0x55FFAA);
  apply_num(MESSAGE_KEY_HoursColor,0xFFFFFF);
  apply_num(MESSAGE_KEY_MinutesColor,0x000000);
  apply_num(MESSAGE_KEY_AmPmColor,0x55FFAA);
  apply_num(MESSAGE_KEY_AccentColor,0xAA0000);
  apply_num(MESSAGE_KEY_DateColor,0x00FF00);
  apply_num(MESSAGE_KEY_BatteryIconColor,0x00AAFF);
  apply_num(MESSAGE_KEY_BatteryTextColor,0xAAAAAA);
  assert(s_settings.battery_icon_color==0x00AAFF);
  FaceSettings previous=s_settings;
  defaults();load_settings();
  assert(memcmp(&previous,&s_settings,sizeof(previous))==0);
  // Clamp malicious/stale 1000% settings to supported range.
  apply_num(MESSAGE_KEY_ClockSize,1000);
  assert(s_settings.clock_size==100);
  apply_num(MESSAGE_KEY_ClockSize,-10);
  assert(s_settings.clock_size==50);
  assert(fake_save_count>=18);
  printf("PASS: layout draws at 100%%, receives every control, persists settings, clamps sizes\n");
  return 0;
}
