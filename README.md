# Web Controlled Display

This project is a web-controlled display using ESP32 and TFT display. It allows you to upload images, display text, and draw a red cross on the display. The project uses the ESPAsyncWebServer library to handle HTTP requests and the LittleFS library to handle file storage on the ESP32.

## Prerequisites

- ESP32 development board with TFT display
- Arduino IDE with ESP32 board support installed

## Setup

1. Open the `WebControlledDisplay.ino` file in the Arduino IDE.
2. Edit the following lines with your WiFi network settings:
    - Replace `"FRITZ!Box 6660 Cable IH"` with your WiFi network name (SSID).
    - Replace `"39188923097050675231"` with your WiFi password.
3. Install the necessary libraries. In the Arduino IDE, go to `Sketch -> Include Library -> Manage Libraries`. Search for and install the following libraries:
    - ESPAsyncWebServer
    - LittleFS for ESP32
    - TFT_eSPI
    - TJpg_Decoder

## Upload and Run

1. Connect your ESP32 board to your computer.
2. Select the correct board and port in the Arduino IDE.
3. Click on the upload button to compile and upload the sketch to your ESP32 board.
4. Open the Serial Monitor (`Tools -> Serial Monitor`) and set the baud rate to 115200.
5. After the ESP32 connects to the WiFi network, it will print its IP address in the Serial Monitor.
6. Open a web browser and enter the IP address. You should see the web interface of your web-controlled display.

## Usage

- To upload an image, use the `/upload` endpoint. The image will be displayed on the TFT display after upload.
- To list all uploaded files, use the `/list` endpoint. This will return a list of all files stored on the ESP32.
- To display a text, send a POST request to the `/display` endpoint with the text in the request body.
- To draw a red cross on the display, use the `/cross` endpoint. This will draw a red cross on the display and return a confirmation message.
- To clear the display, use the `/clearDisplay` endpoint. This will clear the display and return a confirmation message.

## Code Explanation

The code sets up an asynchronous web server on the ESP32. It defines several endpoints for different functionalities. The `/list` endpoint lists all files stored on the ESP32. The `/display` endpoint displays a text sent in the request body. The `/cross` endpoint draws a red cross on the display. The `/clearDisplay` endpoint clears the display. The `loop()` function is empty because all the work is done in the background by the ESPAsyncWebServer library.

## Interacting with the Server using Curl

You can use the `curl` command-line tool to interact with the server endpoints. Here are some examples:

- To send a GET request to the `/list` endpoint:
  ```
  curl http://<your-server-ip>/list
  ```

- To send a POST request to the `/display` endpoint with plain text:
  ```
  curl -X POST -d "Hello, World!" http://<your-server-ip>/display
  ```

- To send a GET request to the `/cross` endpoint:
  ```
  curl http://<your-server-ip>/cross
  ```

- To send a GET request to the `/clearDisplay` endpoint:
  ```
  curl http://<your-server-ip>/clearDisplay
  ```

- To upload an image to the web-controlled display:
  ```
  curl -F "image=@/path/to/image.jpg" http://<your-server-ip>/upload
  ```
  Replace `<your-server-ip>` with the actual IP address of your server. The image will be uploaded and stored in the LittleFS filesystem of the ESP32, and then displayed on the TFT display.

## Example Image

![ESP32 Board](/Cleaned_Display.jpeg)
