// ============================================================
// SPORO - ESP32 với WiFiManager (tự hỏi WiFi, không cần sửa code)
//
// Lần đầu (hoặc khi không tìm thấy WiFi cũ): ESP32 phát ra một WiFi
// tên "SPORO-Setup". Người dùng dùng điện thoại kết nối vào đó, một
// trang cài đặt hiện ra để chọn WiFi nhà + nhập mật khẩu. Lưu xong
// ESP32 tự kết nối và ghi nhớ cho các lần sau.
//
// >>> CẦN CÀI THƯ VIỆN TRƯỚC KHI BIÊN DỊCH <<<
// Arduino IDE → Tools → Manage Libraries → tìm "WiFiManager"
// → cài bản của tác giả "tzapu".
// ============================================================

#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiManager.h>   // tzapu/WiFiManager
#include <Preferences.h>   // lưu cấu hình vào flash
#include <math.h>

// --- Địa chỉ server mặc định (có thể đổi ngay trên trang cài đặt) ---
const char* DEFAULT_SERVER = "https://esp32-plant-ghhm.onrender.com/api/update";
char cloudServer[128];

// --- Chân cảm biến ---
const int moisturePin = 34; // cảm biến độ ẩm đất (analog)
const int tempPin = 35;     // cảm biến nhiệt (thermistor, analog)

// --- Nút BOOT (GPIO0) để mở lại trang cài đặt khi cần đổi WiFi ---
const int RESET_BTN = 0;

Preferences prefs;
bool shouldSaveConfig = false;
void saveConfigCallback() { shouldSaveConfig = true; }

// Thermistor: đổi giá trị ADC sang nhiệt độ (°C)
float readTemperature(int tempValue) {
    const float R_FIXED = 10000.0;
    const float R0 = 10000.0;
    const float BETA = 3950.0;
    const float T0 = 298.15;

    if (tempValue >= 4095) tempValue = 4094;
    if (tempValue <= 0) tempValue = 1;

    float resistance = R_FIXED * (float)tempValue / (4095.0 - tempValue);
    float temperature = 1.0 / ((log(resistance / R0) / BETA) + (1.0 / T0));
    temperature -= 273.15;
    temperature += -6.5; // hiệu chỉnh (calibration) — chỉnh theo phòng bạn
    return temperature;
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    pinMode(RESET_BTN, INPUT_PULLUP);

    // Lấy server URL đã lưu (nếu chưa có thì dùng mặc định)
    prefs.begin("sporo", false);
    String saved = prefs.getString("server", DEFAULT_SERVER);
    saved.toCharArray(cloudServer, sizeof(cloudServer));

    WiFiManager wm;

    // Thêm ô nhập server URL vào trang cài đặt (tùy chọn)
    WiFiManagerParameter custom_server("server", "Link server (/api/update)", cloudServer, 128);
    wm.addParameter(&custom_server);
    wm.setSaveConfigCallback(saveConfigCallback);
    wm.setConfigPortalTimeout(180); // trang cài đặt mở tối đa 3 phút

    // Giữ nút BOOT khi bật nguồn => xóa WiFi cũ, buộc mở lại trang cài đặt
    if (digitalRead(RESET_BTN) == LOW) {
        Serial.println("Nut BOOT duoc giu -> xoa WiFi cu, mo trang cai dat...");
        wm.resetSettings();
    }

    Serial.println("\nDang ket noi WiFi...");
    Serial.println("Neu chua co WiFi: dien thoai ket noi vao mang 'SPORO-Setup' (mat khau: sporo1234)");

    // Tự kết nối; nếu chưa có/không được thì phát AP "SPORO-Setup"
    if (!wm.autoConnect("SPORO-Setup", "sporo1234")) {
        Serial.println("Khong ket noi duoc. Khoi dong lai...");
        delay(3000);
        ESP.restart();
    }

    // Lưu server URL nếu người dùng vừa nhập mới
    if (shouldSaveConfig) {
        strncpy(cloudServer, custom_server.getValue(), sizeof(cloudServer));
        prefs.putString("server", cloudServer);
        Serial.println("Da luu server URL moi.");
    }

    Serial.println("WiFi da ket noi!");
    Serial.print("IP ESP32: ");
    Serial.println(WiFi.localIP());
    Serial.print("Gui du lieu toi: ");
    Serial.println(cloudServer);
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;

        // Đọc cảm biến
        int tempRaw = analogRead(tempPin);
        int moistureRaw = analogRead(moisturePin);

        // Đổi độ ẩm sang % (chỉnh 2 số 2525/1000 theo sensor khô/ướt của bạn)
        int moisturePercent = map(moistureRaw, 2525, 1000, 0, 100);
        moisturePercent = constrain(moisturePercent, 0, 100);
        float temperature = readTemperature(tempRaw);

        // Ghép link: .../api/update?temp=28.5&moisture=65
        String url = String(cloudServer) + "?temp=" + String(temperature, 1) +
                     "&moisture=" + String(moisturePercent);

        Serial.print("Gui du lieu... ");
        http.begin(url);
        int code = http.GET();
        if (code > 0) {
            Serial.print("Server tra ve: ");
            Serial.println(code); // 200 = thanh cong
        } else {
            Serial.print("Loi gui. Ma: ");
            Serial.println(code);
        }
        http.end();
    } else {
        Serial.println("Mat WiFi! Dang ket noi lai...");
        WiFi.reconnect();
    }

    delay(20000); // gửi mỗi 20 giây
}
