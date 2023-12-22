#include "WifiCam.hpp"
#include <WiFi.h>
#include <HTTPClient.h>

static const char* WIFI_SSID = "SEA-IC";
static const char* WIFI_PASS = "seaic2022";
static const char* GOOGLE_DRIVE_UPLOAD_URL = "https://www.googleapis.com/upload/drive/v3/files?uploadType=multipart";

esp32cam::Resolution initialResolution;

WebServer server(80);

void setup()
{
  Serial.begin(115200);
  Serial.println();
  delay(2000);

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi failure");
    delay(5000);
    ESP.restart();
  }
  Serial.println("WiFi connected");

  {
    using namespace esp32cam;

    initialResolution = Resolution::find(1024, 768);

    Config cfg;
    cfg.setPins(pins::AiThinker);
    cfg.setResolution(initialResolution);
    cfg.setJpeg(80);

    bool ok = Camera.begin(cfg);
    if (!ok) {
      Serial.println("camera initialize failure");
      delay(5000);
      ESP.restart();
    }
    Serial.println("camera initialize success");
  }

  Serial.println("camera starting");
  Serial.print("http://");
  Serial.println(WiFi.localIP());

  addRequestHandlers();
  server.begin();
}

void loop()
{
  server.handleClient();
}

void addRequestHandlers()
{
  server.on("/", HTTP_GET, handleRoot);
  server.on("/upload", HTTP_POST, handleUpload);
}

void handleRoot()
{
  server.send(200, "text/html", "<form method='post' action='/upload' enctype='multipart/form-data'><input type='file' name='image' accept='image/*'><input type='submit' value='Upload'></form>");
}

void handleUpload()
{
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    Serial.println("Start uploading");
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    // Here you can process the uploaded image data
    Serial.println("Writing...");
  } else if (upload.status == UPLOAD_FILE_END) {
    // Image upload complete, you can add your Google Drive upload code here
    Serial.println("Upload finished");

    // Call the function to upload to Google Drive
    if (uploadToGoogleDrive(upload.filename, upload.totalSize)) {
      Serial.println("Upload to Google Drive successful");
    } else {
      Serial.println("Upload to Google Drive failed");
    }
  }
}

bool uploadToGoogleDrive(const char* filename, size_t fileSize)
{
  // Replace with your Google Drive API credentials and token
  const char* accessToken = "YOUR_ACCESS_TOKEN";
  const char* folderId = "YOUR_FOLDER_ID";  // Optional, specify the folder where you want to upload the file

  HTTPClient http;

  // Set authorization header
  String authorizationHeader = "Bearer " + String(accessToken);
  http.begin(GOOGLE_DRIVE_UPLOAD_URL);
  http.addHeader("Authorization", authorizationHeader);
  http.addHeader("Content-Type", "multipart/related; boundary=foo_bar_baz");

  // Construct the multipart request
  String multipartRequest = "--foo_bar_baz\r\n";
  multipartRequest += "Content-Type: application/json; charset=UTF-8\r\n\r\n";
  multipartRequest += "{\"name\":\"" + String(filename) + "\", \"parents\":[\"" + String(folderId) + "\"]}\r\n";
  multipartRequest += "--foo_bar_baz\r\n";
  multipartRequest += "Content-Type: image/jpeg\r\n\r\n";

  // Read image data and append it to the request
  for (size_t i = 0; i < fileSize; i++) {
    char c = readImageDataFromCamera();  // Replace with your method to read image data
    multipartRequest += c;
  }

  multipartRequest += "\r\n--foo_bar_baz--\r\n";

  // Send the request
  int httpResponseCode = http.POST(multipartRequest);
  http.end();

  return httpResponseCode == 200;
}

char readImageDataFromCamera()
{
  // Replace this with your method to read image data from the camera
  // You might need to use Camera.read(), Camera.readBytes(), or similar methods
  // to fetch image data from the camera module.
  // Ensure to return one character at a time.
  // For simplicity, this example returns a placeholder character 'A'.
  return 'A';
}
