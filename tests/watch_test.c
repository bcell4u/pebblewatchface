/* Build: cc -std=c11 -Itests/mocks -Wall -Wextra -Werror -Wno-unused-parameter -o /tmp/test tests/watch_test.c */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pebble.h"

int fake_dirty_count=0, fake_child_count=0, fake_draw_count=0, fake_bad_draw=0, fake_save_count=0;
static int date_native_measure_calls=0;
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
static int accent_x, accent_w, accent_y;
static GColor test_accent_color=0x55FFAA;
static int date_pixels, date_visible_min_x, date_visible_max_x, date_soft_pixels;
static GFont percent_font;
static GFont weekday_font, date_font;
static int battery_rect_count=0, stroke_count=0;
static bool record_tiny_battery=false;
static int tiny_runs, tiny_min_x, tiny_max_x, tiny_min_y, tiny_max_y;
static void reset_tiny_battery(void) {
  tiny_runs=0; tiny_min_x=999; tiny_max_x=-1; tiny_min_y=999; tiny_max_y=-1;
}

static const char *first_time_font;
static int hours_y=-1;
static bool drawn_hours=false, drawn_minutes=false, drawn_ampm=false, drawn_date=false, drawn_pct=false;
static GColor hours_col=0, minutes_col=0, battery_text_col=0;
static GColor weekday_text_col=0, monthday_text_col=0;

GColor GColorFromHEX(uint32_t h){return h;}
bool gcolor_equal(GColor a,GColor b){return a==b;}
GFont fonts_get_system_font(const char *k){return k;}
GSize graphics_text_layout_get_content_size(const char *text,GFont f,GRect rect,GTextOverflowMode o,GTextAlignment a){
  (void)o;(void)a;
  date_native_measure_calls++;
  assert(rect.size.w>=144);
  // Simplified native Gothic font metrics for host-only geometry regression.
  // Real sizing is supplied by Pebble's system font engine on the watch.
  int sz=strstr(f,"GOTHIC28")!=NULL ? 28 :
         strstr(f,"GOTHIC24")!=NULL ? 24 :
         strstr(f,"GOTHIC18")!=NULL ? 18 : 14;
  int ww=(int)strlen(text)*sz/2;
  if (strstr(f, "B")!=NULL) ww += (int)strlen(text)*sz/12;
  return (GSize){.w=ww,.h=sz+4};
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
  if(r.origin.y>=120 && strcmp(t,"80%")!=0 && strcmp(t,"PM")!=0 && strcmp(t,"AM")!=0) {
    drawn_date=true;
    date_pixels+=r.size.w*r.size.h;
    if(r.origin.x<date_visible_min_x)date_visible_min_x=r.origin.x;
    if(r.origin.x+r.size.w>date_visible_max_x)date_visible_max_x=r.origin.x+r.size.w;
    if(strcmp(t,"MON")==0) { weekday_font=f; weekday_text_col=current_text_color; }
    if(strcmp(t,"OCT 25")==0) { date_font=f; monthday_text_col=current_text_color; }
  }

}
void graphics_fill_rect(GContext*c,GRect r,int radius,GCornerMask co){
  (void)c;(void)radius;(void)co;check_rect(r);
  if (record_tiny_battery && r.size.h==1) {
    ++tiny_runs;
    if(r.origin.x<tiny_min_x)tiny_min_x=r.origin.x;
    if(r.origin.x+r.size.w>tiny_max_x)tiny_max_x=r.origin.x+r.size.w;
    if(r.origin.y<tiny_min_y)tiny_min_y=r.origin.y;
    if(r.origin.y+1>tiny_max_y)tiny_max_y=r.origin.y+1;
  }
  if (r.origin.y>=0 && r.origin.y<=165 && r.size.h==3 && r.size.w>=8 && current_fill_color==test_accent_color) {
    accent_x=r.origin.x;
    accent_w=r.size.w;
    accent_y=r.origin.y;
  }
  // Date strokes are binary foreground/background, never washed-out grays.
  if(r.origin.y>=136 && r.size.h==1 && current_fill_color!=0xFFFFFF &&
     current_fill_color!=0x000000 && current_fill_color!=0xFF0000 &&
     current_fill_color!=0x55FFAA) date_soft_pixels++;
  if(r.origin.y>=132 && r.size.h==1 && current_fill_color==0xFFFFFF) {
    date_pixels++;
    drawn_date=true;
    if(r.origin.x<date_visible_min_x)date_visible_min_x=r.origin.x;
    if(r.origin.x+r.size.w>date_visible_max_x)date_visible_max_x=r.origin.x+r.size.w;
  }
  if (r.size.h == 1 && r.origin.y < 136) {
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
  first_time_font=NULL;weekday_font=NULL;date_font=NULL;hours_y=-1;stroke_count=0;battery_rect_count=0;fake_bad_draw=0;
  hour_rect_count=minute_rect_count=0;
  hour_top=minute_top=999;hour_bottom=minute_bottom=0;
  hour_left=minute_left=999;hour_right=minute_right=0;
  icon_bottom=-1;icon_right=-1;icon_left=-1;percent_y=-1;percent_right=-1;percent_left=-1;percent_font=NULL;
  accent_x=-1;accent_w=-1;accent_y=-1;weekday_text_col=0;monthday_text_col=0;date_pixels=0;date_soft_pixels=0;date_visible_min_x=999;date_visible_max_x=-1;
}
static void render_checked(void) {
  reset_draw();
  test_accent_color=(GColor)s_settings.accent_color;
  canvas_update(s_canvas,&fake_context);
  assert(fake_bad_draw==0);
  assert(hour_rect_count > 0 && minute_rect_count > 0 && drawn_date);
  // Scalable large text intentionally uses antialiased intermediate colors.
  if (s_settings.weekday_size<=28 && s_settings.date_size<=28)
    assert(date_soft_pixels==0);
}

int main(void){
  // Date size slider must grow beyond the largest 28pt native font.
  for (int sz=10; sz<=40; ++sz) {
    const int tier=date_font_tier(sz);
    assert(tier==14 || tier==18 || tier==24 || tier==28 || (tier>=29 && tier<=40));
    if (tier<=28) {
      assert(date_native_font(tier,false)!=NULL);
      assert(date_native_font(tier,true)!=NULL);
    }
    assert(measure_date_text("MON",tier,false).w>0);
  }
  assert(date_font_tier(29)==29 && date_font_tier(34)==34 && date_font_tier(40)==40);
  assert(date_custom_text_width("OCT 25",40,false) > date_custom_text_width("OCT 25",29,false));
  assert(date_custom_text_width("MON",40,true) > date_custom_text_width("MON",29,true));
  // Bold atlas must contain substantially more ink than regular glyphs.
  const DateGlyph *normal_m=date_glyph('M',false), *bold_m=date_glyph('M',true);
  int normal_ink=0,bold_ink=0;
  for(int y=0;y<normal_m->height;++y) for(int x=0;x<normal_m->width;++x)
    normal_ink += date_source_alpha(normal_m,k_date_regular_data,x,y);
  for(int y=0;y<bold_m->height;++y) for(int x=0;x<bold_m->width;++x)
    bold_ink += date_source_alpha(bold_m,k_date_bold_data,x,y);
  assert(bold_ink>normal_ink*11/10);
  defaults();
  window_load(&fake_window);
  assert(fake_child_count==1);
  s_battery.charge_percent=80;
  strcpy(s_hours,"12");strcpy(s_minutes,"59");strcpy(s_ampm,"PM");
  strcpy(s_weekday,"MON");strcpy(s_monthday,"OCT 25");

  // Default 30% tier must render a visibly smaller compact percentage,
  // directly in the top-right corner and aligned with the battery icon.
  assert(s_settings.battery_position==0 && s_settings.battery_percent_visible==1);
  assert(s_settings.battery_text_size==30);
  s_settings.battery_icon_visible=0;
  reset_tiny_battery(); record_tiny_battery=true;
  draw_battery(&fake_context,144,168);
  record_tiny_battery=false;
  assert(tiny_runs>12 && tiny_min_x>=117 && tiny_max_x==134);
  assert(tiny_min_y==10 && tiny_max_y==17);
  // Mirror keeps the group against the opposite screen edge.
  s_settings.mirror_layout=1;
  reset_tiny_battery(); record_tiny_battery=true;
  draw_battery(&fake_context,144,168);
  record_tiny_battery=false;
  assert(tiny_runs>12 && tiny_min_x==10 && tiny_max_x<=27);
  s_settings.mirror_layout=0;
  s_settings.battery_icon_visible=1;
  reset_tiny_battery(); record_tiny_battery=true;
  draw_battery(&fake_context,144,168);
  record_tiny_battery=false;
  assert(tiny_min_y>20 && tiny_max_y<40);

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
    MESSAGE_KEY_WeekdaySize, MESSAGE_KEY_DateSize, MESSAGE_KEY_MirrorLayout, MESSAGE_KEY_ClockSpacing,
    MESSAGE_KEY_WeekdayBold, MESSAGE_KEY_DateBold, MESSAGE_KEY_AccentYOffset, MESSAGE_KEY_WeekdayColor };
  int32_t vals[] = { 1, 100, 1, 1, 0, 1, 1, 30, 50, 0,
    0xFF0000, 0x55FFAA, 0xAAAAAA, 0x55FFAA, 0xFFFFFF,
    0x55FFAA, 0x00FF00, 29, 14, 14, 0, 4, 0, 0, 0, 0xFFFFFF };
  TupleValue values[26];Tuple tuples[26];
  for(int i=0;i<26;i++) { values[i].int32=vals[i]; tuples[i]=(Tuple){.key=keys[i],.type=TUPLE_INT,.length=4,.value=&values[i]}; }
  DictionaryIterator dict={.tuples=tuples,.count=26};
  int dirty_before=fake_dirty_count;
  message_received(&dict,NULL);
  assert(fake_save_count==1 && fake_dirty_count==dirty_before+1);
  assert(s_settings.clock_layout==1 && s_settings.clock_size==100 && s_settings.battery_position==0);
  assert(s_settings.battery_text_color==0x00FF00);
  assert(s_settings.accent_width==29 && s_settings.weekday_size==21);
  assert(s_settings.date_size==27 && s_settings.mirror_layout==0 && s_settings.clock_spacing==4);
  assert(s_settings.weekday_bold==0 && s_settings.date_bold==0 && s_settings.accent_y_offset==0);
  assert(s_settings.weekday_color==0xFFFFFF);
  render_checked();
  assert(hour_top >= 45); // Top-right battery moves the inline clock down.
  assert(battery_text_col==0x00FF00);
  assert(drawn_pct && battery_rect_count==1);

  apply_num(MESSAGE_KEY_ClockLayout,0);
  apply_num(MESSAGE_KEY_BatteryPosition,1);
  render_checked();
  assert(minute_top-hour_bottom >= 3 && minute_top-hour_bottom <= 8);
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
  assert(minute_top-hour_bottom >= 3 && minute_top-hour_bottom <= 8);
  assert(minute_top >= 64 && minute_bottom <= accent_y-1); // no clipped top/bottom at 100%

  // Clock spacing is its own saved 1..10 px slider. Verify geometry at
  // minimum, default and maximum, across layout directions and font scales.
  defaults();
  s_settings.weekday_size=14; s_settings.date_size=14; s_settings.accent_y_offset=0;
  s_settings.hours_color=0xFF0000;
  s_settings.minutes_color=0x55FFAA;
  strcpy(s_hours,"12"); strcpy(s_minutes,"59");
  int previous_ink_gap=-1;
  for (int g=1; g<=10; ++g) {
    apply_num(MESSAGE_KEY_ClockSpacing,g);
    assert(s_settings.clock_spacing==g);
    for (int mirror=0; mirror<=1; ++mirror) {
      s_settings.mirror_layout=mirror;
      for (int size=50;size<=100;size+=5) {
        s_settings.clock_size=size;
        render_checked();
        assert(hour_bottom <= minute_top);
        assert(minute_bottom < accent_y); // never touches dynamically positioned accent
      }
    }
    s_settings.mirror_layout=0;
    s_settings.clock_size=100;
    render_checked();
    const int ink_gap=minute_top-hour_bottom;
    if (previous_ink_gap != -1) assert(ink_gap >= previous_ink_gap);
    previous_ink_gap=ink_gap;
  }
  // The accent line should descend into formerly unused whitespace as the
  // requested gap increases, not force the numerals to shrink prematurely.
  s_settings.clock_size=100;
  s_settings.weekday_size=14;
  s_settings.date_size=14;
  apply_num(MESSAGE_KEY_ClockSpacing,1);
  render_checked();
  const int accent_y_gap1=accent_y;
  const int minute_height_gap1=minute_bottom-minute_top;
  apply_num(MESSAGE_KEY_ClockSpacing,10);
  render_checked();
  assert(accent_y>accent_y_gap1 && accent_y<=133);
  assert(minute_bottom-minute_top>=minute_height_gap1-2);
  s_settings.weekday_size=24; s_settings.date_size=24;
  render_checked();
  assert(accent_y+6<=compute_date_layout(144,168).top_y); // clear actual fitted date row
  s_settings.weekday_size=14; s_settings.date_size=14;
  apply_num(MESSAGE_KEY_ClockSpacing,1);
  render_checked();
  const int narrow_ink_gap=minute_top-hour_bottom;
  apply_num(MESSAGE_KEY_ClockSpacing,10);
  render_checked();
  assert(minute_top-hour_bottom >= narrow_ink_gap+6);
  apply_num(MESSAGE_KEY_ClockSpacing,-7);
  assert(s_settings.clock_spacing==1);
  apply_num(MESSAGE_KEY_ClockSpacing,25);
  assert(s_settings.clock_spacing==10);
  apply_num(MESSAGE_KEY_ClockSpacing,4);
  s_settings.clock_size=100;

  // Clay may send signed slider values as short decimal strings.
  int32_t signed_offset=0;
  assert(parse_setting_bytes(TUPLE_CSTRING,(const uint8_t*)"-7",3,&signed_offset));
  assert(signed_offset==-7);
  assert(parse_setting_bytes(TUPLE_CSTRING,(const uint8_t*)"+9",3,&signed_offset));
  assert(signed_offset==9);

  // Accent offset: positive goes up, negative goes down; changing only accent
  // dimensions/position or the weekday/date size and bold preferences.
  defaults();
  s_settings.accent_y_offset=0;
  s_settings.clock_size=72;
  s_settings.clock_spacing=4;
  s_settings.hours_color=0xFF0000;
  s_settings.minutes_color=0x55FFAA;
  s_settings.weekday_size=14;
  s_settings.date_size=14;
  s_settings.battery_position=0;
  s_settings.mirror_layout=0;
  render_checked();
  const int accent_auto=accent_y;
  const int original_hour_top=hour_top, original_minute_top=minute_top;
  const int original_hour_bottom=hour_bottom, original_minute_bottom=minute_bottom;
  apply_num(MESSAGE_KEY_AccentYOffset,-5);
  assert(s_settings.accent_y_offset==-5);
  render_checked();
  assert(accent_y==accent_auto+5);
  assert(hour_top==original_hour_top && minute_top==original_minute_top);
  assert(hour_bottom==original_hour_bottom && minute_bottom==original_minute_bottom);
  apply_num(MESSAGE_KEY_AccentYOffset,5);
  render_checked();
  assert(accent_y==accent_auto-5);
  apply_num(MESSAGE_KEY_AccentYOffset,-99);
  assert(s_settings.accent_y_offset==-12);
  apply_num(MESSAGE_KEY_AccentYOffset,99);
  assert(s_settings.accent_y_offset==12);
  // Every step must produce a real one-pixel movement, even at 100% time
  // size, larger date sizes, maximum clock spacing and in mirrored mode.
  // Manual offsets are allowed to overlap neighboring text at extremes.
  for(int mir=0;mir<=1;++mir)
    for(int scale=50;scale<=100;scale+=10)
      for(int spacing=1;spacing<=10;spacing+=3)
        for(int pos=0;pos<=1;++pos) {
          s_settings.mirror_layout=mir;
          s_settings.clock_size=scale;
          s_settings.clock_spacing=spacing;
          s_settings.battery_position=pos;
          apply_num(MESSAGE_KEY_AccentYOffset,0);
          render_checked();
          int base_y=accent_y;
          for(int off=-12;off<=12;++off) {
            apply_num(MESSAGE_KEY_AccentYOffset,off);
            render_checked();
            assert(accent_y==base_y-off);
          }
        }
  // Check that changing the line position doesn't alter saved font settings.
  apply_num(MESSAGE_KEY_AccentYOffset,-9);
  assert(s_settings.weekday_size==14 && s_settings.date_size==14);
  defaults();
  s_settings.hours_color=0xFF0000;
  s_settings.minutes_color=0x55FFAA;
  apply_num(MESSAGE_KEY_AccentYOffset,0);
  // Battery percentage must sit immediately under the icon, not far below.
  s_settings.battery_percent_visible=1;
  s_settings.battery_text_size=50; // preserve native Gothic tier regression
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
  assert(s_settings.clock_size==120);
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
          if(hour>=10) {s_hours[0]='1'; s_hours[1]=(char)('0'+hour-10); s_hours[2]=0;}
          else {s_hours[0]=(char)('0'+hour); s_hours[1]=0;}
          for(int j=0;j<6;j++) {
            s_minutes[0]=(char)('0'+minute_cases[j]/10);
            s_minutes[1]=(char)('0'+minute_cases[j]%10);
            s_minutes[2]=0;
            render_checked();
          }
        }
      }
    }
  }
  // New: changes to accent length must change actual rectangle dimensions.
  defaults();
  s_settings.battery_percent_visible=1;
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

  // Independent centering controls: centering one must not center the other.
  // Both toggles must override mirror alignment, then restore it when off.
  defaults();
  s_settings.hours_color=0xFF0000;
  s_settings.minutes_color=0x55FFAA;
  apply_num(MESSAGE_KEY_AccentWidth,60);
  apply_num(MESSAGE_KEY_WeekdaySize,14);
  apply_num(MESSAGE_KEY_DateSize,14);
  render_checked();
  assert(accent_x==12);
  DateLayout left_fit=compute_date_layout(144,168);
  assert(left_fit.weekday_x==12);
  apply_num(MESSAGE_KEY_CenterAccent,1);
  render_checked();
  assert(s_settings.center_accent==1 && accent_x==42);
  assert(get_date_layout(144,168)->weekday_x==12);
  apply_num(MESSAGE_KEY_CenterDateRow,1);
  render_checked();
  DateLayout centered_fit=*get_date_layout(144,168);
  const int centered_total=centered_fit.weekday_w+4+centered_fit.date_w;
  assert(centered_fit.weekday_x==(144-centered_total)/2);
  assert(centered_fit.date_x==centered_fit.weekday_x+centered_fit.weekday_w+4);
  assert(centered_fit.weekday_x>left_fit.weekday_x);
  apply_num(MESSAGE_KEY_MirrorLayout,1);
  render_checked();
  assert(accent_x==42);
  assert(get_date_layout(144,168)->weekday_x==centered_fit.weekday_x);
  apply_num(MESSAGE_KEY_CenterAccent,0);
  render_checked();
  assert(accent_x==144-12-60);
  assert(get_date_layout(144,168)->weekday_x==centered_fit.weekday_x);
  apply_num(MESSAGE_KEY_CenterDateRow,0);
  render_checked();
  assert(get_date_layout(144,168)->weekday_x==144-12-centered_total);
  // Values are normalized, and the independent settings survive persistence.
  apply_num(MESSAGE_KEY_CenterAccent,100);
  apply_num(MESSAGE_KEY_CenterDateRow,-1);
  assert(s_settings.center_accent==1 && s_settings.center_date_row==0);
  apply_num(MESSAGE_KEY_CenterDateRow,1);
  FaceSettings centered_saved=s_settings;
  defaults(); load_settings();
  assert(memcmp(&centered_saved,&s_settings,sizeof(s_settings))==0);
  defaults();
  s_settings.hours_color=0xFF0000;
  s_settings.minutes_color=0x55FFAA;
  apply_num(MESSAGE_KEY_AccentWidth,29);

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

  // Regression: Clay can resend stale weekday/date slider defaults with
  // an accent-only edit. Save previously customized typography and verify
  // that changing the accent length cannot alter those saved sizes.
  apply_num(MESSAGE_KEY_AccentWidth,29);
  apply_num(MESSAGE_KEY_WeekdaySize,22);
  apply_num(MESSAGE_KEY_DateSize,18);
  render_checked();
  const int day_pixels_before_accent = date_pixels;
  const int day_left_before_accent = date_visible_min_x;
  const int day_right_before_accent = date_visible_max_x;
  TupleValue groupvals[3]={ {.int32=96}, {.int32=14}, {.int32=14} };
  Tuple grouptuples[3]={
    {.key=MESSAGE_KEY_AccentWidth,.type=TUPLE_INT,.length=4,.value=&groupvals[0]},
    {.key=MESSAGE_KEY_WeekdaySize,.type=TUPLE_INT,.length=4,.value=&groupvals[1]},
    {.key=MESSAGE_KEY_DateSize,.type=TUPLE_INT,.length=4,.value=&groupvals[2]}
  };
  DictionaryIterator grouped={.tuples=grouptuples,.count=3};
  message_received(&grouped,NULL);
  assert(s_settings.accent_width==96);
  assert(s_settings.weekday_size==22 && s_settings.date_size==18);
  render_checked();
  assert(accent_w==96);
  assert(date_pixels==day_pixels_before_accent);
  assert(date_visible_min_x==day_left_before_accent);
  assert(date_visible_max_x==day_right_before_accent);
  // Date-only saves still change sizes independently, even after an accent edit.
  apply_num(MESSAGE_KEY_WeekdaySize,17);
  apply_num(MESSAGE_KEY_DateSize,12);
  assert(s_settings.accent_width==96);
  assert(s_settings.weekday_size==17 && s_settings.date_size==12);
  apply_num(MESSAGE_KEY_AccentWidth,29);

  // Weekday and date must draw with independently saved colors, even when
  // the text sizes differ or the layout is mirrored.
  s_settings.weekday_size=14; s_settings.date_size=14;
  apply_num(MESSAGE_KEY_DateColor,0xFFFF00);
  apply_num(MESSAGE_KEY_WeekdayColor,0x00FFFF);
  render_checked();
  assert(weekday_text_col==0x00FFFF && monthday_text_col==0xFFFF00);
  apply_num(MESSAGE_KEY_WeekdayColor,0xFF00FF);
  render_checked();
  assert(weekday_text_col==0xFF00FF && monthday_text_col==0xFFFF00);
  apply_num(MESSAGE_KEY_DateColor,0x00FF00);
  render_checked();
  assert(weekday_text_col==0xFF00FF && monthday_text_col==0x00FF00);
  apply_num(MESSAGE_KEY_WeekdayColor,0xFFFFFF);
  apply_num(MESSAGE_KEY_DateColor,0xFFFFFF);

  // Independently selected native font tiers. Neither font is scaled by
  // rectangular pixel painting: graphics_draw_text receives the system font.
  strcpy(s_weekday,"MON");strcpy(s_monthday,"OCT 25");
  s_settings.battery_position=0;
  s_settings.weekday_size=14;s_settings.date_size=14;
  render_checked();
  assert(strcmp(weekday_font,FONT_KEY_GOTHIC_14)==0);
  assert(strcmp(date_font,FONT_KEY_GOTHIC_14)==0);
  s_settings.weekday_size=18;
  render_checked();
  assert(strcmp(weekday_font,FONT_KEY_GOTHIC_18)==0);
  assert(strcmp(date_font,FONT_KEY_GOTHIC_14)==0);
  s_settings.weekday_size=14;s_settings.date_size=18;
  render_checked();
  assert(strcmp(weekday_font,FONT_KEY_GOTHIC_14)==0);
  assert(strcmp(date_font,FONT_KEY_GOTHIC_18)==0);

  // Both bold toggles independently control font weight and persist.
  // Large sizes use real antialiased masks, not the same native 28pt font.
  s_settings.weekday_size=14;s_settings.date_size=14;
  apply_num(MESSAGE_KEY_WeekdayBold,1);
  render_checked();
  assert(strcmp(weekday_font,FONT_KEY_GOTHIC_14_BOLD)==0);
  assert(strcmp(date_font,FONT_KEY_GOTHIC_14)==0);
  apply_num(MESSAGE_KEY_DateBold,1);
  render_checked();
  assert(strcmp(weekday_font,FONT_KEY_GOTHIC_14_BOLD)==0);
  assert(strcmp(date_font,FONT_KEY_GOTHIC_14_BOLD)==0);
  apply_num(MESSAGE_KEY_WeekdayBold,0);
  render_checked();
  assert(strcmp(weekday_font,FONT_KEY_GOTHIC_14)==0);
  assert(strcmp(date_font,FONT_KEY_GOTHIC_14_BOLD)==0);
  apply_num(MESSAGE_KEY_DateBold,0);
  apply_num(MESSAGE_KEY_WeekdaySize,40);
  apply_num(MESSAGE_KEY_DateSize,40);
  assert(s_settings.weekday_size==40 && s_settings.date_size==40);
  s_settings.battery_position=0;
  render_checked();
  // Both fields at 40 must fit together; effective sizes are reduced as needed.
  assert(compute_date_layout(144,168).weekday_tier>=14);
  apply_num(MESSAGE_KEY_WeekdayBold,1);
  apply_num(MESSAGE_KEY_DateBold,1);
  render_checked();
  assert(s_settings.weekday_bold==1 && s_settings.date_bold==1);
  FaceSettings bold_saved=s_settings;
  defaults();load_settings();
  assert(memcmp(&bold_saved,&s_settings,sizeof(bold_saved))==0);
  apply_num(MESSAGE_KEY_WeekdayBold,0);
  apply_num(MESSAGE_KEY_DateBold,0);

  // With room beside the text, a requested size of 40 genuinely draws
  // larger than 29. Switching bold preserves the requested size and gives
  // the separate segment a distinct set of glyphs.
  s_settings.battery_position=0;
  s_settings.weekday_size=10;
  s_settings.date_size=29;
  s_settings.weekday_bold=0;
  s_settings.date_bold=0;
  DateLayout small_date=compute_date_layout(144,168);
  s_settings.date_size=40;
  DateLayout bigger_date=compute_date_layout(144,168);
  assert(bigger_date.date_tier>small_date.date_tier);
  assert(measure_date_text(s_monthday,bigger_date.date_tier,false).h >
         measure_date_text(s_monthday,small_date.date_tier,false).h);
  s_settings.weekday_bold=1;
  s_settings.date_bold=1;
  assert(s_settings.weekday_bold==1 && s_settings.date_bold==1);
  render_checked();
  s_settings.weekday_bold=0;
  s_settings.date_bold=0;

  // Moving the battery MUST change effective fitted text size without
  // altering either saved slider value. A top or hidden battery frees the
  // bottom corner and therefore allows larger native Gothic tiers.
  s_settings.weekday_size = 40;
  s_settings.date_size = 40;
  s_settings.battery_enabled = 1;
  s_settings.battery_icon_visible = 1;
  s_settings.battery_percent_visible = 1;
  strcpy(s_weekday,"MON");
  strcpy(s_monthday,"OCT 25");
  s_settings.weekday_bold = 0;
  s_settings.date_bold = 0;
  s_settings.battery_position = 1;
  DateLayout bottom_fit = compute_date_layout(144, 168);
  s_settings.battery_position = 0;
  DateLayout top_fit = compute_date_layout(144, 168);
  assert(top_fit.weekday_tier >= bottom_fit.weekday_tier);
  assert(top_fit.date_tier >= bottom_fit.date_tier);
  assert(top_fit.weekday_tier > bottom_fit.weekday_tier ||
         top_fit.date_tier > bottom_fit.date_tier);
  s_settings.battery_enabled = 0;
  DateLayout hidden_fit = compute_date_layout(144, 168);
  assert(top_fit.weekday_tier == hidden_fit.weekday_tier);
  assert(top_fit.date_tier == hidden_fit.date_tier);
  s_settings.battery_enabled = 1;
  s_settings.battery_position = 1;
  assert(s_settings.weekday_size == 40 && s_settings.date_size == 40);
  // Check both regular and bold, mirrored and not, with both battery
  // placements: all text rectangles must remain within the screen.
  s_settings.accent_y_offset=0;
  for (int mir=0;mir<=1;mir++) for (int bold=0;bold<=1;bold++)
    for (int pos=0;pos<=1;pos++) {
      s_settings.mirror_layout=mir;
      s_settings.weekday_bold=bold;
      s_settings.date_bold=bold;
      s_settings.battery_position=pos;
      render_checked();
      assert(accent_y + 6 <= compute_date_layout(144,168).top_y);
    }
  s_settings.weekday_bold=0;
  s_settings.date_bold=0;
  s_settings.mirror_layout=0;

  // Weekday and month/day can be independently changed (10..40 requested)
  // with fitting to prevent either segment colliding with the bottom battery.
  for (int mirror=0; mirror<=1; ++mirror) {
    apply_num(MESSAGE_KEY_MirrorLayout,mirror);
    for (int pos=0; pos<=1; ++pos) {
      apply_num(MESSAGE_KEY_BatteryPosition,pos);
      for (int wd=10; wd<=40; wd+=3) {
        apply_num(MESSAGE_KEY_WeekdaySize,wd);
        for (int md=10; md<=40; md+=3) {
          apply_num(MESSAGE_KEY_DateSize,md);
          render_checked();
          assert(s_settings.weekday_size==wd && s_settings.date_size==md);
          DateLayout fit=compute_date_layout(144,168);
          assert(fit.weekday_x>=8 && fit.date_x+fit.date_w<=136);
          assert(fit.top_y>=90 && fit.date_y+fit.date_h<=168);
        }
      }
    }
  }
  // String-width stress: longest abbreviated weekday and representative
  // month/day combinations at maximum font size, including mirror + battery.
  s_settings.weekday_size=40;
  s_settings.date_size=40;
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
        DateLayout fit=compute_date_layout(144,168);
        assert(fit.weekday_x>=8 && fit.date_x+fit.date_w<=136);
        assert(fit.top_y>=90 && fit.date_y+fit.date_h<=168);
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
  const int old_record_size=offsetof(FaceSettings,accent_width);
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
  assert(s_settings.accent_width==60 && s_settings.weekday_size==21);
  assert(s_settings.date_size==27 && s_settings.mirror_layout==0 && s_settings.clock_spacing==2);
  assert(s_settings.weekday_bold==0 && s_settings.date_bold==0);
  // Re-saving migrates to the new record size, retaining older preferences.
  apply_num(MESSAGE_KEY_MirrorLayout,1);
  assert(stored_size==sizeof(FaceSettings));
  assert(s_settings.clock_size==93 && s_settings.battery_text_size==95);
  // v2->v4 migration preserves all previous settings and defaults gap/bold controls.
  defaults();
  s_settings.version=2;
  s_settings.clock_size=87;
  s_settings.mirror_layout=1;
  s_settings.accent_width=58;
  s_settings.weekday_size=20;
  s_settings.battery_text_size=95;
  const int previous_size=offsetof(FaceSettings,clock_spacing);
  memcpy(stored,&s_settings,previous_size);
  stored_size=previous_size;
  defaults();load_settings();
  assert(s_settings.version==SETTINGS_VERSION && s_settings.clock_spacing==2);
  assert(s_settings.weekday_bold==0 && s_settings.date_bold==0);
  assert(s_settings.clock_size==87 && s_settings.mirror_layout==1);
  assert(s_settings.accent_width==58 && s_settings.weekday_size==20);
  assert(s_settings.battery_text_size==95);
  apply_num(MESSAGE_KEY_ClockSpacing,8);
  assert(s_settings.clock_spacing==8 && stored_size==sizeof(FaceSettings));
  FaceSettings v3_saved=s_settings;
  defaults();load_settings();
  assert(memcmp(&v3_saved,&s_settings,sizeof(FaceSettings))==0);
  // v4 -> v5 migration must preserve all 24 existing preferences.
  defaults();
  s_settings.version=4;
  s_settings.clock_size=96;
  s_settings.clock_spacing=9;
  s_settings.weekday_bold=1;
  s_settings.date_bold=1;
  s_settings.accent_width=73;
  const int v4_size=offsetof(FaceSettings,accent_y_offset);
  memcpy(stored,&s_settings,v4_size);
  stored_size=v4_size;
  defaults();load_settings();
  assert(s_settings.version==SETTINGS_VERSION);
  assert(s_settings.accent_y_offset==-4);
  assert(s_settings.clock_size==96 && s_settings.clock_spacing==9);
  assert(s_settings.weekday_bold==1 && s_settings.date_bold==1 && s_settings.accent_width==73);
  apply_num(MESSAGE_KEY_AccentYOffset,-8);
  assert(s_settings.accent_y_offset==-8 && stored_size==sizeof(FaceSettings));
  FaceSettings v5_saved=s_settings;
  defaults();load_settings();
  assert(memcmp(&v5_saved,&s_settings,sizeof(FaceSettings))==0);
  // Upgrade an actual v5 record: preserve the original unified date color
  // as the weekday color, instead of resetting the weekday to white.
  defaults();
  s_settings.version=5;
  s_settings.date_color=0x55FFAA;
  s_settings.clock_size=98;
  const int old_v5=offsetof(FaceSettings,weekday_color);
  memcpy(stored,&s_settings,old_v5);
  stored_size=old_v5;
  defaults();load_settings();
  assert(s_settings.version==SETTINGS_VERSION);
  assert(s_settings.date_color==0x55FFAA);
  assert(s_settings.weekday_color==0x55FFAA);
  assert(s_settings.clock_size==98);
  apply_num(MESSAGE_KEY_WeekdayColor,0xFF0000);
  assert(s_settings.weekday_color==0xFF0000 && s_settings.date_color==0x55FFAA);
  defaults();load_settings();
  assert(s_settings.weekday_color==0xFF0000 && s_settings.date_color==0x55FFAA);

  // Verify the new Oxford Blue first-install default.
  defaults();
  assert(s_settings.background_color==0x000055);

  // Upgrade an actual v6 record: preserve the already independent colors
  // while defaulting only the two new horizontal-center flags to OFF.
  defaults();
  s_settings.version=6;
  s_settings.clock_size=115;
  s_settings.weekday_color=0x00FF00;
  s_settings.date_color=0xFF00FF;
  s_settings.background_color=0x555555; // preserve existing user preference
  s_settings.mirror_layout=1;
  const int v6_record_size=offsetof(FaceSettings,center_accent);
  memcpy(stored,&s_settings,v6_record_size);
  stored_size=v6_record_size;
  defaults(); load_settings();
  assert(s_settings.version==SETTINGS_VERSION);
  assert(s_settings.clock_size==115 && s_settings.mirror_layout==1);
  assert(s_settings.weekday_color==0x00FF00 && s_settings.date_color==0xFF00FF);
  assert(s_settings.background_color==0x555555);
  assert(s_settings.center_accent==0 && s_settings.center_date_row==0);
  apply_num(MESSAGE_KEY_CenterDateRow,1);
  assert(stored_size==sizeof(FaceSettings));
  defaults(); load_settings();
  assert(s_settings.center_date_row==1 && s_settings.center_accent==0);

  // With ordinary small date settings, the stacked clock at 120% really
  // occupies more pixels than 100% without crossing the automatic accent.
  defaults();
  s_settings.hours_color=0xFF0000;
  s_settings.minutes_color=0x55FFAA;
  s_settings.clock_spacing=4;
  s_settings.weekday_size=14;
  s_settings.date_size=14;
  strcpy(s_hours,"12"); strcpy(s_minutes,"59");
  apply_num(MESSAGE_KEY_ClockSize,100);
  render_checked();
  const int span100 = minute_bottom-hour_top;
  apply_num(MESSAGE_KEY_ClockSize,120);
  render_checked();
  const int span120 = minute_bottom-hour_top;
  assert(span120 >= span100+6);
  assert(minute_bottom < accent_y && hour_top >= 0);
  for (int mirror=0;mirror<=1;++mirror) for (int pos=0;pos<=1;++pos) {
    s_settings.mirror_layout=mirror;
    s_settings.battery_position=pos;
    s_settings.clock_spacing=10;
    render_checked();
    assert(hour_top>=0 && minute_bottom<accent_y);
  }

  // Optimized event path: no redraw for an identical minute, or a battery
  // notification that only changes the charging flag (not displayed).
  defaults();
  s_last_minute_of_day=-1;
  s_last_yday=-1;
  struct tm example={0};
  example.tm_hour=23; example.tm_min=9; example.tm_yday=297;
  example.tm_wday=0; example.tm_mon=9; example.tm_mday=25;
  int redraw_before=fake_dirty_count;
  update_time_from_tm(&example);
  assert(strcmp(s_hours,"11")==0 && strcmp(s_minutes,"09")==0);
  assert(strcmp(s_ampm,"PM")==0 && strcmp(s_weekday,"SUN")==0);
  assert(strcmp(s_monthday,"OCT 25")==0);
  assert(fake_dirty_count==redraw_before+1);
  update_time_from_tm(&example);
  assert(fake_dirty_count==redraw_before+1);
  example.tm_min=10;
  update_time_from_tm(&example);
  assert(fake_dirty_count==redraw_before+2);
  example.tm_hour=0; example.tm_min=0; example.tm_wday=1;
  example.tm_yday=298; example.tm_mday=26;
  update_time_from_tm(&example);
  assert(strcmp(s_hours,"12")==0 && strcmp(s_ampm,"AM")==0);
  assert(strcmp(s_weekday,"MON")==0 && strcmp(s_monthday,"OCT 26")==0);
  s_battery=(BatteryChargeState){80,false,false};
  redraw_before=fake_dirty_count;
  battery_event((BatteryChargeState){80,true,true});
  assert(fake_dirty_count==redraw_before);
  battery_event((BatteryChargeState){79,true,true});
  assert(fake_dirty_count==redraw_before+1);
  s_settings.battery_enabled=0;
  battery_event((BatteryChargeState){78,true,true});
  assert(fake_dirty_count==redraw_before+1);
  // Minute redraws must reuse an unchanged fitted date layout.  Direct
  // changes to size/placement or the calendar text still invalidate it.
  defaults();
  s_settings.hours_color=0xFF0000;
  s_settings.minutes_color=0x55FFAA;
  strcpy(s_weekday,"MON");strcpy(s_monthday,"OCT 25");
  strcpy(s_hours,"12");strcpy(s_minutes,"59");
  s_date_layout_cache_valid=false;
  int before_measure=date_native_measure_calls;
  render_checked();
  assert(date_native_measure_calls>=before_measure+2);
  int first_measures=date_native_measure_calls;
  render_checked();
  assert(date_native_measure_calls==first_measures);
  s_settings.weekday_size=18;
  render_checked();
  assert(date_native_measure_calls>first_measures);
  int changed_measures=date_native_measure_calls;
  strcpy(s_weekday,"WED");
  render_checked();
  assert(date_native_measure_calls>changed_measures);
  changed_measures=date_native_measure_calls;
  s_settings.battery_position=1;
  render_checked();
  assert(date_native_measure_calls>changed_measures);
  // Compact 16-bit glyph offsets must cover all packed alpha data.
  for (int i=0; i<10; ++i) {
    assert(k_smooth_bold_glyphs[i].offset +
      (int)k_smooth_bold_glyphs[i].width*k_smooth_bold_glyphs[i].height <=
      (int)sizeof(k_smooth_bold_alpha)*4);
    assert(k_smooth_light_glyphs[i].offset +
      (int)k_smooth_light_glyphs[i].width*k_smooth_light_glyphs[i].height <=
      (int)sizeof(k_smooth_light_alpha)*4);
  }
  for (int i=0; i<32; ++i) {
    assert(k_date_regular_glyphs[i].offset +
      ((int)k_date_regular_glyphs[i].width*k_date_regular_glyphs[i].height+3)/4 <=
      (int)sizeof(k_date_regular_data));
    assert(k_date_bold_glyphs[i].offset +
      ((int)k_date_bold_glyphs[i].width*k_date_bold_glyphs[i].height+3)/4 <=
      (int)sizeof(k_date_bold_data));
  }
  // The small date atlas includes only letters actually used by English
  // weekday/month labels. Unused glyphs must be rejected safely.
  assert(date_glyph_index('K')==-1 && date_glyph_index('Q')==-1);
  assert(date_glyph_index('X')==-1 && date_glyph_index('Z')==-1);
  assert(date_glyph_index('M')>=0 && date_glyph_index('9')>=0);
  puts("PASS: 28 settings; +Y up/-Y down; faster redraws; compact fonts; saved settings");
  return 0;
}
