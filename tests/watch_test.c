/* Build: cc -std=c11 -Itests/mocks -Wall -Wextra -Werror -Wno-unused-parameter -o /tmp/test tests/watch_test.c */
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
static GColor current_text_color;
static int battery_rect_count=0, stroke_count=0;
static const char *first_time_font;
static int hours_y=-1;
static bool drawn_hours=false, drawn_minutes=false, drawn_ampm=false, drawn_date=false, drawn_pct=false;
static GColor hours_col=0, minutes_col=0, battery_text_col=0;

GColor GColorFromHEX(uint32_t h){return h;}
GFont fonts_get_system_font(const char *k){return k;}
GSize graphics_text_layout_get_content_size(const char *text,GFont f,GRect rect,GTextOverflowMode o,GTextAlignment a){
  (void)text;(void)f;(void)rect;(void)o;(void)a;
  assert(!"No text width measurement allowed in draw callback");
  return (GSize){.w=0,.h=0};
}
void graphics_context_set_text_color(GContext*c,GColor col){(void)c;current_text_color=col;}
void graphics_context_set_fill_color(GContext*c,GColor col){(void)c;(void)col;}
void graphics_context_set_stroke_color(GContext*c,GColor col){(void)c;(void)col;stroke_count++;}
static void check_rect(GRect r) {
  if(r.size.w<=0||r.size.h<=0||r.origin.x<0||r.origin.x+r.size.w>144||r.origin.y<0||r.origin.y+r.size.h>168)fake_bad_draw++;
}
void graphics_draw_text(GContext*c,const char*t,GFont f,GRect r,GTextOverflowMode o,GTextAlignment a,void*attrs){
  (void)c;(void)o;(void)a;(void)attrs;
  fake_draw_count++;check_rect(r);
  if(strcmp(t,"12")==0 || strcmp(t,"12:")==0) {
    if(!drawn_hours) { drawn_hours=true;first_time_font=f;hours_y=r.origin.y;hours_col=current_text_color; }
  }
  if(strcmp(t,"59")==0) { drawn_minutes=true;minutes_col=current_text_color; }
  if(strcmp(t,"PM")==0) drawn_ampm=true;
  if(strcmp(t,"80%")==0) {drawn_pct=true;battery_text_col=current_text_color;}
  if(strcmp(t,"MON OCT 25")==0) drawn_date=true;
}
void graphics_fill_rect(GContext*c,GRect r,int radius,GCornerMask co){(void)c;(void)radius;(void)co;check_rect(r);}
void graphics_draw_rect(GContext*c,GRect r){(void)c;battery_rect_count++;check_rect(r);}
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
AppMessageResult app_message_open(uint32_t a,uint32_t b){assert(a>=512&&b>=64);return APP_MSG_OK;}
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
static void reset_draw(void) {
  drawn_hours=false;drawn_minutes=false;drawn_ampm=false;drawn_date=false;drawn_pct=false;
  first_time_font=NULL;hours_y=-1;stroke_count=0;battery_rect_count=0;fake_bad_draw=0;
}
static void render_checked(void) {
  reset_draw();
  canvas_update(s_canvas,&fake_context);
  assert(fake_bad_draw==0);
  assert(drawn_hours && drawn_minutes && drawn_date);
}

int main(void){
  defaults();
  window_load(&fake_window);
  assert(fake_child_count==1);
  s_battery.charge_percent=80;
  strcpy(s_hours,"12");strcpy(s_minutes,"59");strcpy(s_ampm,"PM");
  strcpy(s_date,"MON OCT 25");

  // A real Clay save sends all controls as one AppMessage. Regression test
  // receives the full dictionary, applies it, marks the layer dirty, persists.
  uint32_t keys[] = { MESSAGE_KEY_ClockLayout, MESSAGE_KEY_ClockSize,
    MESSAGE_KEY_ShowAmPm, MESSAGE_KEY_BatteryEnabled,
    MESSAGE_KEY_BatteryPosition, MESSAGE_KEY_BatteryIconVisible,
    MESSAGE_KEY_BatteryPercentVisible, MESSAGE_KEY_BatteryIconSize,
    MESSAGE_KEY_BatteryTextSize, MESSAGE_KEY_BackgroundColor,
    MESSAGE_KEY_HoursColor, MESSAGE_KEY_MinutesColor,
    MESSAGE_KEY_AmPmColor, MESSAGE_KEY_AccentColor,
    MESSAGE_KEY_DateColor, MESSAGE_KEY_BatteryIconColor,
    MESSAGE_KEY_BatteryTextColor };
  int32_t vals[] = { 1, 100, 1, 1, 0, 1, 1, 30, 50, 0,
    0xFF0000, 0x55FFAA, 0xAAAAAA, 0x55FFAA, 0xFFFFFF,
    0x55FFAA, 0x00FF00 };
  TupleValue values[17];Tuple tuples[17];
  for(int i=0;i<17;i++) { values[i].int32=vals[i]; tuples[i]=(Tuple){.key=keys[i],.type=TUPLE_INT,.value=&values[i]}; }
  DictionaryIterator dict={.tuples=tuples,.count=17};
  int dirty_before=fake_dirty_count;
  message_received(&dict,NULL);
  assert(fake_save_count==1 && fake_dirty_count==dirty_before+1);
  assert(s_settings.clock_layout==1 && s_settings.clock_size==100 && s_settings.battery_position==0);
  assert(s_settings.battery_text_color==0x00FF00);
  render_checked();
  assert(hours_y==46); // Top right battery forces clock down in single-line mode.
  assert(hours_col==0xFF0000 && minutes_col==0x55FFAA && battery_text_col==0x00FF00);
  assert(drawn_pct && battery_rect_count==1);
  assert(first_time_font && strstr(first_time_font,"34"));

  apply_num(MESSAGE_KEY_ClockLayout,0);
  apply_num(MESSAGE_KEY_BatteryPosition,1);
  render_checked();
  assert(hours_y==6);
  apply_num(MESSAGE_KEY_ClockSize,50);
  render_checked();
  assert(strstr(first_time_font,"24"));
  apply_num(MESSAGE_KEY_ClockSize,100);
  for(int i=0;i<1000;i++) render_checked();
  assert(strstr(first_time_font,"42"));

  apply_num(MESSAGE_KEY_BatteryEnabled,0);
  render_checked();
  assert(!drawn_pct && battery_rect_count==0);
  apply_num(MESSAGE_KEY_BatteryEnabled,1);
  apply_num(MESSAGE_KEY_BatteryIconVisible,0);
  render_checked();
  assert(battery_rect_count==0 && drawn_pct);
  apply_num(MESSAGE_KEY_BatteryPercentVisible,0);
  render_checked();
  assert(!drawn_pct && battery_rect_count==0);
  apply_num(MESSAGE_KEY_BatteryIconVisible,1);
  render_checked();
  assert(battery_rect_count==1 && !drawn_pct);

  FaceSettings saved=s_settings;
  defaults();load_settings();
  assert(memcmp(&saved,&s_settings,sizeof(saved))==0);
  apply_num(MESSAGE_KEY_ClockSize,1000);
  assert(s_settings.clock_size==100);
  apply_num(MESSAGE_KEY_ClockSize,-10);
  assert(s_settings.clock_size==50);
  assert(fake_save_count>8);
  puts("PASS: 17-key AppMessage received/redrawn/saved; clock layout+size, colors, battery toggles; 1000 max-size redraws in bounds");
  return 0;
}
