#include <WiFi.h>
#include <PubSubClient.h>

// Network Credentials
#define WIFI_SSID "wifi"
#define WIFI_PASSWORD "pass"

// MQTT Broker Settings
#define MQTT_BROKER "broker.emqx.io"
#define MQTT_USERNAME "mqtt-user"
#define MQTT_PASSWORD "mqtt-user"
#define MQTT_TOPIC "inTopic"

WiFiClient espClient;
PubSubClient mqttClient(espClient);

// LED Matrix Configurations
#define NUM_ROW 7
#define NUM_COL 60 // Adjusted to a consistent naming and calculation
#define NUM_LEDS (NUM_ROW * NUM_COL) 

#define R1_PIN 13
#define R2_PIN 23
#define R3_PIN 14
#define R4_PIN 27
#define R5_PIN 26
#define R6_PIN 25
#define R7_PIN 16
#define CLOCK_PIN 15
#define DATA_PIN 4

uint8_t og_matrix[NUM_ROW][NUM_COL];
uint8_t led_matrix[NUM_ROW][NUM_COL];

int textPosition; // Position for text animation
int maxTextColumns; // Maximum columns for the text
int animationSpeed;

unsigned short Alphabets[][5]={ 
0x7e, 0x09, 0x09, 0x09, 0x7e, // A
0x7f, 0x49, 0x49, 0x49, 0x36,  // B
0x3e, 0x41, 0x41, 0x41, 0x22,
0x7f, 0x41, 0x41,0x22, 0x1c,
0x7f, 0x49, 0x49, 0x49, 0x63,
0x7f, 0x09, 0x09, 0x09, 0x01,
0x3e, 0x41, 0x41, 0x49, 0x7a,
0x7f, 0x08, 0x08, 0x08, 0x7f,
0x00, 0x41, 0x7f, 0x41, 0x00,  // I
0x20, 0x40, 0x41, 0x3f, 0x01,
0x7f, 0x08, 0x14, 0x22, 0x41,
0x7f, 0x40, 0x40, 0x40, 0x60,
0x7f, 0x02, 0x04, 0x02, 0x7f,
0x7f, 0x04, 0x08, 0x10, 0x7f,
0x3e, 0x41, 0x41, 0x41, 0x3e,
0x7f, 0x09, 0x09, 0x09, 0x06,
0x3e, 0x41, 0x51, 0x21, 0x5e,
0x7f, 0x09, 0x19, 0x29, 0x46,
0x46, 0x49, 0x49, 0x49, 0x31,  // S
0x01, 0x01, 0x7f, 0x01, 0x01,
0x3f, 0x40, 0x40, 0x40, 0x3f,
0x1f, 0x20, 0x40, 0x20, 0x1f,
0x3f, 0x40, 0x30, 0x40, 0x3f,
0x63, 0x14, 0x08, 0x14, 0x63,
0x07, 0x08, 0x70, 0x08, 0x07,
0x61, 0x51, 0x49, 0x45, 0x430, // Z
0x3E, 0x41, 0x41, 0x41, 0x3E,   // a
0x7F, 0x49, 0x49, 0x49, 0x36,   // b
0x3E, 0x41, 0x41, 0x41, 0x22,   // c
0x7F, 0x41, 0x41, 0x22, 0x1C,   // d
0x7F, 0x49, 0x49, 0x49, 0x41,   // e
0x7F, 0x48, 0x48, 0x48, 0x40,   // f
0x3E, 0x41, 0x49, 0x49, 0x3A,   // g
0x7F, 0x08, 0x08, 0x08, 0x7F,   // h
0x00, 0x41, 0x7F, 0x41, 0x00,   // i
0x20, 0x40, 0x41, 0x3F, 0x01,   // j
0x7F, 0x08, 0x14, 0x22, 0x41,   // k
0x7F, 0x01, 0x01, 0x01, 0x01,   // l
0x7F, 0x20, 0x10, 0x20, 0x7F,   // m
0x7F, 0x10, 0x08, 0x04, 0x7F,   // n
0x3E, 0x41, 0x41, 0x41, 0x3E,   // o
0x7F, 0x48, 0x48, 0x48, 0x30,   // p
0x3E, 0x41, 0x45, 0x42, 0x3D,   // q
0x7F, 0x48, 0x4C, 0x4A, 0x31,   // r
0x32, 0x49, 0x49, 0x49, 0x26,   // s
0x40, 0x40, 0x7F, 0x40, 0x40,   // t
0x7E, 0x01, 0x01, 0x01, 0x7E,   // u
0x7C, 0x02, 0x01, 0x02, 0x7C,   // v
0x7E, 0x01, 0x0E, 0x01, 0x7E,   // w
0x63, 0x14, 0x08, 0x14, 0x63,   // x
0x60, 0x10, 0x0F, 0x10, 0x60,   // y
0x43, 0x45, 0x49, 0x51, 0x61,   // z
0x3E, 0x51, 0x49, 0x45, 0x3E,   // 0
0x00, 0x42, 0x7F, 0x40, 0x00,   // 1
0x42, 0x61, 0x51, 0x49, 0x46,   // 2
0x21, 0x41, 0x45, 0x4B, 0x31,   // 3
0x18, 0x14, 0x12, 0x7F, 0x10,   // 4
0x27, 0x45, 0x45, 0x45, 0x39,   // 5
0x3C, 0x4A, 0x49, 0x49, 0x30,   // 6
0x01, 0x71, 0x09, 0x05, 0x03,   // 7
0x36, 0x49, 0x49, 0x49, 0x36,   // 8
0x06, 0x49, 0x49, 0x29, 0x1E,   // 9
0x00, 0x00, 0x00, 0x00, 0x00,   // (space)
0x00, 0x00, 0x5F, 0x00, 0x00,   // !
0x00, 0x07, 0x00, 0x07, 0x00,   // "
0x14, 0x7F, 0x14, 0x7F, 0x14,   // #
0x24, 0x2A, 0x7F, 0x2A, 0x12,   // $
0x23, 0x13, 0x08, 0x64, 0x62,   // %
0x36, 0x49, 0x55, 0x22, 0x50,   // &
0x00, 0x05, 0x03, 0x00, 0x00,   // '
0x00, 0x1C, 0x22, 0x41, 0x00,   // (
0x00, 0x41, 0x22, 0x1C, 0x00,   // )
0x14, 0x08, 0x3E, 0x08, 0x14,   // *
0x08, 0x08, 0x3E, 0x08, 0x08,   // +
0x00, 0x50, 0x30, 0x00, 0x00,   // ,
0x08, 0x08, 0x08, 0x08, 0x08,   // -
0x00, 0x60, 0x60, 0x00, 0x00,   // .
0x20, 0x10, 0x08, 0x04, 0x02,   // /
};


// Forward declarations
void connectToWifi();
void connectToMQTT();
void mqttCallback(char* topic, byte* payload, unsigned int length);
void animateText();
void clearLEDMatrix();
int calculateRowPin(uint8_t row);

void setup() {
  Serial.begin(115200);

  pinMode(R1_PIN, OUTPUT); // R61
  pinMode(R2_PIN, OUTPUT); // R62
  pinMode(R3_PIN, OUTPUT); // R63
  pinMode(R4_PIN, OUTPUT); // R64
  pinMode(R5_PIN, OUTPUT); // R65
  pinMode(R6_PIN, OUTPUT); // R66
  pinMode(R7_PIN, OUTPUT); // R67

  pinMode(CLOCK_PIN, OUTPUT); // CLK 10
  pinMode(DATA, OUTPUT); // U2B 11

  digitalWrite(CLOCK_PIN, LOW);
  digitalWrite(R1_PIN, HIGH);
  digitalWrite(R2_PIN, HIGH);
  digitalWrite(R3_PIN, HIGH);
  digitalWrite(R4_PIN, HIGH);
  digitalWrite(R5_PIN, HIGH);
  digitalWrite(R6_PIN, HIGH);
  digitalWrite(R7_PIN, HIGH);

  clean_matrix();
  
  connectToWifi();
  mqttClient.setServer(MQTT_BROKER, 1883);
  mqttClient.setCallback(mqttCallback);

  // LED matrix pin setup code
}

void loop() {
  if (!mqttClient.connected()) {
    connectToMQTT();
  }
  mqttClient.loop();
}

void connectToWifi() {
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void connectToMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect("ESP32Client", MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.println("connected");
      mqttClient.subscribe(MQTT_TOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// MQTT callback function processes incoming messages and prepares them for display
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  char message[100]; // Adjust size according to your expected maximum message length
  for (unsigned int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
    Serial.print(message[i]);
  }
  message[length] = '\0'; // Null-terminate the message
  Serial.println();

  // Prepare the message for display
  createText(message);
  animationSpeed = 5; // Adjust speed as needed
  animateText(); // This may need adjustment to fit within your application's structure
}

void createText(const char* text) {
  memset(og_matrix, 0, sizeof(og_matrix)); // Clear the original matrix
  int pos = 0; // Position in the og_matrix

  // Iterate over each character in the text
  for (size_t i = 0; i < strlen(text); i++) {
    int alphabetIndex = toupper(text[i]) - 'A';
    if (alphabetIndex < 0 || alphabetIndex > 25) continue; // Skip non-alphabet characters

    // Copy character bitmap to og_matrix
    for (int col = 0; col < 5; col++) { // Character width is 5
      for (int row = 0; row < 7; row++) { // Character height is 7
        int bitVal = (Alphabets[alphabetIndex][col] >> row) & 1;
        og_matrix[row][pos + col] = bitVal;
      }
    }
    pos += 6; // Move to the next character position, including space
  }

  maxTextColumns = pos; // Update the maximum text columns based on the text length
}

void animateText() {
  for (textPosition = 0; textPosition < maxTextColumns + NUM_COL; textPosition++) {
    // Shift text left by one column
    for (int row = 0; row < NUM_ROW; row++) {
      for (int col = 0; col < NUM_COL; col++) {
        int sourceCol = textPosition + col - NUM_COL;
        led_matrix[row][col] = (sourceCol >= 0 && sourceCol < maxTextColumns) ? og_matrix[row][sourceCol] : 0;
      }
    }
    
    // Draw the current frame of the text animation
    drawMatrix();
    delay(animationSpeed * 10); // Adjust delay to control the speed of the animation
  }
}

void drawMatrix() {
  for (uint8_t row = 0; row < NUM_ROW; row++) {
    enableRow(row);
    for (uint8_t col = 0; col < NUM_COL; col++) {
      writePixel(led_matrix[row][col]);
    }
    disableRow(row);
  }
}

void enableRow(uint8_t row) {
  digitalWrite(calculateRowPin(row), LOW); // Assuming LOW enables the row
}

void disableRow(uint8_t row) {
  digitalWrite(calculateRowPin(row), HIGH); // Assuming HIGH disables the row
}

void writePixel(uint8_t value) {
  digitalWrite(DATA_PIN, value);
  digitalWrite(CLOCK_PIN, HIGH);
  digitalWrite(CLOCK_PIN, LOW); // Clock the data in
}

int calculateRowPin(uint8_t row) {
  switch (row) {
    case 0: return R1_PIN;
    case 1: return R2_PIN;
    case 2: return R3_PIN;
    case 3: return R4_PIN;
    case 4: return R5_PIN;
    case 5: return R6_PIN;
    case 6: return R7_PIN;
    default: return -1; // Invalid row
  }
}
