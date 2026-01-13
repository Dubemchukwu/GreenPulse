#include <lvgl.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

unsigned long lastTick = 0;

// Create a local frame buffer for LVGL
static uint8_t framebuffer[SCREEN_WIDTH * SCREEN_HEIGHT / 8];

// Forward declaration
void myDispFlush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n\n========================================");
  Serial.println("OLED + LVGL - Fixed Partial Refresh");
  Serial.println("========================================\n");
  
  pinMode(15, OUTPUT);
  digitalWrite(15, HIGH);
  
  // Initialize I2C
  Serial.println("Initializing I2C...");
  Wire.begin(6, 7);
  Wire.setClock(400000);
  
  // Initialize display
  Serial.println("Initializing Display...");
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("ERROR: Display failed at 0x3C, trying 0x3D...");
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) {
      Serial.println("ERROR: Display not found!");
      while(1);
    }
  }
  Serial.println("✓ Display OK\n");
  
  // Clear frame buffer
  memset(framebuffer, 0, sizeof(framebuffer));
  
  // Test display
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.println("Starting LVGL...");
  display.display();
  delay(1000);
  
  // Initialize LVGL
  Serial.println("Initializing LVGL...");
  lv_init();
  
  // Create display buffer - LARGER buffer for better updates
  static lv_disp_draw_buf_t draw_buf;
  static lv_color_t buf[SCREEN_WIDTH * SCREEN_HEIGHT];  // Full screen buffer
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, SCREEN_WIDTH * SCREEN_HEIGHT);
  
  // Initialize display driver
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  
  disp_drv.hor_res = SCREEN_WIDTH;
  disp_drv.ver_res = SCREEN_HEIGHT;
  disp_drv.flush_cb = myDispFlush;
  disp_drv.draw_buf = &draw_buf;
  
  // CRITICAL: Force full refresh every time
  disp_drv.full_refresh = 1;
  
  lv_disp_drv_register(&disp_drv);
  Serial.println("✓ LVGL OK\n");
  
  // Create UI
  Serial.println("Creating UI...");
  createUI();
  
  // Force initial full redraw
  lv_obj_invalidate(lv_scr_act());
  lv_refr_now(NULL);
  
  Serial.println("\n========================================");
  Serial.println("Setup Complete!");
  Serial.println("========================================\n");
}

void myDispFlush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  static unsigned long flush_count = 0;
  flush_count++;
  
  // Debug output
  if (flush_count % 50 == 1) {
    Serial.print("Flush #");
    Serial.print(flush_count);
    Serial.print(" - Full screen: ");
    Serial.print((area->x1 == 0 && area->y1 == 0 && 
                  area->x2 == SCREEN_WIDTH-1 && area->y2 == SCREEN_HEIGHT-1) ? "YES" : "NO");
    Serial.print(" Area: (");
    Serial.print(area->x1);
    Serial.print(",");
    Serial.print(area->y1);
    Serial.print(")-(");
    Serial.print(area->x2);
    Serial.print(",");
    Serial.print(area->y2);
    Serial.println(")");
  }
  
  // CRITICAL FIX: Update framebuffer first, then send entire buffer to display
  uint16_t width = area->x2 - area->x1 + 1;
  uint16_t height = area->y2 - area->y1 + 1;
  
  // Update the local framebuffer with new pixels
  for (uint16_t y = 0; y < height; y++) {
    for (uint16_t x = 0; x < width; x++) {
      lv_color_t c = color_p[y * width + x];
      
      int32_t px = area->x1 + x;
      int32_t py = area->y1 + y;
      
      // Calculate byte position in framebuffer (SSD1306 uses vertical bytes)
      uint16_t byte_idx = px + (py / 8) * SCREEN_WIDTH;
      uint8_t bit_idx = py % 8;
      
      // Set or clear the bit
      if (lv_color_to1(c)) {
        framebuffer[byte_idx] |= (1 << bit_idx);   // Set bit (white)
      } else {
        framebuffer[byte_idx] &= ~(1 << bit_idx);  // Clear bit (black)
      }
    }
  }
  
  // Now send the ENTIRE framebuffer to the display
  display.clearDisplay();
  
  // Copy framebuffer to display buffer
  for (int y = 0; y < SCREEN_HEIGHT; y++) {
    for (int x = 0; x < SCREEN_WIDTH; x++) {
      uint16_t byte_idx = x + (y / 8) * SCREEN_WIDTH;
      uint8_t bit_idx = y % 8;
      
      if (framebuffer[byte_idx] & (1 << bit_idx)) {
        display.drawPixel(x, y, SSD1306_WHITE);
      }
    }
  }
  
  display.display();
  
  // Tell LVGL we're ready
  lv_disp_flush_ready(disp);
}

void createUI() {
  // Get active screen
  lv_obj_t *scr = lv_scr_act();
  
  // Set black background
  lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
  lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
  
  // Create title label
  lv_obj_t *label = lv_label_create(scr);
  lv_label_set_text(label, "LVGL Working!");
  lv_obj_align(label, LV_ALIGN_CENTER, 0, -20);
  lv_obj_set_style_text_color(label, lv_color_white(), 0);
  
  // Create counter label
  lv_obj_t *counter_label = lv_label_create(scr);
  lv_label_set_text(counter_label, "Count: 0");
  lv_obj_align(counter_label, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_text_color(counter_label, lv_color_white(), 0);
  
  // Create status label
  lv_obj_t *status_label = lv_label_create(scr);
  lv_label_set_text(status_label, "Refresh: OK");
  lv_obj_align(status_label, LV_ALIGN_CENTER, 0, 20);
  lv_obj_set_style_text_color(status_label, lv_color_white(), 0);
  
  // Store counter label reference for loop updates
  lv_obj_set_user_data(scr, counter_label);
  
  Serial.println("✓ UI Created");
}

void loop() {
  static unsigned long lastPrint = 0;
  static int counter = 0;
  static unsigned long lastUpdate = 0;
  
  // Update LVGL tick (CRITICAL)
  unsigned long now = millis();
  lv_tick_inc(now - lastTick);
  lastTick = now;
  
  // Handle LVGL tasks (CRITICAL)
  lv_timer_handler();
  
  // Update counter every second
  if (now - lastUpdate >= 1000) {
    counter++;
    
    // Get counter label and update it
    lv_obj_t *scr = lv_scr_act();
    lv_obj_t *counter_label = (lv_obj_t*)lv_obj_get_user_data(scr);
    if (counter_label) {
      char buf[32];
      snprintf(buf, sizeof(buf), "Count: %d", counter);
      lv_label_set_text(counter_label, buf);
      
      Serial.print("Counter: ");
      Serial.println(counter);
    }
    
    lastUpdate = now;
  }
  
  // Status print every 5 seconds
  if (now - lastPrint >= 5000) {
    Serial.print("Loop active - Counter: ");
    Serial.print(counter);
    Serial.print(" - Free mem: ");
    Serial.println(ESP.getFreeHeap());
    lastPrint = now;
  }
  
  delay(5);
}