//ESP MAIN Board CODE UPDATED
 
#include <WiFi.h>
#include <WebServer.h>
 
// ======================================================
// WIFI CONFIGURATION
// ======================================================
 
const char* ssid = "*********";
const char* password = "*********";
 
// ======================================================
// ESP32-CAM STREAM
// ======================================================
//
// ESP32-CAM IP:
// 10.141.205.90
//
// Standard CameraWebServer:
// Main webpage:
// http://10.141.205.90
//
// MJPEG Stream:
// http://10.141.205.90:81/stream
//
 
char cameraStream[128] =
  "http://172.19.220.90:81/stream";
 
 
// ======================================================
// L298N MOTOR DRIVER
// ======================================================
 
// LEFT MOTOR
#define IN1 26
#define IN2 27
 
// RIGHT MOTOR
#define IN3 14
#define IN4 13
 
// ENABLE / PWM
#define ENA 25
#define ENB 33
 
 
// ======================================================
// CAMERA PAN / TILT SERVOS
// ======================================================
 
#define PAN_PIN   18
#define TILT_PIN  19
 
 
// ======================================================
// MOTOR PWM
// ESP32 Arduino Core 3.x
// ======================================================
 
const int MOTOR_PWM_FREQ = 1000;
const int MOTOR_PWM_RES  = 8;
 
int motorSpeed = 220;
 
 
// ======================================================
// SERVO PWM
// ======================================================
 
const int SERVO_PWM_FREQ = 50;
const int SERVO_PWM_RES  = 14;
 
 
// ======================================================
// SERVO POSITION
// ======================================================
 
int panAngle  = 90;
int tiltAngle = 90;
 
const int SERVO_STEP = 10;
 
const int PAN_MIN = 10;
const int PAN_MAX = 170;
 
const int TILT_MIN = 20;
const int TILT_MAX = 160;
 
 
// ======================================================
// MOTOR SAFETY WATCHDOG
// ======================================================
 
unsigned long lastCommandTime = 0;
 
const unsigned long SAFETY_TIMEOUT_MS = 400;
 
bool isMoving = false;
 
 
// ======================================================
// WEB SERVER
// ======================================================
 
WebServer server(80);
 
 
// ======================================================
// SERVO CONTROL
// ======================================================
 
void writeServoAngle(int pin, int angle)
{
  angle = constrain(angle, 0, 180);
 
  /*
     14-bit PWM
     0 - 16383
 
     Approx servo pulse:
     0°   -> 500 us
     180° -> 2400 us
  */
 
  int duty = map(angle, 0, 180, 410, 1966);
 
  ledcWrite(pin, duty);
}
 
 
// ======================================================
// CAMERA MOVEMENT
// ======================================================
 
void cameraLeft()
{
  panAngle -= SERVO_STEP;
 
  panAngle = constrain(
    panAngle,
    PAN_MIN,
    PAN_MAX
  );
 
  writeServoAngle(
    PAN_PIN,
    panAngle
  );
}
 
 
void cameraRight()
{
  panAngle += SERVO_STEP;
 
  panAngle = constrain(
    panAngle,
    PAN_MIN,
    PAN_MAX
  );
 
  writeServoAngle(
    PAN_PIN,
    panAngle
  );
}
 
 
void cameraUp()
{
  tiltAngle -= SERVO_STEP;
 
  tiltAngle = constrain(
    tiltAngle,
    TILT_MIN,
    TILT_MAX
  );
 
  writeServoAngle(
    TILT_PIN,
    tiltAngle
  );
}
 
 
void cameraDown()
{
  tiltAngle += SERVO_STEP;
 
  tiltAngle = constrain(
    tiltAngle,
    TILT_MIN,
    TILT_MAX
  );
 
  writeServoAngle(
    TILT_PIN,
    tiltAngle
  );
}
 
 
void cameraCenter()
{
  panAngle = 90;
  tiltAngle = 90;
 
  writeServoAngle(
    PAN_PIN,
    panAngle
  );
 
  writeServoAngle(
    TILT_PIN,
    tiltAngle
  );
}
 
 
// ======================================================
// MOTOR SPEED
// ======================================================
 
void applySpeed(int speedVal)
{
  speedVal = constrain(
    speedVal,
    0,
    255
  );
 
  ledcWrite(
    ENA,
    speedVal
  );
 
  ledcWrite(
    ENB,
    speedVal
  );
}
 
 
// ======================================================
// STOP ROBOT
// ======================================================
 
void stopRobot()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
 
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
 
  applySpeed(0);
 
  isMoving = false;
}
 
 
// ======================================================
// FORWARD
// ======================================================
 
void forward()
{
  // LEFT
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
 
  // RIGHT
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
 
  applySpeed(motorSpeed);
 
  isMoving = true;
 
  lastCommandTime = millis();
}
 
 
// ======================================================
// BACKWARD
// ======================================================
 
void backward()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
 
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
 
  applySpeed(motorSpeed);
 
  isMoving = true;
 
  lastCommandTime = millis();
}
 
 
// ======================================================
// TURN LEFT
// ======================================================
 
void turnLeft()
{
  // LEFT backwards
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
 
  // RIGHT forwards
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
 
  applySpeed(motorSpeed);
 
  isMoving = true;
 
  lastCommandTime = millis();
}
 
 
// ======================================================
// TURN RIGHT
// ======================================================
 
void turnRight()
{
  // LEFT forwards
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
 
  // RIGHT backwards
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
 
  applySpeed(motorSpeed);
 
  isMoving = true;
 
  lastCommandTime = millis();
}
 
 
// ======================================================
// WEB PAGE
// ======================================================
 
const char INDEX_HTML[] PROGMEM =
  "<!DOCTYPE html>\n"
  "<html lang=\"en\">\n"
  "<head>\n"
  "  <meta charset=\"UTF-8\">\n"
  "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
  "  <title>CHIREC | ESP32 Surveillance Rover</title>\n"
  "\n"
  "  <style>\n"
  "    /* Basic page styling */\n"
  "    * {\n"
  "      box-sizing: border-box;\n"
  "    }\n"
  "\n"
  "    body {\n"
  "      margin: 0;\n"
  "      padding: 16px;\n"
  "      background: #080c11;\n"
  "      color: #f1f5f9;\n"
  "      font-family: Arial, Helvetica, sans-serif;\n"
  "    }\n"
  "\n"
  "    .container {\n"
  "      width: 100%;\n"
  "      max-width: 1200px;\n"
  "      margin: auto;\n"
  "      display: flex;\n"
  "      flex-direction: column;\n"
  "      gap: 14px;\n"
  "    }\n"
  "\n"
  "    /* Header */\n"
  "    header {\n"
  "      display: flex;\n"
  "      align-items: center;\n"
  "      justify-content: space-between;\n"
  "      gap: 16px;\n"
  "      padding: 14px 18px;\n"
  "      background: #111821;\n"
  "      border: 1px solid rgba(255,255,255,0.12);\n"
  "      border-radius: 12px;\n"
  "    }\n"
  "\n"
  "    .brand {\n"
  "      display: flex;\n"
  "      align-items: center;\n"
  "      gap: 16px;\n"
  "    }\n"
  "\n"
  "    .logo {\n"
  "      width: 150px;\n"
  "      height: 58px;\n"
  "      display: flex;\n"
  "      align-items: center;\n"
  "      justify-content: center;\n"
  "      background: white;\n"
  "      border-radius: 8px;\n"
  "      padding: 4px 8px;\n"
  "      overflow: hidden;\n"
  "      flex-shrink: 0;\n"
  "    }\n"
  "\n"
  "    .logo img {\n"
  "      display: block;\n"
  "      width: 100%;\n"
  "      height: 100%;\n"
  "      object-fit: contain;\n"
  "    }\n"
  "\n"
  "    h1 {\n"
  "      margin: 0;\n"
  "      color: #00e5ff;\n"
  "      font-size: clamp(17px, 2.5vw, 25px);\n"
  "      letter-spacing: 1px;\n"
  "    }\n"
  "\n"
  "    .status {\n"
  "      padding: 8px 12px;\n"
  "      border-radius: 20px;\n"
  "      color: #00e68a;\n"
  "      background: rgba(0,230,138,0.1);\n"
  "      border: 1px solid rgba(0,230,138,0.4);\n"
  "      font-size: 12px;\n"
  "      white-space: nowrap;\n"
  "    }\n"
  "\n"
  "    /* Main camera and control panels */\n"
  "    .main-layout {\n"
  "      display: grid;\n"
  "      grid-template-columns: minmax(0, 2fr) minmax(260px, 0.9fr);\n"
  "      gap: 14px;\n"
  "      align-items: stretch;\n"
  "    }\n"
  "\n"
  "    .panel {\n"
  "      min-width: 0;\n"
  "      padding: 16px;\n"
  "      background: #111821;\n"
  "      border: 1px solid rgba(255,255,255,0.12);\n"
  "      border-radius: 12px;\n"
  "    }\n"
  "\n"
  "    .panel-title {\n"
  "      margin-bottom: 14px;\n"
  "      color: #8491a3;\n"
  "      font-size: 13px;\n"
  "      font-weight: bold;\n"
  "      letter-spacing: 1px;\n"
  "      text-transform: uppercase;\n"
  "    }\n"
  "\n"
  "    .camera-container {\n"
  "      position: relative;\n"
  "      width: 100%;\n"
  "      aspect-ratio: 16 / 9;\n"
  "      min-height: 260px;\n"
  "      overflow: hidden;\n"
  "      display: flex;\n"
  "      align-items: center;\n"
  "      justify-content: center;\n"
  "      background: #000;\n"
  "      border: 1px solid rgba(255,255,255,0.12);\n"
  "      border-radius: 8px;\n"
  "    }\n"
  "\n"
  "    #cameraFrame {\n"
  "      width: 100%;\n"
  "      height: 100%;\n"
  "      object-fit: contain;\n"
  "      background: #000;\n"
  "    }\n"
  "\n"
  "    .camera-placeholder {\n"
  "      position: absolute;\n"
  "      padding: 12px;\n"
  "      text-align: center;\n"
  "      color: #8491a3;\n"
  "      pointer-events: none;\n"
  "    }\n"
  "\n"
  "    .crosshair {\n"
  "      position: absolute;\n"
  "      left: 50%;\n"
  "      top: 50%;\n"
  "      width: 40px;\n"
  "      height: 40px;\n"
  "      border: 1px solid rgba(0,229,255,0.55);\n"
  "      border-radius: 50%;\n"
  "      transform: translate(-50%, -50%);\n"
  "      pointer-events: none;\n"
  "    }\n"
  "\n"
  "    .crosshair::before,\n"
  "    .crosshair::after {\n"
  "      content: \"\";\n"
  "      position: absolute;\n"
  "      background: rgba(0,229,255,0.55);\n"
  "    }\n"
  "\n"
  "    .crosshair::before {\n"
  "      width: 58px;\n"
  "      height: 1px;\n"
  "      left: 50%;\n"
  "      top: 50%;\n"
  "      transform: translate(-50%, -50%);\n"
  "    }\n"
  "\n"
  "    .crosshair::after {\n"
  "      width: 1px;\n"
  "      height: 58px;\n"
  "      left: 50%;\n"
  "      top: 50%;\n"
  "      transform: translate(-50%, -50%);\n"
  "    }\n"
  "\n"
  "    .camera-message {\n"
  "      position: absolute;\n"
  "      left: 10px;\n"
  "      bottom: 10px;\n"
  "      padding: 6px 9px;\n"
  "      border-radius: 5px;\n"
  "      background: rgba(0,0,0,0.75);\n"
  "      color: #00e5ff;\n"
  "      font-family: monospace;\n"
  "      font-size: 11px;\n"
  "    }\n"
  "\n"
  "    /* Right-side controls */\n"
  "    .side-controls {\n"
  "      display: flex;\n"
  "      flex-direction: column;\n"
  "      gap: 14px;\n"
  "    }\n"
  "\n"
  "    .control-panel {\n"
  "      flex: 1;\n"
  "      display: flex;\n"
  "      flex-direction: column;\n"
  "      align-items: center;\n"
  "    }\n"
  "\n"
  "    .control-panel .panel-title {\n"
  "      width: 100%;\n"
  "    }\n"
  "\n"
  "    .dpad {\n"
  "      display: grid;\n"
  "      grid-template-columns: repeat(3, 64px);\n"
  "      grid-template-rows: repeat(3, 56px);\n"
  "      gap: 7px;\n"
  "      justify-content: center;\n"
  "    }\n"
  "\n"
  "    .control-btn {\n"
  "      display: grid;\n"
  "      place-items: center;\n"
  "      border: 1px solid rgba(255,255,255,0.15);\n"
  "      border-radius: 9px;\n"
  "      background: #1b2633;\n"
  "      color: white;\n"
  "      font-size: 23px;\n"
  "      cursor: pointer;\n"
  "      touch-action: none;\n"
  "      user-select: none;\n"
  "    }\n"
  "\n"
  "    .control-btn:hover {\n"
  "      border-color: #00e5ff;\n"
  "    }\n"
  "\n"
  "    .control-btn:active,\n"
  "    .control-btn.active {\n"
  "      background: #00e5ff;\n"
  "      color: #080c11;\n"
  "    }\n"
  "\n"
  "    .stop {\n"
  "      color: #ff3d63;\n"
  "      font-size: 11px;\n"
  "      font-weight: bold;\n"
  "    }\n"
  "\n"
  "    .stop:active {\n"
  "      background: #ff3d63;\n"
  "      color: white;\n"
  "    }\n"
  "\n"
  "    .camera-btn:active {\n"
  "      background: #a855f7;\n"
  "      color: white;\n"
  "    }\n"
  "\n"
  "    .hint {\n"
  "      margin-top: 12px;\n"
  "      color: #8491a3;\n"
  "      font-size: 11px;\n"
  "      text-align: center;\n"
  "    }\n"
  "\n"
  "    /* Bottom camera URL and speed settings */\n"
  "    .settings {\n"
  "      display: grid;\n"
  "      grid-template-columns: minmax(220px, 1fr) auto minmax(220px, 0.8fr);\n"
  "      align-items: center;\n"
  "      gap: 12px;\n"
  "      padding: 14px;\n"
  "      background: #111821;\n"
  "      border: 1px solid rgba(255,255,255,0.12);\n"
  "      border-radius: 12px;\n"
  "    }\n"
  "\n"
  "    .url-group,\n"
  "    .speed-group {\n"
  "      display: flex;\n"
  "      align-items: center;\n"
  "      gap: 9px;\n"
  "      min-width: 0;\n"
  "    }\n"
  "\n"
  "    label {\n"
  "      color: #8491a3;\n"
  "      font-size: 11px;\n"
  "      font-weight: bold;\n"
  "      white-space: nowrap;\n"
  "    }\n"
  "\n"
  "    input[type=\"text\"] {\n"
  "      width: 100%;\n"
  "      min-width: 0;\n"
  "      padding: 10px;\n"
  "      color: white;\n"
  "      background: #070a0f;\n"
  "      border: 1px solid rgba(255,255,255,0.16);\n"
  "      border-radius: 6px;\n"
  "    }\n"
  "\n"
  "    input[type=\"text\"]:focus {\n"
  "      outline: 1px solid #00e5ff;\n"
  "    }\n"
  "\n"
  "    .apply-btn {\n"
  "      padding: 10px 14px;\n"
  "      color: #00e5ff;\n"
  "      background: rgba(0,229,255,0.1);\n"
  "      border: 1px solid #00e5ff;\n"
  "      border-radius: 6px;\n"
  "      cursor: pointer;\n"
  "      font-weight: bold;\n"
  "      white-space: nowrap;\n"
  "    }\n"
  "\n"
  "    .apply-btn:hover {\n"
  "      background: rgba(0,229,255,0.2);\n"
  "    }\n"
  "\n"
  "    input[type=\"range\"] {\n"
  "      width: 100%;\n"
  "      accent-color: #00e5ff;\n"
  "      cursor: pointer;\n"
  "    }\n"
  "\n"
  "    #speedValue {\n"
  "      min-width: 28px;\n"
  "      color: #00e5ff;\n"
  "      font-family: monospace;\n"
  "      text-align: right;\n"
  "    }\n"
  "\n"
  "    .footer {\n"
  "      color: #8491a3;\n"
  "      font-size: 11px;\n"
  "      text-align: center;\n"
  "    }\n"
  "\n"
  "    @media (max-width: 760px) {\n"
  "      body {\n"
  "        padding: 10px;\n"
  "      }\n"
  "\n"
  "      .main-layout {\n"
  "        grid-template-columns: 1fr;\n"
  "      }\n"
  "\n"
  "      .side-controls {\n"
  "        display: grid;\n"
  "        grid-template-columns: 1fr 1fr;\n"
  "      }\n"
  "\n"
  "      .settings {\n"
  "        grid-template-columns: 1fr;\n"
  "      }\n"
  "\n"
  "      .url-group,\n"
  "      .speed-group {\n"
  "        width: 100%;\n"
  "      }\n"
  "\n"
  "      .apply-btn {\n"
  "        width: 100%;\n"
  "      }\n"
  "\n"
  "      .camera-container {\n"
  "        min-height: 200px;\n"
  "      }\n"
  "    }\n"
  "\n"
  "    @media (max-width: 480px) {\n"
  "      header {\n"
  "        align-items: flex-start;\n"
  "        padding: 12px;\n"
  "      }\n"
  "\n"
  "      .brand {\n"
  "        gap: 9px;\n"
  "      }\n"
  "\n"
  "      .logo {\n"
  "        width: 105px;\n"
  "        height: 48px;\n"
  "        padding: 3px 5px;\n"
  "      }\n"
  "\n"
  "      .status {\n"
  "        padding: 6px 8px;\n"
  "        font-size: 10px;\n"
  "      }\n"
  "\n"
  "      .side-controls {\n"
  "        grid-template-columns: 1fr;\n"
  "      }\n"
  "\n"
  "      .dpad {\n"
  "        grid-template-columns: repeat(3, 60px);\n"
  "        grid-template-rows: repeat(3, 52px);\n"
  "      }\n"
  "\n"
  "      .url-group {\n"
  "        align-items: stretch;\n"
  "        flex-direction: column;\n"
  "      }\n"
  "    }\n"
  "  </style>\n"
  "</head>\n"
  "\n"
  "<body>\n"
  "  <main class=\"container\">\n"
  "\n"
  "    <!-- Header: logo, page title, and robot status -->\n"
  "    <header>\n"
  "      <div class=\"brand\">\n"
  "        <div class=\"logo\"><img src=\"data:image/png;base64,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\" alt=\"CHIREC International School logo\"></div>\n"
  "        <h1>ESP32 SURVEILLANCE ROVER</h1>\n"
  "      </div>\n"
  "      <div class=\"status\" id=\"connectionStatus\">● ROBOT ONLINE</div>\n"
  "    </header>\n"
  "\n"
  "    <!-- Camera feed on the left and controls on the right -->\n"
  "    <section class=\"main-layout\">\n"
  "\n"
  "      <div class=\"panel\">\n"
  "        <div class=\"panel-title\">Live camera feed</div>\n"
  "\n"
  "        <div class=\"camera-container\">\n"
  "          <img id=\"cameraFrame\" alt=\"ESP32 camera stream\">\n"
  "          <div class=\"camera-placeholder\" id=\"cameraPlaceholder\">\n"
  "            Camera feed will appear here<br>\n"
  "            Enter the camera URL below and click Apply Camera\n"
  "          </div>\n"
  "          <div class=\"crosshair\"></div>\n"
  "          <div class=\"camera-message\" id=\"cameraStatus\">CAMERA DISCONNECTED</div>\n"
  "        </div>\n"
  "      </div>\n"
  "\n"
  "      <div class=\"side-controls\">\n"
  "\n"
  "        <!-- Rover movement controls -->\n"
  "        <div class=\"panel control-panel\">\n"
  "          <div class=\"panel-title\">Drive control</div>\n"
  "\n"
  "          <div class=\"dpad\" aria-label=\"Drive controls\">\n"
  "            <div></div>\n"
  "            <button class=\"control-btn\" data-drive=\"F\" aria-label=\"Forward\">↑</button>\n"
  "            <div></div>\n"
  "\n"
  "            <button class=\"control-btn\" data-drive=\"L\" aria-label=\"Left\">←</button>\n"
  "            <button class=\"control-btn stop\" id=\"stopButton\">STOP</button>\n"
  "            <button class=\"control-btn\" data-drive=\"R\" aria-label=\"Right\">→</button>\n"
  "\n"
  "            <div></div>\n"
  "            <button class=\"control-btn\" data-drive=\"B\" aria-label=\"Backward\">↓</button>\n"
  "            <div></div>\n"
  "          </div>\n"
  "          <div class=\"hint\">Press and hold a direction to move</div>\n"
  "        </div>\n"
  "\n"
  "        <!-- Camera pan and tilt controls -->\n"
  "        <div class=\"panel control-panel\">\n"
  "          <div class=\"panel-title\">Camera pan / tilt</div>\n"
  "\n"
  "          <div class=\"dpad\" aria-label=\"Camera pan and tilt controls\">\n"
  "            <div></div>\n"
  "            <button class=\"control-btn camera-btn\" data-camera=\"UP\" aria-label=\"Tilt up\">↑</button>\n"
  "            <div></div>\n"
  "\n"
  "            <button class=\"control-btn camera-btn\" data-camera=\"LEFT\" aria-label=\"Pan left\">←</button>\n"
  "            <button class=\"control-btn camera-btn\" data-camera=\"CENTER\" aria-label=\"Center camera\">●</button>\n"
  "            <button class=\"control-btn camera-btn\" data-camera=\"RIGHT\" aria-label=\"Pan right\">→</button>\n"
  "\n"
  "            <div></div>\n"
  "            <button class=\"control-btn camera-btn\" data-camera=\"DOWN\" aria-label=\"Tilt down\">↓</button>\n"
  "            <div></div>\n"
  "          </div>\n"
  "          <div class=\"hint\">Adjust the camera viewing direction</div>\n"
  "        </div>\n"
  "\n"
  "      </div>\n"
  "    </section>\n"
  "\n"
  "    <!-- Camera URL and drive speed -->\n"
  "    <section class=\"settings\">\n"
  "      <div class=\"url-group\">\n"
  "        <label for=\"camUrl\">CAMERA URL</label>\n"
  "        <input\n"
  "          type=\"text\"\n"
  "          id=\"camUrl\"\n"
  "          placeholder=\"http://192.168.1.105:81/stream\"\n"
  "          autocomplete=\"url\"\n"
  "          spellcheck=\"false\"\n"
  "        >\n"
  "      </div>\n"
  "\n"
  "      <button class=\"apply-btn\" type=\"button\" onclick=\"updateStream()\">APPLY CAMERA</button>\n"
  "\n"
  "      <div class=\"speed-group\">\n"
  "        <label for=\"speedSlider\">DRIVE SPEED</label>\n"
  "        <input\n"
  "          type=\"range\"\n"
  "          id=\"speedSlider\"\n"
  "          min=\"100\"\n"
  "          max=\"255\"\n"
  "          value=\"220\"\n"
  "          oninput=\"sendSpeed(this.value)\"\n"
  "        >\n"
  "        <output id=\"speedValue\">220</output>\n"
  "      </div>\n"
  "    </section>\n"
  "\n"
  "    <div class=\"footer\">ESP32 Surveillance Rover Control Panel</div>\n"
  "  </main>\n"
  "\n"
  "  <script>\n"
  "    // Keep track of the current movement command.\n"
  "    let activeDirection = null;\n"
  "    let commandHeartbeat = null;\n"
  "\n"
  "    const cameraFrame = document.getElementById(\"cameraFrame\");\n"
  "    const cameraStatus = document.getElementById(\"cameraStatus\");\n"
  "    const cameraPlaceholder = document.getElementById(\"cameraPlaceholder\");\n"
  "\n"
  "    // Send a command to the ESP32 web server.\n"
  "    function sendCommand(endpoint, value) {\n"
  "      fetch(\"/\" + endpoint + \"?dir=\" + encodeURIComponent(value), {\n"
  "        cache: \"no-store\"\n"
  "      }).catch(function () {\n"
  "        // The page can still be previewed without an ESP32 connected.\n"
  "      });\n"
  "    }\n"
  "\n"
  "    // Update the displayed speed and send it to the ESP32.\n"
  "    function sendSpeed(value) {\n"
  "      document.getElementById(\"speedValue\").textContent = value;\n"
  "\n"
  "      fetch(\"/speed?val=\" + encodeURIComponent(value), {\n"
  "        cache: \"no-store\"\n"
  "      }).catch(function () {\n"
  "        // Ignore connection errors during offline preview.\n"
  "      });\n"
  "    }\n"
  "\n"
  "    // Start movement and repeat the command while the button is held.\n"
  "    function startMovement(command) {\n"
  "      if (activeDirection === command) {\n"
  "        return;\n"
  "      }\n"
  "\n"
  "      activeDirection = command;\n"
  "      sendCommand(\"move\", command);\n"
  "      clearInterval(commandHeartbeat);\n"
  "\n"
  "      commandHeartbeat = setInterval(function () {\n"
  "        if (activeDirection) {\n"
  "          sendCommand(\"move\", activeDirection);\n"
  "        }\n"
  "      }, 200);\n"
  "    }\n"
  "\n"
  "    // Stop the rover.\n"
  "    function stopMovement() {\n"
  "      activeDirection = null;\n"
  "      clearInterval(commandHeartbeat);\n"
  "      sendCommand(\"move\", \"S\");\n"
  "\n"
  "      document.querySelectorAll(\"[data-drive]\").forEach(function (button) {\n"
  "        button.classList.remove(\"active\");\n"
  "      });\n"
  "    }\n"
  "\n"
  "    // Send pan/tilt commands to the ESP32.\n"
  "    function cameraCommand(direction) {\n"
  "      sendCommand(\"camera\", direction);\n"
  "    }\n"
  "\n"
  "    // Load the camera stream into the image area.\n"
  "    function loadCamera(url) {\n"
  "      if (!url) {\n"
  "        return;\n"
  "      }\n"
  "\n"
  "      cameraStatus.textContent = \"CONNECTING CAMERA...\";\n"
  "      cameraStatus.style.color = \"#00e5ff\";\n"
  "      cameraPlaceholder.style.display = \"none\";\n"
  "      cameraFrame.src = url;\n"
  "    }\n"
  "\n"
  "    cameraFrame.onload = function () {\n"
  "      cameraStatus.textContent = \"CAMERA LIVE\";\n"
  "      cameraStatus.style.color = \"#00e68a\";\n"
  "      cameraPlaceholder.style.display = \"none\";\n"
  "    };\n"
  "\n"
  "    cameraFrame.onerror = function () {\n"
  "      cameraStatus.textContent = \"CAMERA STREAM ERROR\";\n"
  "      cameraStatus.style.color = \"#ff3d63\";\n"
  "      cameraPlaceholder.style.display = \"grid\";\n"
  "    };\n"
  "\n"
  "    // Apply the URL entered by the user.\n"
  "    function updateStream() {\n"
  "      let url = document.getElementById(\"camUrl\").value.trim();\n"
  "\n"
  "      if (!url) {\n"
  "        cameraStatus.textContent = \"ENTER CAMERA URL\";\n"
  "        cameraStatus.style.color = \"#ff3d63\";\n"
  "        return;\n"
  "      }\n"
  "\n"
  "      // If only an IP address is entered, use the common ESP32-CAM stream path.\n"
  "      if (!url.includes(\"/stream\")) {\n"
  "        url = url.replace(/\\/$/, \"\");\n"
  "        const match = url.match(/^http:\\/\\/([^:/]+)$/);\n"
  "\n"
  "        if (match) {\n"
  "          url = \"http://\" + match[1] + \":81/stream\";\n"
  "        }\n"
  "      }\n"
  "\n"
  "      document.getElementById(\"camUrl\").value = url;\n"
  "      loadCamera(url);\n"
  "    }\n"
  "\n"
  "    // Support mouse, touch, and stylus for drive buttons.\n"
  "    document.querySelectorAll(\"[data-drive]\").forEach(function (button) {\n"
  "      const command = button.getAttribute(\"data-drive\");\n"
  "\n"
  "      button.addEventListener(\"pointerdown\", function (event) {\n"
  "        event.preventDefault();\n"
  "        startMovement(command);\n"
  "        button.classList.add(\"active\");\n"
  "      });\n"
  "\n"
  "      [\"pointerup\", \"pointerleave\", \"pointercancel\"].forEach(function (eventName) {\n"
  "        button.addEventListener(eventName, function () {\n"
  "          stopMovement();\n"
  "        });\n"
  "      });\n"
  "    });\n"
  "\n"
  "    // Camera pan and tilt buttons send one command per click.\n"
  "    document.querySelectorAll(\"[data-camera]\").forEach(function (button) {\n"
  "      button.addEventListener(\"click\", function () {\n"
  "        cameraCommand(button.getAttribute(\"data-camera\"));\n"
  "      });\n"
  "    });\n"
  "\n"
  "    document.getElementById(\"stopButton\").addEventListener(\"click\", stopMovement);\n"
  "\n"
  "    // Keyboard controls: W/A/S/D or arrow keys to drive; space to stop.\n"
  "    window.addEventListener(\"keydown\", function (event) {\n"
  "      if (event.target.tagName === \"INPUT\") {\n"
  "        return;\n"
  "      }\n"
  "\n"
  "      const key = event.key.toLowerCase();\n"
  "      const keys = {\n"
  "        w: \"F\", arrowup: \"F\",\n"
  "        a: \"L\", arrowleft: \"L\",\n"
  "        s: \"B\", arrowdown: \"B\",\n"
  "        d: \"R\", arrowright: \"R\"\n"
  "      };\n"
  "\n"
  "      if (keys[key]) {\n"
  "        event.preventDefault();\n"
  "        startMovement(keys[key]);\n"
  "      } else if (key === \" \") {\n"
  "        stopMovement();\n"
  "      } else if (key === \"i\") {\n"
  "        cameraCommand(\"UP\");\n"
  "      } else if (key === \"k\") {\n"
  "        cameraCommand(\"DOWN\");\n"
  "      } else if (key === \"j\") {\n"
  "        cameraCommand(\"LEFT\");\n"
  "      } else if (key === \"l\") {\n"
  "        cameraCommand(\"RIGHT\");\n"
  "      } else if (key === \"c\") {\n"
  "        cameraCommand(\"CENTER\");\n"
  "      }\n"
  "    });\n"
  "\n"
  "    window.addEventListener(\"keyup\", function (event) {\n"
  "      if (event.target.tagName === \"INPUT\") {\n"
  "        return;\n"
  "      }\n"
  "\n"
  "      if ([\"w\", \"a\", \"s\", \"d\", \"arrowup\", \"arrowdown\", \"arrowleft\", \"arrowright\"].includes(event.key.toLowerCase())) {\n"
  "        stopMovement();\n"
  "      }\n"
  "    });\n"
  "\n"
  "    // Stop movement if the browser loses focus or the tab is hidden.\n"
  "    window.addEventListener(\"blur\", stopMovement);\n"
  "\n"
  "    document.addEventListener(\"visibilitychange\", function () {\n"
  "      if (document.hidden) {\n"
  "        stopMovement();\n"
  "      }\n"
  "    });\n"
  "\n"
  "    // Try to get the configured camera URL from the ESP32 server.\n"
  "    // This endpoint must exist in your ESP32 firmware for auto-loading to work.\n"
  "    window.addEventListener(\"load\", function () {\n"
  "      fetch(\"/getCamUrl\", { cache: \"no-store\" })\n"
  "        .then(function (response) {\n"
  "          if (!response.ok) {\n"
  "            throw new Error(\"Camera URL endpoint unavailable\");\n"
  "          }\n"
  "          return response.text();\n"
  "        })\n"
  "        .then(function (url) {\n"
  "          url = url.trim();\n"
  "          if (url) {\n"
  "            document.getElementById(\"camUrl\").value = url;\n"
  "            loadCamera(url);\n"
  "          }\n"
  "        })\n"
  "        .catch(function () {\n"
  "          cameraStatus.textContent = \"ENTER CAMERA URL\";\n"
  "        });\n"
  "    });\n"
  "  </script>\n"
  "</body>\n"
  "</html>\n";
 
 
// ======================================================
// WEB ROUTES
// ======================================================
 
 
// ------------------------------------------------------
// ROOT PAGE
// ------------------------------------------------------
 
void handleRoot()
{
  server.send_P(
    200,
    "text/html",
    INDEX_HTML
  );
}
 
 
// ------------------------------------------------------
// CAMERA URL
// ------------------------------------------------------
 
void handleGetCamUrl()
{
  server.sendHeader(
    "Cache-Control",
    "no-store"
  );
 
  server.send(
    200,
    "text/plain",
    cameraStream
  );
}
 
 
// ------------------------------------------------------
// SPEED
// ------------------------------------------------------
 
void handleSpeed()
{
  if(
    server.hasArg("val")
  )
  {
    motorSpeed =
      constrain(
        server
        .arg("val")
        .toInt(),
        0,
        255
      );
 
 
    if(isMoving)
    {
      applySpeed(
        motorSpeed
      );
    }
  }
 
 
  server.send(
    200,
    "text/plain",
    "OK"
  );
}
 
 
// ------------------------------------------------------
// MOVEMENT
// ------------------------------------------------------
 
void handleMovement()
{
  String dir =
    server.arg("dir");
 
 
  if(dir == "F")
  {
    forward();
  }
 
  else if(dir == "B")
  {
    backward();
  }
 
  else if(dir == "L")
  {
    turnLeft();
  }
 
  else if(dir == "R")
  {
    turnRight();
  }
 
  else
  {
    stopRobot();
  }
 
 
  server.send(
    200,
    "text/plain",
    "OK"
  );
}
 
 
// ------------------------------------------------------
// CAMERA MOVEMENT
// ------------------------------------------------------
 
void handleCamera()
{
  String dir =
    server.arg("dir");
 
 
  if(dir == "LEFT")
  {
    cameraLeft();
  }
 
  else if(dir == "RIGHT")
  {
    cameraRight();
  }
 
  else if(dir == "UP")
  {
    cameraUp();
  }
 
  else if(dir == "DOWN")
  {
    cameraDown();
  }
 
  else if(dir == "CENTER")
  {
    cameraCenter();
  }
 
 
  server.send(
    200,
    "text/plain",
    "OK"
  );
}
 
 
// ======================================================
// SETUP
// ======================================================
 
void setup()
{
  Serial.begin(115200);
 
  delay(1000);
 
 
  Serial.println();
 
  Serial.println(
    "================================"
  );
 
  Serial.println(
    "ESP32 SURVEILLANCE ROVER"
  );
 
  Serial.println(
    "================================"
  );
 
 
  // ====================================================
  // MOTOR PINS
  // ====================================================
 
  pinMode(
    IN1,
    OUTPUT
  );
 
  pinMode(
    IN2,
    OUTPUT
  );
 
  pinMode(
    IN3,
    OUTPUT
  );
 
  pinMode(
    IN4,
    OUTPUT
  );
 
 
  // ====================================================
  // MOTOR PWM
  // ====================================================
 
  ledcAttach(
    ENA,
    MOTOR_PWM_FREQ,
    MOTOR_PWM_RES
  );
 
  ledcAttach(
    ENB,
    MOTOR_PWM_FREQ,
    MOTOR_PWM_RES
  );
 
 
  stopRobot();
 
 
  // ====================================================
  // SERVO PWM
  // ====================================================
 
  ledcAttach(
    PAN_PIN,
    SERVO_PWM_FREQ,
    SERVO_PWM_RES
  );
 
 
  ledcAttach(
    TILT_PIN,
    SERVO_PWM_FREQ,
    SERVO_PWM_RES
  );
 
 
  cameraCenter();
 
 
  // ====================================================
  // WIFI
  // ====================================================
 
  WiFi.mode(
    WIFI_STA
  );
 
 
  WiFi.setSleep(
    false
  );
 
 
  WiFi.begin(
    ssid,
    password
  );
 
 
  Serial.print(
    "Connecting to WiFi"
  );
 
 
  while(
    WiFi.status() !=
    WL_CONNECTED
  )
  {
    delay(500);
 
    Serial.print(".");
  }
 
 
  Serial.println();
 
  Serial.println(
    "WiFi connected!"
  );
 
 
  Serial.print(
    "Robot IP: http://"
  );
 
  Serial.println(
    WiFi.localIP()
  );
 
 
  Serial.print(
    "Camera Stream: "
  );
 
  Serial.println(
    cameraStream
  );
 
 
  // ====================================================
  // SERVER ROUTES
  // ====================================================
 
  server.on(
    "/",
    HTTP_GET,
    handleRoot
  );
 
 
  server.on(
    "/getCamUrl",
    HTTP_GET,
    handleGetCamUrl
  );
 
 
  server.on(
    "/speed",
    HTTP_GET,
    handleSpeed
  );
 
 
  server.on(
    "/move",
    HTTP_GET,
    handleMovement
  );
 
 
  server.on(
    "/camera",
    HTTP_GET,
    handleCamera
  );
 
 
  // ====================================================
  // 404
  // ====================================================
 
  server.onNotFound(
    []()
    {
      server.send(
        404,
        "text/plain",
        "Not Found"
      );
    }
  );
 
 
  // ====================================================
  // START SERVER
  // ====================================================
 
  server.begin();
 
 
  Serial.println(
    "Web server started."
  );
 
 
  Serial.println(
    "================================"
  );
}
 
 
// ======================================================
// LOOP
// ======================================================
 
void loop()
{
  server.handleClient();
 
 
  // ====================================================
  // MOTOR WATCHDOG
  // ====================================================
 
  if(
    isMoving &&
    (
      millis() -
      lastCommandTime >
      SAFETY_TIMEOUT_MS
    )
  )
  {
    stopRobot();
  }
}
