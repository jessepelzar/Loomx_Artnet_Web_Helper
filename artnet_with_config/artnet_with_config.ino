#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArtnetWifi.h>
#include <FastLED.h>
#include <Preferences.h>
#include <vector>

//Wifi settings - be sure to replace these with the WiFi network that your computer is connected to

//const char* ssid = "Verizon_73MLNY";
//const char* password = "squad4-rift-hod";

//const char* ssid = "GL-MT300N-V2-dc8";
//const char* password = "goodlife";

//const char* ssid = "MyAtlice 3f1615";
//const char* password = "37-pink-2045";
Preferences preferences;
const char* ssid = "MyAltice 3f1615";
const char* password = "37-pink-2045";

// LED Strip
const int numLeds = 480; // Change if your setup has more or less LED's
const int numberOfChannels = numLeds * 3; // Total number of DMX channels you want to receive (1 led = 3 channels)
#define DATA_PIN 32 //The data pin that the WS2812 strips are connected to.
CRGB leds[numLeds];

// Artnet settings
ArtnetWifi artnet;
const int startUniverse = 0;

bool sendFrame = 1;
int previousDataLength = 0;

// connect to wifi – returns true if successful or false if not
boolean ConnectWifi(void)
{
  boolean state = true;
  int i = 0;

  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 20){
      state = false;
      break;
    }
    i++;
  }
  if (state){
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("");
    Serial.println("Connection failed.");
  }

  return state;
}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
  sendFrame = 1;
  // set brightness of the whole strip 
  if (universe == 15)
  {
    FastLED.setBrightness(data[0]);
  }
  // read universe and put into the right part of the display buffer
  for (int i = 0; i < length / 3; i++)
  {
    int led = i + (universe - startUniverse) * (previousDataLength / 3);
    if (led < numLeds)
    {
      leds[led] = CRGB(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
      Serial.println("data");
    }
  }
  previousDataLength = length;     
  FastLED.show();
}

void printPreferences() {
    preferences.begin("artnet_config", true); // true means "read-only mode"
    
    // Read and print each key-value pair
    String ssid = preferences.getString("SSID", "N/A"); // "N/A" is the default value if the key doesn't exist
    String password = preferences.getString("PASSWORD", "N/A");
    unsigned int numLeds = preferences.getUInt("NUM_LEDS", 0);
    String colorOrder = preferences.getString("COLOR_ORDER", "N/A");
    String ledType = preferences.getString("LED_TYPE", "N/A");
    unsigned int universe = preferences.getUInt("UNIVERSE", 0);

    // Print the values
    Serial.println("Stored Preferences:");
    Serial.println("SSID: " + ssid);
    Serial.println("PASSWORD: " + password);
    Serial.println("NUM_LEDS: " + String(numLeds));
    Serial.println("COLOR_ORDER: " + colorOrder);
    Serial.println("LED_TYPE: " + ledType);
    Serial.println("UNIVERSE: " + String(universe));

    preferences.end();
}


// This function splits a string based on a delimiter and returns the result as a vector of strings.
std::vector<String> splitString(const String &s, char delimiter) {
    std::vector<String> result;
    int startIndex = 0;
    int endIndex = 0;

    while (endIndex != -1) {
        endIndex = s.indexOf(delimiter, startIndex);
        result.push_back(s.substring(startIndex, endIndex));
        startIndex = endIndex + 1;
    }

    return result;
}


void getCommand() {
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        Serial.println("Received Command: " + command);
        // command = "SSID:sample_ssid,PASSWORD:sample_password,NUM_LEDS:100,COLOR_ORDER:RGB,LED_TYPE:WS2812B,UNIVERSE:1";
        command.trim();  // Remove any trailing whitespace
        Serial.println("got data");
        std::vector<String> commands = splitString(command, ',');
        for (int i = 0; i < commands.size(); i++) {
            std::vector<String> keyValue = splitString(commands[i], ':');
            if (keyValue.size() == 2) {
                storeData(keyValue[0], keyValue[1]);
                Serial.println("data loop");
            }
        }
    }
}


void storeData(String key, String data) {
    preferences.begin("artnet_config", false);
    if (key == "SSID" || key == "PASSWORD" || key == "COLOR_ORDER" || key == "LED_TYPE") {
        preferences.putString(key.c_str(), data);
        Serial.println(key + " Updated");
    } else if (key == "NUM_LEDS" || key == "UNIVERSE") {
        preferences.putUInt(key.c_str(), data.toInt());
        Serial.println(key + " Updated");
    }
    String storedValue = preferences.getString(key.c_str(), "ERROR");
    Serial.println(key + " Stored as: " + storedValue);

    // if (preferences.putString(key.c_str(), data) == ESP_ERR_NVS_INVALID_HANDLE) {
    //     Serial.println("Failed to store " + key);
    // }
    preferences.end();
}


void setup() {
    Serial.begin(115200);
    ConnectWifi();
    artnet.begin();
    FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, numLeds);
    artnet.setArtDmxCallback(onDmxFrame);

    printPreferences(); // Print stored preferences
}


void loop()
{
  // we call the read function inside the loop
  artnet.read();
}