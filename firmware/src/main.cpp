#include <Arduino.h>
#include <ArduinoJson.h>
#include <WebServer.h>
#include <WiFi.h>

#include "config.h"
#include "drive_mixer.h"
#include "motor_driver.h"
#include "web_ui.h"

MotorDriver motors;
WebServer server(HTTP_PORT);

enum class PatternMode {
  None,
  Wiggle,
  Explore,
  Forward,
  Pounce,
  Circle,
};

struct PatternState {
  PatternMode mode = PatternMode::None;
  float intensity = 0.0f;
  uint32_t startedMs = 0;
  uint32_t durationMs = 0;
  uint32_t nextStepMs = 0;
  bool phase = false;
};

PatternState pattern;

String makeApSsid() {
  const uint32_t chip = static_cast<uint32_t>(ESP.getEfuseMac());
  char suffix[9];
  snprintf(suffix, sizeof(suffix), "%04X", static_cast<unsigned int>(chip & 0xFFFF));
  return String(AP_SSID_PREFIX) + "-" + suffix;
}

void sendJson(const JsonDocument &doc, int status = 200) {
  String body;
  serializeJson(doc, body);
  server.send(status, "application/json", body);
}

bool parseBody(JsonDocument &doc) {
  const String body = server.arg("plain");
  DeserializationError error = deserializeJson(doc, body);
  if (error) {
    JsonDocument response;
    response["ok"] = false;
    response["error"] = "invalid_json";
    sendJson(response, 400);
    return false;
  }
  return true;
}

PatternMode parsePatternName(const char *name) {
  if (strcmp(name, "wiggle") == 0) return PatternMode::Wiggle;
  if (strcmp(name, "explore") == 0) return PatternMode::Explore;
  if (strcmp(name, "forward") == 0) return PatternMode::Forward;
  if (strcmp(name, "pounce") == 0) return PatternMode::Pounce;
  if (strcmp(name, "circle") == 0) return PatternMode::Circle;
  return PatternMode::None;
}

void startPattern(PatternMode mode, float intensity, uint32_t durationMs) {
  pattern.mode = mode;
  pattern.intensity = constrain(intensity, 0.0f, 1.0f);
  pattern.startedMs = millis();
  pattern.durationMs = constrain(durationMs, 100UL, 60000UL);
  pattern.nextStepMs = 0;
  pattern.phase = false;
}

void stopPattern() {
  pattern = PatternState{};
}

void updatePattern() {
  if (pattern.mode == PatternMode::None) return;

  const uint32_t now = millis();
  if (now - pattern.startedMs >= pattern.durationMs) {
    stopPattern();
    motors.stop();
    return;
  }

  switch (pattern.mode) {
    case PatternMode::Forward:
      motors.setRaw(pattern.intensity, pattern.intensity);
      break;

    case PatternMode::Circle:
      motors.setRaw(pattern.intensity, pattern.intensity * 0.35f);
      break;

    case PatternMode::Wiggle:
      if (now >= pattern.nextStepMs) {
        pattern.phase = !pattern.phase;
        pattern.nextStepMs = now + 140;
        if (pattern.phase) {
          motors.setRaw(pattern.intensity, pattern.intensity * 0.25f);
        } else {
          motors.setRaw(pattern.intensity * 0.25f, pattern.intensity);
        }
      }
      break;

    case PatternMode::Explore:
      if (now >= pattern.nextStepMs) {
        pattern.nextStepMs = now + random(180, 650);
        const float turn = random(-100, 101) / 100.0f;
        MotorCommand command = mixDrive(pattern.intensity, turn);
        motors.setRaw(command.left, command.right);
      }
      break;

    case PatternMode::Pounce: {
      const uint32_t elapsed = now - pattern.startedMs;
      if (elapsed < 700) {
        motors.setRaw(0.0f, 0.0f);
      } else if (elapsed < 1200) {
        motors.setRaw(pattern.intensity, pattern.intensity);
      } else {
        motors.setRaw(0.0f, 0.0f);
      }
      break;
    }

    case PatternMode::None:
      break;
  }
}

void handleRoot() {
  server.send_P(200, "text/html", INDEX_HTML);
}

void handleAppJs() {
  server.send_P(200, "application/javascript", APP_JS);
}

void handleStyleCss() {
  server.send_P(200, "text/css", STYLE_CSS);
}

void handleStatus() {
  JsonDocument doc;
  const MotorCommand output = motors.output();
  const RuntimeConfig config = motors.runtimeConfig();

  doc["ok"] = true;
  doc["left"] = output.left;
  doc["right"] = output.right;
  doc["last_command_ms_ago"] = motors.lastCommandAgeMs();
  doc["max_duty"] = config.maxDuty;
  doc["battery_mv"] = nullptr;
  doc["pattern_active"] = pattern.mode != PatternMode::None;
  sendJson(doc);
}

void handleGetConfig() {
  JsonDocument doc;
  const RuntimeConfig config = motors.runtimeConfig();

  doc["ok"] = true;
  doc["command_timeout_ms"] = COMMAND_TIMEOUT_MS;
  doc["max_duty"] = config.maxDuty;
  doc["ramp_rate_per_second"] = config.rampRatePerSecond;
  doc["left_trim"] = config.leftTrim;
  doc["right_trim"] = config.rightTrim;
  doc["pwm_freq_hz"] = PWM_FREQ_HZ;
  doc["pwm_res_bits"] = PWM_RES_BITS;
  doc["left_motor_pin"] = LEFT_MOTOR_PWM_PIN;
  doc["right_motor_pin"] = RIGHT_MOTOR_PWM_PIN;
  sendJson(doc);
}

void handlePostConfig() {
  JsonDocument body;
  if (!parseBody(body)) return;

  RuntimeConfig config = motors.runtimeConfig();
  if (body["max_duty"].is<float>()) config.maxDuty = body["max_duty"].as<float>();
  if (body["ramp_rate_per_second"].is<float>()) config.rampRatePerSecond = body["ramp_rate_per_second"].as<float>();
  if (body["left_trim"].is<float>()) config.leftTrim = body["left_trim"].as<float>();
  if (body["right_trim"].is<float>()) config.rightTrim = body["right_trim"].as<float>();

  motors.setRuntimeConfig(config);
  handleGetConfig();
}

void handleStop() {
  stopPattern();
  motors.stop();
  Serial.println("STOP");

  JsonDocument doc;
  doc["ok"] = true;
  doc["left"] = 0;
  doc["right"] = 0;
  sendJson(doc);
}

void handleDrive() {
  JsonDocument body;
  if (!parseBody(body)) return;

  const char *mode = body["mode"] | "";
  MotorCommand command{0.0f, 0.0f};

  if (strcmp(mode, "drive") == 0) {
    stopPattern();
    command = mixDrive(body["throttle"] | 0.0f, body["turn"] | 0.0f);
    motors.setRaw(command.left, command.right);
  } else if (strcmp(mode, "direct") == 0) {
    stopPattern();
    command = {body["left"] | 0.0f, body["right"] | 0.0f};
    motors.setRaw(command.left, command.right);
  } else if (strcmp(mode, "pattern") == 0) {
    const char *name = body["name"] | "";
    const PatternMode patternMode = parsePatternName(name);
    if (patternMode == PatternMode::None) {
      JsonDocument response;
      response["ok"] = false;
      response["error"] = "unknown_pattern";
      sendJson(response, 400);
      return;
    }
    startPattern(patternMode, body["intensity"] | 0.6f, body["duration_ms"] | 3000UL);
  } else {
    JsonDocument response;
    response["ok"] = false;
    response["error"] = "unknown_mode";
    sendJson(response, 400);
    return;
  }

  Serial.printf("drive mode=%s left=%.2f right=%.2f\n", mode, command.left, command.right);

  JsonDocument response;
  response["ok"] = true;
  response["left"] = motors.target().left;
  response["right"] = motors.target().right;
  response["ttl_ms"] = COMMAND_TIMEOUT_MS;
  response["pattern_active"] = pattern.mode != PatternMode::None;
  sendJson(response);
}

void handleNotFound() {
  JsonDocument doc;
  doc["ok"] = false;
  doc["error"] = "not_found";
  sendJson(doc, 404);
}

void setupRoutes() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/app.js", HTTP_GET, handleAppJs);
  server.on("/style.css", HTTP_GET, handleStyleCss);
  server.on("/status", HTTP_GET, handleStatus);
  server.on("/config", HTTP_GET, handleGetConfig);
  server.on("/config", HTTP_POST, handlePostConfig);
  server.on("/drive", HTTP_POST, handleDrive);
  server.on("/stop", HTTP_POST, handleStop);
  server.onNotFound(handleNotFound);
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println("VibroBug boot");

  motors.begin();
  randomSeed(static_cast<uint32_t>(ESP.getEfuseMac()));

  const String ssid = makeApSsid();
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid.c_str(), AP_PASSWORD);

  Serial.printf("AP SSID: %s\n", ssid.c_str());
  Serial.printf("AP IP: %s\n", WiFi.softAPIP().toString().c_str());

  setupRoutes();
  server.begin();
  Serial.println("HTTP server ready");
}

void loop() {
  server.handleClient();
  updatePattern();
  motors.update();
  delay(2);
}

