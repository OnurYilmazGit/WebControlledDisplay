#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <TFT_eSPI.h>               // Hardware-specific library
#include <TJpg_Decoder.h>
#include "FS.h"
#include "LittleFS.h"

TFT_eSPI tft = TFT_eSPI();         // Invoke custom library
AsyncWebServer server(80);

const char* ssid = "FRITZ!Box 6660 Cable IH";
const char* password = "39188923097050675231";

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
  if (y >= tft.height()) return 0; // Stop further decoding as image is running off bottom of screen
  tft.pushImage(x, y, w, h, bitmap); // Clip the image block rendering automatically at the TFT boundaries
  return 1; // Return 1 to decode next block
}

void clearDisplay(AsyncWebServerRequest *request) {
    tft.fillScreen(TFT_BLACK); // Clear the screen
    request->send(200, "text/plain", "Display cleared");
}

  
int determineFontSize(const String& text) {
  // Dynamically adjust font size based on text length
  int screenArea = tft.width() * tft.height();
  int textArea = text.length() * 7 * 14; // Approx. area per char for smallest font

  if (textArea < screenArea / 4) return 4; // Use larger font for short text
  else if (textArea < screenArea / 2) return 2; // Medium font for moderate text
  else return 1; // Small font for long text
}

void displayText(const String& text) {
  int fontSize = determineFontSize(text); // Determine optimal font size
  tft.fillScreen(TFT_BLACK); // Clear the screen
  tft.setCursor(0, 0); // Start from top-left corner

  switch(fontSize) {
    case 4: tft.setTextFont(4); tft.setTextSize(2); break; // Large text
    case 2: tft.setTextFont(2); tft.setTextSize(1); break; // Medium text
    case 1: default: tft.setTextFont(1); tft.setTextSize(1); // Small text
  }

  tft.setTextColor(TFT_WHITE, TFT_BLACK); // Set text color
  tft.println(text); // Print the text
}


void displayRedCross() {
    tft.fillScreen(TFT_BLACK); // Clear the screen first
    tft.drawLine(0, 0, tft.width(), tft.height(), TFT_RED); // Top-left to bottom-right
    tft.drawLine(tft.width(), 0, 0, tft.height(), TFT_RED); // Top-right to bottom-left

    // To make the cross appear bolder, draw additional lines around the primary lines
    // Adjust the 'offset' to change the boldness
    int offset = 1; // Increase for a bolder line
    for(int i = 1; i <= offset; i++) {
        // Parallel lines for top-left to bottom-right
        tft.drawLine(0, i, tft.width(), tft.height() + i, TFT_RED);
        tft.drawLine(i, 0, tft.width() + i, tft.height(), TFT_RED);

        // Parallel lines for top-right to bottom-left
        tft.drawLine(tft.width() - i, 0, -i, tft.height(), TFT_RED);
        tft.drawLine(tft.width(), i, 0, tft.height() + i, TFT_RED);
    }
}


void handleTextDisplay(AsyncWebServerRequest *request) {
  if (!request->hasArg("plain")) { // Corrected method call
    request->send(400, "text/plain", "Text not found");
    return;
  }

  String message = request->arg("plain"); // Corrected method call
  displayText(message);
  request->send(200, "text/plain", "Text displayed");
}


void setup() {
  Serial.begin(115200);
  Serial.println("\n\nTesting TJpg_Decoder library");

  if (!LittleFS.begin()) {
    Serial.println("LittleFS Mount Failed, formatting...");
    LittleFS.format();
    if (!LittleFS.begin()) {
      Serial.println("Failed to format and mount LittleFS");
      return;
    }
  }
  
  Serial.println("\r\nInitialization done.");

  tft.begin();
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true); // Swap the colour bytes (endianess)

  TJpgDec.setJpgScale(2);
  TJpgDec.setCallback(tft_output);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "File upload successful.");
  }, [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
    static File uploadFile;
    if (!index) {
      Serial.println("Upload Start: " + filename);
      uploadFile = LittleFS.open("/" + filename, "w");
    }
    if (uploadFile) {
      uploadFile.write(data, len);
      if (final) {
        Serial.println("Upload Complete: " + filename);
        uploadFile.close();
        server.on("/clearDisplay", HTTP_GET, clearDisplay);
        TJpgDec.drawFsJpg(0, 0, ("/" + filename).c_str(), LittleFS); // Display the JPEG after upload
      }
    }
  });

  server.on("/list", HTTP_GET, [](AsyncWebServerRequest *request) {
    String output = "Files:\n";
    File root = LittleFS.open("/");
    File file = root.openNextFile();
    while(file) {
      output += String(file.name()) + "\n";
      file = root.openNextFile();
    }
    request->send(200, "text/plain", output);
  });

  server.on("/display", HTTP_POST, [](AsyncWebServerRequest *request) {
  handleTextDisplay(request); });

  // Add a new handler for drawing the red cross
  server.on("/cross", HTTP_GET, [](AsyncWebServerRequest *request) {
      displayRedCross();
      request->send(200, "text/plain", "Red cross displayed");
  });

  server.on("/clearDisplay", HTTP_GET, [](AsyncWebServerRequest *request) {
    clearDisplay(request);
});


  server.begin();
}

void loop() {
}
