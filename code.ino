#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SpotifyEsp32.h>

#define TFT_CS   5
#define TFT_DC   3
#define TFT_RST  2
#define TFT_SCLK 8
#define TFT_MOSI 10

#define BTN_PLAY 0
#define BTN_NEXT 1
#define BTN_PREV 4

char* SSID = "WIFI";
char* PASSWORD = "PASSWORD";

const char* CLIENT_ID = "CLIENT_ID";
const char* CLIENT_SECRET = "CLIENT_SECRET";

Spotify sp(CLIENT_ID, CLIENT_SECRET);
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

String lastTrack = "";
String lastArtist = "";

bool lastPlayState = HIGH;
bool lastNextState = HIGH;
bool lastPrevState = HIGH;

void setup() {
  Serial.begin(115200);

  pinMode(BTN_PLAY, INPUT_PULLUP);
  pinMode(BTN_NEXT, INPUT_PULLUP);
  pinMode(BTN_PREV, INPUT_PULLUP);

  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);

  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi Connected");

  tft.setCursor(0,0);
  tft.setTextSize(1);
  tft.write("WiFi OK");

  sp.begin();
  while (!sp.is_auth()) {
    sp.handle_client();
  }

  tft.setCursor(0,20);
  tft.write("Spotify OK");
}

void handleButtons() {
  bool playState = digitalRead(BTN_PLAY);
  bool nextState = digitalRead(BTN_NEXT);
  bool prevState = digitalRead(BTN_PREV);

  if (playState == LOW && lastPlayState == HIGH) {
    sp.start_resume_playback();
    Serial.println("Play/Pause");
    delay(200);
  }

  if (nextState == LOW && lastNextState == HIGH) {
    sp.skip();
    Serial.println("Next");
    delay(200);
  }

  if (prevState == LOW && lastPrevState == HIGH) {
    sp.previous();
    Serial.println("Previous");
    delay(200);
  }

  lastPlayState = playState;
  lastNextState = nextState;
  lastPrevState = prevState;
}

void updateDisplay() {
  String artist = sp.current_artist_names();
  String track = sp.current_track_name();

  if (artist != lastArtist || track != lastTrack) {
    tft.fillScreen(ST77XX_BLACK);

    tft.setCursor(0, 10);
    tft.setTextSize(1);
    tft.write(artist.c_str());

    tft.setCursor(0, 40);
    tft.setTextSize(2);
    tft.write(track.c_str());

    lastArtist = artist;
    lastTrack = track;
  }
}

void loop() {
  handleButtons();
  updateDisplay();

  delay(1000);
}
