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
static GColor current_text_color, current_fill_color;
static int hour_rect_count, minute_rect_count, hour_top, hour_bottom, minute_top, minute_bottom;
static int hour_left, hour_right, minute_left, minute_right;
static int icon_bottom, icon_right, icon_left, percent_y, percent_right, percent_left;
static int accent_x, accent_w;
static int date_pixels, date_visible_min_x, date_visible_max_x;
static GFont percent_font;
static int battery_rect_count=0, stroke_count=0;
static const char *first_time_font;
static int hours_y=-1;
static bool drawn_hours=false, drawn_minutes=false, drawn_ampm=false, drawn_date=false, drawn_pct=false;
static GColor hours_col=0, minutes_col=0, battery_text_col=0;

GColor GColorFromHEX(uint32_t h){return h;}
bool gcolor_equal(GColor a,GColor b){return a==b;}
GFont fonts_get_system_font(const char *k){return k;}
GSize graphics_text_layout_get_content_size(const char *text,GFont f,GRect rect,GTextOverflowMode o,GTextAlignment a){
  (void)text;(void)f;(void)rect;(void)o;(void)a;
  assert(!"No text width measurement allowed in draw callback");
  return (GSize){.w=0,.h=0};
}
void graphics_context_set_text_color(GContext*c,GColor col){(void)c;current_text_color=col;}
void graphics_context_set_fill_color(GContext*c,GColor col){(void)c;current_fill_color=col;}
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
  if(strcmp(t,"80%")==0) {drawn_pct=true;battery_text_col=current_text_color;percent_y=r.origin.y;percent_left=r.origin.x;percent_right=r.origin.x+r.size.w;percent_font=f;}

}
void graphics_fill_rect(GContext*c,GRect r,int radius,GCornerMask co){
  (void)c;(void)radius;(void)co;check_rect(r);
  if (r.origin.y==124 && r.size.h==3 && current_fill_color==0x55FFAA) {
    accent_x=r.origin.x;
    accent_w=r.size.w;
  }
  if(r.origin.y>=132 && r.size.h==1 && current_fill_color==0xFFFFFF) {
    date_pixels++;
    drawn_date=true;
    if(r.origin.x<date_visible_min_x)date_visible_min_x=r.origin.x;
    if(r.origin.x+r.size.w>date_visible_max_x)date_visible_max_x=r.origin.x+r.size.w;
  }
  if (r.origin.y < 120) {
    if(current_fill_color == 0xFF0000) {
      hour_rect_count++;
      if(r.origin.x<hour_left)hour_left=r.origin.x;
      if(r.origin.x+r.size.w>hour_right)hour_right=r.origin.x+r.size.w;
      if(r.origin.y<hour_top)hour_top=r.origin.y;
      if(r.origin.y+r.size.h>hour_bottom)hour_bottom=r.origin.y+r.size.h;
    } else if(current_fill_color == 0x55FFAA) {
      minute_rect_count++;
      if(r.origin.x<minute_left)minute_left=r.origin.x;
      if(r.origin.x+r.size.w>minute_right)minute_right=r.origin.x+r.size.w;
      if(r.origin.y<minute_top)minute_top=r.origin.y;
      if(r.origin.y+r.size.h>minute_bottom)minute_bottom=r.origin.y+r.size.h;
    }
  }
}
void graphics_draw_rect(GContext*c,GRect r){(void)c;battery_rect_count++;check_rect(r);icon_bottom=r.origin.y+r.size.h;icon_right=r.origin.x+r.size.w+3;icon_left=r.origin.x;}
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
Tuple *dict_read_first(DictionaryIterator *it){ it->cursor=0;return it->count ? &it->tuples[0] : NULL; }
Tuple *dict_read_next(DictionaryIterator *it){++it->cursor;return it->cursor<it->count ? &it->tuples[it->cursor] : NULL;}
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
  Tuple t={.key=key,.type=TUPLE_INT,.length=4,.value=&val};
  DictionaryIterator it={.tuples=&t,.count=1};
  message_received(&it,NULL);
}
static void reset_draw(void) {
  drawn_hours=false;drawn_minutes=false;drawn_ampm=false;drawn_date=false;drawn_pct=false;
  first_time_font=NULL;hours_y=-1;stroke_count=0;battery_rect_count=0;fake_bad_draw=0;
  hour_rect_count=minute_rect_count=0;
  hour_top=minute_top=999;hour_bottom=minute_bottom=0;
  hour_left=minute_left=999;hour_right=minute_right=0;
  icon_bottom=-1;icon_right=-1;icon_left=-1;percent_y=-1;percent_right=-1;percent_left=-1;percent_font=NULL;
  accent_x=-1;accent_w=-1;date_pixels=0;date_visible_min_x=999;date_visible_max_x=-1;
}
static void render_checked(void) {
  reset_draw();
  canvas_update(s_canvas,&fake_context);
  assert(fake_bad_draw==0);
  assert(hour_rect_count > 0 && minute_rect_count > 0 && drawn_date);
}

int main(void){
  defaults();
  window_load(&fake_window);
  assert(fake_child_count==1);
  s_battery.charge_percent=80;
  strcpy(s_hours,"12");strcpy(s_minutes,"59");strcpy(s_ampm,"PM");
  strcpy(s_weekday,"MON");strcpy(s_monthday,"OCT 25");

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
    MESSAGE_KEY_BatteryTextColor, MESSAGE_KEY_AccentWidth,
    MESSAGE_KEY_WeekdaySize, MESSAGE_KEY_DateSize, MESSAGE_KEY_MirrorLayout };
  int32_t vals[] = { 1, 100, 1, 1, 0, 1, 1, 30, 50, 0,
    0xFF0000, 0x55FFAA, 0xAAAAAA, 0x55FFAA, 0xFFFFFF,
    0x55FFAA, 0x00FF00, 29, 14, 14, 0 };
  TupleValue values[21];Tuple tuples[21];
  for(int i=0;i<21;i++) { values[i].int32=vals[i]; tuples[i]=(Tuple){.key=keys[i],.type=TUPLE_INT,.length=4,.value=&values[i]}; }
  DictionaryIterator dict={.tuples=tuples,.count=21};
  int dirty_before=fake_dirty_count;
  message_received(&dict,NULL);
  assert(fake_save_count==1 && fake_dirty_count==dirty_before+1);
  assert(s_settings.clock_layout==1 && s_settings.clock_size==100 && s_settings.battery_position==0);
  assert(s_settings.battery_text_color==0x00FF00);
  assert(s_settings.accent_width==29 && s_settings.weekday_size==14);
  assert(s_settings.date_size==14 && s_settings.mirror_layout==0);
  render_checked();
  assert(hour_top >= 45); // Top-right battery moves the inline clock down.
  assert(battery_text_col==0x00FF00);
  assert(drawn_pct && battery_rect_count==1);

  apply_num(MESSAGE_KEY_ClockLayout,0);
  apply_num(MESSAGE_KEY_BatteryPosition,1);
  render_checked();
  assert(minute_top-hour_bottom <= 3 && minute_top >= hour_bottom);
  assert(abs((hour_right-hour_left)-(minute_right-minute_left))<=2);
  apply_num(MESSAGE_KEY_ClockSize,50);
  render_checked();
  const int small_span=minute_bottom-hour_top;
  apply_num(MESSAGE_KEY_ClockSize,72);
  render_checked();
  const int default_span=minute_bottom-hour_top;
  assert(default_span > small_span);
  apply_num(MESSAGE_KEY_ClockSize,100);
  for(int i=0;i<1000;i++) render_checked();
  const int full_span=minute_bottom-hour_top;
  assert(full_span >= default_span+15);
  assert(abs((hour_right-hour_left)-(minute_right-minute_left))<=2);
  assert(minute_top-hour_bottom <= 3 && minute_top >= hour_bottom);
  assert(minute_top >= 62 && minute_bottom <= 119); // no clipped top/bottom at 100%

  // Battery percentage must sit immediately under the icon, not far below.
  render_checked();
  assert(battery_rect_count==1 && drawn_pct);
  assert(percent_y - icon_bottom <= 2 && percent_y - icon_bottom >= -4);
  apply_num(MESSAGE_KEY_BatteryPosition,0);
  render_checked();
  assert(percent_y - icon_bottom <= 2 && percent_y - icon_bottom >= -4);
  apply_num(MESSAGE_KEY_BatteryPosition,1);

  // Regression: moving the battery must never change icon/percentage size.
  // Both components share the same right-hand edge at every size/position.
  apply_num(MESSAGE_KEY_BatteryTextSize,95);
  const int saved_text_size=s_settings.battery_text_size;
  for(int pos=0;pos<=1;++pos) {
    apply_num(MESSAGE_KEY_BatteryPosition,pos);
    for(int icon_scale=30;icon_scale<=100;icon_scale+=70) {
      apply_num(MESSAGE_KEY_BatteryIconSize,icon_scale);
      render_checked();
      assert(s_settings.battery_text_size==saved_text_size);
      assert(strcmp(percent_font,FONT_KEY_GOTHIC_24)==0);
      assert(icon_right==134 && percent_right==134);
      assert(percent_y-icon_bottom>=-4 && percent_y-icon_bottom<=2);
    }
  }
  apply_num(MESSAGE_KEY_BatteryPosition,1);
  apply_num(MESSAGE_KEY_BatteryTextSize,50);

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
  // Real Pebble messages may contain packed 1- or 2-byte integer values.
  TupleValue shortval={0};
  shortval.data[0]=1;
  shortval.data[1]=0xA5; // poison the next byte to catch out-of-length reads
  Tuple shorttuple={.key=MESSAGE_KEY_BatteryPosition,.type=TUPLE_UINT,.length=1,.value=&shortval};
  DictionaryIterator shortdict={.tuples=&shorttuple,.count=1};
  message_received(&shortdict,NULL);
  assert(s_settings.battery_position==1);
  shortval.data[0]=0;
  message_received(&shortdict,NULL);
  assert(s_settings.battery_position==0);

  // 2-byte signed integer sign-extension and clamping.
  TupleValue signedval={0};
  signedval.data[0]=0xFF; signedval.data[1]=0xFF;
  Tuple signedtuple={.key=MESSAGE_KEY_ClockSize,.type=TUPLE_INT,.length=2,.value=&signedval};
  DictionaryIterator signeddict={.tuples=&signedtuple,.count=1};
  message_received(&signeddict,NULL);
  assert(s_settings.clock_size==50);

  // Reject malformed strings and CSTRINGs missing their final NUL.
  int32_t parsed_value = 999;
  const uint8_t one[] = {'0',0};
  const uint8_t bad[] = {'0','X'};
  const uint8_t dec[] = {'7','2',0};
  const uint8_t hex[] = {'0','x','F','F','0','0','A','A',0};
  assert(parse_setting_bytes(TUPLE_CSTRING,one,2,&parsed_value) && parsed_value==0);
  assert(!parse_setting_bytes(TUPLE_CSTRING,bad,2,&parsed_value));
  assert(parse_setting_bytes(TUPLE_CSTRING,dec,3,&parsed_value) && parsed_value==72);
  assert(parse_setting_bytes(TUPLE_CSTRING,hex,9,&parsed_value) && parsed_value==0xFF00AA);
  assert(!parse_setting_bytes(TUPLE_CSTRING,one,1,&parsed_value));

  // Bounded Clay strings: choices, booleans and #RRGGBB colors.
  TupleValue selectval={0};
  strcpy(selectval.cstring,"1");
  Tuple selecttuple={.key=MESSAGE_KEY_ClockLayout,.type=TUPLE_CSTRING,.length=2,.value=&selectval};
  DictionaryIterator selectdict={.tuples=&selecttuple,.count=1};
  message_received(&selectdict,NULL);
  assert(s_settings.clock_layout==1);
  // This exact two-byte CSTRING shape was the first tuple in the device crash.
  // Clay selects like "0" arrive as type=1, len=2 (including the trailing NUL).
  strcpy(selectval.cstring,"0");
  message_received(&selectdict,NULL);
  assert(s_settings.clock_layout==0);
  strcpy(selectval.cstring,"1");
  selecttuple.key=MESSAGE_KEY_BatteryPosition;
  message_received(&selectdict,NULL);
  assert(s_settings.battery_position==1);
  strcpy(selectval.cstring,"0");
  message_received(&selectdict,NULL);
  assert(s_settings.battery_position==0);
  selecttuple.key=MESSAGE_KEY_ClockSize;
  strcpy(selectval.cstring,"100");
  selecttuple.length=4;
  message_received(&selectdict,NULL);
  assert(s_settings.clock_size==100);
  TupleValue boolval={0};
  strcpy(boolval.cstring,"false");
  Tuple booltuple={.key=MESSAGE_KEY_ShowAmPm,.type=TUPLE_CSTRING,.length=6,.value=&boolval};
  DictionaryIterator booldict={.tuples=&booltuple,.count=1};
  message_received(&booldict,NULL);
  assert(s_settings.show_ampm==0);
  TupleValue colorval={0};
  strcpy(colorval.cstring,"#55FFAA");
  Tuple coltuple={.key=MESSAGE_KEY_AccentColor,.type=TUPLE_CSTRING,.length=8,.value=&colorval};
  DictionaryIterator colordict={.tuples=&coltuple,.count=1};
  message_received(&colordict,NULL);
  assert(s_settings.accent_color==0x55FFAA);

  // Reject malformed values without changing settings or persisting them.
  int saves_before=fake_save_count;
  shortval.data[0]=99;
  shorttuple.length=3;
  message_received(&shortdict,NULL);
  assert(fake_save_count==saves_before);
  colorval.data[3]='Z'; // no longer valid hex
  message_received(&colordict,NULL);
  assert(fake_save_count==saves_before);
  assert(s_settings.accent_color==0x55FFAA);
  render_checked();
  assert(fake_bad_draw==0);
  // Check the entire 12h clock and several minute values at every size step
  // in both layouts and both battery positions. No drawing may leave Basalt.
  const int minute_cases[]={0,8,11,28,39,59};
  for(int size=50;size<=100;size+=5) {
    s_settings.clock_size=size;
    for(int layout=0;layout<=1;layout++) {
      s_settings.clock_layout=layout;
      for(int pos=0;pos<=1;pos++) {
        s_settings.battery_position=pos;
        for(int hour=1;hour<=12;hour++) {
          snprintf(s_hours,sizeof(s_hours),"%d",hour);
          for(int j=0;j<6;j++) {
            snprintf(s_minutes,sizeof(s_minutes),"%02d",minute_cases[j]);
            render_checked();
          }
        }
      }
    }
  }
  // New: changes to accent length must change actual rectangle dimensions.
  defaults();
  s_settings.hours_color=0xFF0000;
  s_settings.minutes_color=0x55FFAA;
  strcpy(s_hours,"12");strcpy(s_minutes,"59");
  apply_num(MESSAGE_KEY_AccentWidth,8);
  render_checked();
  assert(accent_x==12 && accent_w==8);
  apply_num(MESSAGE_KEY_AccentWidth,120);
  render_checked();
  assert(accent_x==12 && accent_w==120);
  apply_num(MESSAGE_KEY_AccentWidth,29);
  render_checked();
  assert(accent_w==29);

  // New: mirror moves clock, date and accent to the right, battery to left;
  // switching back restores the same positions and sizes.
  apply_num(MESSAGE_KEY_MirrorLayout,0);
  apply_num(MESSAGE_KEY_BatteryPosition,1);
  apply_num(MESSAGE_KEY_BatteryTextSize,50);
  apply_num(MESSAGE_KEY_WeekdaySize,14);
  apply_num(MESSAGE_KEY_DateSize,14);
  render_checked();
  const int normal_hour_left=hour_left;
  const int normal_minutes_right=minute_right;
  const int normal_date_left=date_visible_min_x;
  assert(accent_x==12 && icon_right==134 && percent_right==134);
  apply_num(MESSAGE_KEY_MirrorLayout,1);
  render_checked();
  assert(accent_x==144-12-29 && accent_w==29);
  assert(hour_left > normal_hour_left+10);
  assert(minute_right > normal_minutes_right+10);
  assert(date_visible_min_x > normal_date_left+10);
  assert(icon_left==10 && percent_left==10);
  assert(s_settings.battery_text_size==50);
  apply_num(MESSAGE_KEY_AccentWidth,58);
  render_checked();
  assert(accent_x==144-12-58 && accent_w==58);
  apply_num(MESSAGE_KEY_MirrorLayout,0);
  render_checked();
  assert(accent_x==12 && icon_right==134 && hour_left==normal_hour_left);

  // New: weekday and month/day can be independently changed (10..24 pixels)
  // with fitting to prevent either segment colliding with the bottom battery.
  for (int mirror=0; mirror<=1; ++mirror) {
    apply_num(MESSAGE_KEY_MirrorLayout,mirror);
    for (int pos=0; pos<=1; ++pos) {
      apply_num(MESSAGE_KEY_BatteryPosition,pos);
      for (int wd=10; wd<=24; wd+=2) {
        apply_num(MESSAGE_KEY_WeekdaySize,wd);
        for (int md=10; md<=24; md+=2) {
          apply_num(MESSAGE_KEY_DateSize,md);
          render_checked();
          assert(s_settings.weekday_size==wd && s_settings.date_size==md);
          assert(date_pixels>0);
          assert(date_visible_min_x>=8 && date_visible_max_x<=136);
        }
      }
    }
  }
  // String-width stress: longest abbreviated weekday and representative
  // month/day combinations at maximum font size, including mirror + battery.
  s_settings.weekday_size=24;
  s_settings.date_size=24;
  const char *weekcases[]={"WED","THU","SAT"};
  const char *datecases[]={"SEP 30","NOV 28","MAY 11"};
  for(int mir=0;mir<2;++mir) {
    s_settings.mirror_layout=mir;
    for(int bp=0;bp<2;++bp) {
      s_settings.battery_position=bp;
      for(int tc=0;tc<3;++tc) {
        strcpy(s_weekday,weekcases[tc]);
        strcpy(s_monthday,datecases[tc]);
        render_checked();
        assert(date_pixels>0);
        assert(date_visible_min_x>=8 && date_visible_max_x<=136);
      }
    }
  }
  strcpy(s_weekday,"MON");strcpy(s_monthday,"OCT 25");
  // Mirror also applies in the single-line clock arrangement.
  s_settings.clock_layout=1;
  s_settings.clock_size=50;
  s_settings.battery_position=1;
  s_settings.mirror_layout=0;
  render_checked();
  const int single_normal_hour_left=hour_left;
  s_settings.mirror_layout=1;
  render_checked();
  assert(hour_left>single_normal_hour_left+2);
  assert(accent_x==144-12-s_settings.accent_width);
  assert(icon_left==10);
  s_settings.clock_layout=0;

  // Position-only battery save must never reset new date/font controls.
  const int keep_wd=s_settings.weekday_size,keep_md=s_settings.date_size;
  apply_num(MESSAGE_KEY_BatteryPosition,1);
  assert(s_settings.weekday_size==keep_wd && s_settings.date_size==keep_md);
  apply_num(MESSAGE_KEY_ClockSize,51); // persist the direct test-state changes
  FaceSettings previous=s_settings;
  defaults();load_settings();
  assert(memcmp(&previous,&s_settings,sizeof(previous))==0);
  // Upgrade a persisted v1.0.9 record: keep all old preferences and
  // initialize only the four newly appended fields from v1.0.10 defaults.
  const int old_record_size=sizeof(FaceSettings)-4*sizeof(int32_t);
  s_settings.version=1;
  s_settings.clock_size=93;
  s_settings.battery_text_size=95;
  s_settings.battery_position=0;
  memcpy(stored,&s_settings,old_record_size);
  stored_size=old_record_size;
  defaults();load_settings();
  assert(s_settings.version==SETTINGS_VERSION);
  assert(s_settings.clock_size==93 && s_settings.battery_text_size==95);
  assert(s_settings.battery_position==0);
  assert(s_settings.accent_width==29 && s_settings.weekday_size==14);
  assert(s_settings.date_size==14 && s_settings.mirror_layout==0);
  // Re-saving migrates to the new record size, retaining older preferences.
  apply_num(MESSAGE_KEY_MirrorLayout,1);
  assert(stored_size==sizeof(FaceSettings));
  assert(s_settings.clock_size==93 && s_settings.battery_text_size==95);
  puts("PASS: 21 settings, mirror left/right, accent width, independent day/date sizing, migration, and safe clipping");
  return 0;
}
