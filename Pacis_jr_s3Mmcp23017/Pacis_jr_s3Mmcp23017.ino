
/*
====================================================================
  Pacis_jr-s3Mmcp23017 - Junior logic with zone bicolor LEDs
  VERSION: 01.04
  BUILD: PACIS_JR_S3MMCP23017_COLOR_2026_07_03
  BUILD TIME: 2026-07-03 22:35:00 -04:00
  ESP32-WROOM-32E
  Protocolo PACIS / Penta Token Booth Microphone Station
=====================================================================
  Lugar donde se explica los botones fisicos:
  - Boton 1 -> Zona 1       -> tecla PACIS 07
  - Boton 2 -> Zona 2       -> tecla PACIS 08
  - Boton 3 -> Zona 3       -> tecla PACIS 09
  - Boton 4 -> Zona 4       -> tecla PACIS 10
  - Boton 5 -> Zona 5       -> tecla PACIS 11
  - Boton 6 -> PTT          -> tecla de proyecto 06
  - Boton 7 -> White noise  -> tecla de proyecto 12

  Lugar donde se ve el orden del conector de botones:
  - GPIO12 -> Zone 1 / key 07
  - GPIO14 -> Zone 2 / key 08
  - GPIO27 -> Zone 3 / key 09
  - GPIO26 -> Zone 4 / key 10
  - GPIO25 -> Zone 5 / key 11
  - GPIO33 -> PTT / key 06
  - GPIO32 -> White noise / key 12

  Lugar donde se ve el MCP23017 para LEDs de zonas bicolor y W.N verde:
  - MCP23017 GPA0 -> Zona 1 verde
  - MCP23017 GPA1 -> Zona 2 verde
  - MCP23017 GPA2 -> Zona 3 verde
  - MCP23017 GPA3 -> Zona 4 verde
  - MCP23017 GPA4 -> Zona 5 verde
  - MCP23017 GPA5 -> W.N. verde
  - MCP23017 GPB0 -> Zona 1 rojo/busy
  - MCP23017 GPB1 -> Zona 2 rojo/busy
  - MCP23017 GPB2 -> Zona 3 rojo/busy
  - MCP23017 GPB3 -> Zona 4 rojo/busy
  - MCP23017 GPB4 -> Zona 5 rojo/busy
  - MCP23017 GPB5 -> reserva
  - GPIO2  -> LED Heartbeat/fail local existente

  Lugar donde se ve I2C hacia el MCP23017:
  - ESP32 GPIO21 -> MCP23017 SDA
  - ESP32 GPIO22 -> MCP23017 SCL
  - PTT no usa LED, solo boton

  Lugar donde se conecta el modulo MAX3485 RS485 de 3.3V:
  - ESP32 GPIO16 TX -> MAX3485 RX / DI / DX
  - MAX3485 TX / RO -> ESP32 GPIO34 RX
  - ESP32 GPIO13    -> MAX3485 EN
  - ESP32 3.3V      -> MAX3485 VCC
  - ESP32 GND       -> MAX3485 GND
  - MAX3485 A       -> PACIS/server A
  - MAX3485 B       -> PACIS/server B

  Lugar donde se aclara la SD:
  - La SD fue removida en esta etapa. GPIO34 ahora es RS485 RX.
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23X17.h>

static const uint8_t stx = 0x02;
static const uint8_t etx = 0x03;

static const uint8_t adr = 0;
static const char ach = '0' + adr;

static const int rx = 34;
static const int tx = 16;
static const int en = 13;
static const uint32_t br = 9600;

// Lugar donde se activa o desactiva el hardware principal.
static const bool r485 = true;

static const uint8_t bct = 6;
static const int bpn[bct] = {
  12, 14, 27, 26, 25, 32
};
static const uint8_t bky[bct] = {
  7, 8, 9, 10, 11, 12
};

static const int ptt = 33;
static const uint8_t ptk = 6;

static const uint8_t zct = 5;
static const int lpn[zct] = {
  0, 1, 2, 3, 4
};
static const uint8_t ind[zct] = {
  7, 8, 9, 10, 11
};

static const int lpt = -1;
static const int lwn = 5;
static const int lhb = 2;

static const uint8_t sda = 21;
static const uint8_t scl = 22;
static const uint8_t mad = 0x20;
static const uint8_t rpn[zct] = {8, 9, 10, 11, 12};
static const int rwn = -1;

static const uint8_t kbt = 24;
static const uint8_t ict = 24;
static const uint8_t qsz = 64;

static const uint16_t db = 120;
static const uint16_t dbz = 45;
static const uint16_t dbm = 18;
static const uint16_t dbr = 35;
static const uint16_t wgd = 250;
static const uint16_t rty = 900;
static const uint16_t bl = 500;
static const uint16_t hb = 1000;
static const uint8_t hbp = 18;
static const uint8_t flp = 70;
static const uint8_t pwm = 10;
static const uint16_t flt = 1500;
static const uint16_t hbt = 3000;
static const uint16_t lon = 5000;
static const uint16_t lst = 1000;
static const uint16_t pld = 1000;
static const uint16_t rsd = 3000;
static const bool dbg = false;

static uint16_t swt = 100;
static uint8_t pto = 45;
static bool osc = false;
static const char ver[] = "01.04";
static const char tag[] = "PACIS_JR_S3MMCP23017_COLOR_2026_07_03";
static const char bld[] = "2026-07-03 22:35:00 -04:00";

// Lugar donde los nombres largos apuntan a los nombres cortos.
// Esto mantiene la logica PACIS ya probada sin cambiar todo el codigo.
#define STX stx
#define ETX etx
#define STATION_NUMBER adr
#define STATION_CHAR ach
#define RS485_RX_PIN rx
#define RS485_TX_PIN tx
#define RS485_EN_PIN en
#define PACIS_BAUD br
#define RS485_INSTALLED r485
#define PACIS_BUTTON_COUNT bct
#define PACIS_BUTTON_PINS bpn
#define PACIS_BUTTON_KEYS bky
#define PTT_BUTTON_PIN ptt
#define PTT_KEY_NUMBER ptk
#define ZONE_LED_COUNT zct
#define ZONE_LED_PINS lpn
#define ZONE_LED_INDICATORS ind
#define PTT_LED_PIN lpt
#define WHITE_LED_PIN lwn
#define STATUS_LED_PIN lhb
#define I2C_SDA_PIN sda
#define I2C_SCL_PIN scl
#define MCP23017_ADDRESS mad
#define ZONE_RED_LED_PINS rpn
#define WHITE_RED_LED_PIN rwn
#define KEY_STATUS_BITS kbt
#define INDICATOR_STATUS_COUNT ict
#define EVENT_QUEUE_SIZE qsz
#define DEBOUNCE_MS db
#define ZONE_PRESS_DEBOUNCE_MS dbz
#define MOMENT_PRESS_DEBOUNCE_MS dbm
#define MOMENT_RELEASE_DEBOUNCE_MS dbr
#define WHITE_TOGGLE_GUARD_MS wgd
#define ZONE_RETRY_MS rty
#define BLINK_MS bl
#define HEARTBEAT_MS hb
#define HEARTBEAT_BRIGHTNESS_PERCENT hbp
#define FAIL_BRIGHTNESS_PERCENT flp
#define SOFT_PWM_MS pwm
#define FAIL_LED_HOLD_MS flt
#define POLL_HEARTBEAT_TIMEOUT_MS hbt
#define LAMP_TEST_ALL_ON_MS lon
#define LAMP_TEST_STEP_MS lst
#define switchTimeHundredsMs swt
#define pttTimeoutSeconds pto
#define oscillatorOn osc
#define FIRMWARE_VERSION ver
#define LOCAL_BUILD_TAG tag
#define BUILD_STAMP bld

Adafruit_MCP23X17 mcp;
HardwareSerial rs485(2);
static bool mcpReady = false;

struct ButtonState {
  bool stablePressed;
  bool lastRawPressed;
  uint32_t lastChangeMs;
};

static ButtonState pacisButtons[PACIS_BUTTON_COUNT];
static ButtonState pttButton;
static uint8_t indicatorState[INDICATOR_STATUS_COUNT];
static bool selectedZones[ZONE_LED_COUNT];
static bool requestedZones[ZONE_LED_COUNT];
static bool busyTouchedZones[ZONE_LED_COUNT];
static bool busyMutedZones[ZONE_LED_COUNT];
static bool blockedBusyPress[ZONE_LED_COUNT];
static bool busyLockZones[ZONE_LED_COUNT];
static bool busyLockClearSent[ZONE_LED_COUNT];
static volatile bool localBlinkZones[ZONE_LED_COUNT];
static uint8_t requestRetryCount[ZONE_LED_COUNT];
static uint32_t requestRetryMs[ZONE_LED_COUNT];
static uint8_t lastCapturedZoneSi[ZONE_LED_COUNT] = {9, 9, 9, 9, 9};
static bool pttAllRequest = false;
static bool nzActive = false;
static bool nzWait = false;
static uint8_t nzIndex = 0;
static uint8_t nzZone = 0;
static bool clrActive = false;
static bool clrWait = false;
static bool clrNeed[ZONE_LED_COUNT];
static bool clrServerZones[ZONE_LED_COUNT];
static bool clrLocalZones[ZONE_LED_COUNT];
static bool clrKeepBusyZones[ZONE_LED_COUNT];
static uint8_t clrIndex = 0;
static uint8_t clrZone = 0;
static uint8_t clrKey = 0;
static String clrName;
static bool pttActive = false;
static bool pttReleasePending = false;
static bool whiteActive = false;
static uint32_t whiteStartMs = 0;
static uint32_t whiteLastToggleMs = 0;

static String eventQueue[EVENT_QUEUE_SIZE];
static uint8_t queueHead = 0;
static uint8_t queueTail = 0;
static uint8_t queueCount = 0;

static uint8_t rxBuffer[96];
static uint8_t rxLen = 0;
static bool receivingFrame = false;

static volatile uint32_t lastPollTime = 0;
static volatile uint32_t failLedUntilMs = 0;
static TaskHandle_t heartbeatTaskHandle = nullptr;
static TaskHandle_t zoneLedTaskHandle = nullptr;
static uint32_t lastIdlePollDebugMs = 0;
static uint32_t lastRxSummaryMs = 0;
static uint16_t rxGoCount = 0;
static uint16_t rxGkCount = 0;
static uint16_t rxSiCount = 0;
static uint16_t rxOtherCount = 0;

struct CmdRef {
  const char* code;
  const char* dir;
  const char* name;
  const char* meaning;
};

static const CmdRef cmdt[] = {
  {"GK", "RX", "Leer teclas", "Servidor pide los 24 bits de teclas; la estacion contesta ACK y luego KS."},
  {"KS", "TX", "Estado teclas", "La estacion envia los 24 bits de teclas/botones."},
  {"SI", "RX", "Fijar indicadores", "Servidor envia 24 estados de indicadores; la estacion actualiza LEDs y contesta ACK."},
  {"GI", "RX", "Leer indicadores", "Servidor pide estado de indicadores; la estacion contesta ACK y luego IS."},
  {"IS", "TX", "Estado indicadores", "La estacion envia los 24 estados de indicadores."},
  {"KD", "TX", "Tecla presionada", "La estacion reporta que un boton/tecla fue presionado."},
  {"KU", "TX", "Tecla liberada", "La estacion reporta que un boton/tecla fue liberado."},
  {"ST", "RX", "Fijar tiempo", "Servidor fija el tiempo de cambio/switchover."},
  {"GT", "RX", "Leer tiempo", "Servidor pide el tiempo de cambio/switchover."},
  {"TS", "TX", "Estado tiempo", "La estacion envia el tiempo de cambio/switchover."},
  {"GV", "RX", "Leer version", "Servidor pide la version del firmware."},
  {"VE", "TX", "Version", "La estacion envia la version del firmware."},
  {"SP", "RX", "Fijar PTT", "Servidor fija el tiempo limite del PTT."},
  {"BP", "RX", "Beep", "Servidor envia comando de beep/buzzer."},
  {"SO", "RX", "Fijar oscilador", "Servidor fija el estado del oscilador."},
  {"GO", "RX", "Leer oscilador", "Servidor pide el estado del oscilador."},
  {"OS", "TX", "Estado oscilador", "La estacion envia el estado del oscilador."},
  {"!", "TX", "ACK", "Comando aceptado."},
  {"@", "TX", "NAK", "Comando rechazado o error de checksum."}
};

// Lugar donde el sistema lee comandos y significados para debug.
// Si se agrega un comando nuevo, se agrega aqui.
static const uint8_t cmdn = sizeof(cmdt) / sizeof(cmdt[0]);

void printLocalZoneState();
String addZoneToList(String list, uint8_t zoneIndex);
String listServerOkZones();
String listServerBusyZones();
String listRequestedZones();
String listBusyTouchedZones();
String listBoolZones(const bool zones[]);
void printLogicState(const char* label);
void printSeparator();
void runLampTest();
bool anyZoneSelected();
String framePreview(const String &data);
String noChecksumFramePreview(const String &data);
String expectedPollText();
String bytesToHexText(const String &text);
String bytesToBitText(const String &text);
String byteToBitText(uint8_t value);
String commandFromPayload(const String &payload);
String lookupCommandMeaning(const String &commandCode);
bool zoneApprovedByServer(uint8_t zoneIndex);
bool zoneBusyByServer(uint8_t zoneIndex);
void printCommandReference();
void printTrafficDecode(const char* direction, const String &payload, bool hasChecksum);
void printActionDebug(const char* buttonName, bool pressed, const char* ledState, const String &eventData);
void printCommandDebug(const String &cmd, const String &data);
void noteRxCommand(const String &cmd, const String &data);
String queueKeyEvent(uint8_t keyNumber, bool pressed);
void captureZoneSi();
bool anyServerApprovedZone();
void requestAllNonBusyZones(const char* name);
void sendNextNoZoneRequest(const char* name);
void sendNextClearRequest();
void markZoneRequested(uint8_t zoneIndex);
void retryMissedRequestedZones();
String startSession(const char* name, uint8_t keyNumber);
String endSession(const char* name, uint8_t keyNumber);
void checkWhiteTimeout();
void rs485SendBegin();
void rs485SendEnd();
bool setupMcp23017();
void writeZoneLedColor(uint8_t zoneIndex, bool greenOn, bool redOn);
void writeWhiteLedColor(bool greenOn, bool redOn);
void writePttLed(bool on);
void writeStatusLed(bool on, uint8_t brightnessPercent);
void heartbeatTask(void *parameter);
void zoneLedTask(void *parameter);
void clearLocalBlinkZones();

uint8_t calcPentaChecksum(const String &data);
void setFailIndicator();
void sendFrameNoChecksum(const String &data);
void sendDataFrame(const String &data);
void sendAck();
void sendNak();
bool enqueueEvent(const String &eventData);
bool dequeueEvent(String &eventData);
bool queuedZoneEvent(const String &eventData);
void dropQueuedZoneEvents(const char* why);
String twoDigits(uint8_t value);
bool keyIsPressed(uint8_t keyNumber);
String buildKeyStatus();
void printKeyBits();
String buildIndicatorStatus();
void scanPacisButtons();
void scanPttButton();
void updateZoneLeds();
void updateLocalLeds();
void updateStatusLeds();
const char * siStateName(uint8_t state);
void setAllPanelLeds(bool on);
void handlePollByte(uint8_t value);
bool validateDataFrame(String &data);
void handleSetIndicators(const String &data);
void handleCommand(const String &data);
void handleRxByte(uint8_t value);
void setupButtonState(ButtonState &button, int pin, bool internalPullup);
void setup();
void loop();
bool anyZoneSelected() {
  for (uint8_t i = 0; i < ZONE_LED_COUNT; i++) {
    if (zoneApprovedByServer(i)) {
      return true;
    }
  }

  return false;
}

bool anyServerApprovedZone() {
  // Lugar donde se pregunta al servidor si ya hay zona seleccionada.
  // Variables cortas usadas: ind, indicatorState.
  return anyZoneSelected();
}

bool zoneApprovedByServer(uint8_t zoneIndex) {
  // Lugar donde se decide si el servidor aprobo una zona para hablar.
  // En este prototipo solo SI=1, green fijo, es valido para seleccionar.
  // Cualquier blink se trata como no disponible para evitar doble seleccion.
  uint8_t state = indicatorState[ZONE_LED_INDICATORS[zoneIndex]];
  return state == 1;
}

bool zoneBusyByServer(uint8_t zoneIndex) {
  // Lugar donde se detecta zona ocupada por el servidor.
  // SI=2 red, SI=3 blink y SI=4 red blink se bloquean para el operador.
  uint8_t state = indicatorState[ZONE_LED_INDICATORS[zoneIndex]];
  return state == 2 || state == 3 || state == 4;
}

bool zoneNotOffByServer(uint8_t zoneIndex) {
  // Lugar donde se bloquea el boton si el servidor ya muestra algo en esa zona.
  // Si no esta OFF, no se vuelve a mandar KD/KU para esa zona.
  uint8_t state = indicatorState[ZONE_LED_INDICATORS[zoneIndex]];
  return state != 0;
}

String addZoneToList(String list, uint8_t zoneIndex) {
  if (list.length() > 0) {
    list += ",";
  }
  list += "Z";
  list += String(zoneIndex + 1);
  return list;
}

String listServerOkZones() {
  String out;
  for (uint8_t i = 0; i < ZONE_LED_COUNT; i++) {
    if (zoneApprovedByServer(i)) {
      out = addZoneToList(out, i);
    }
  }
  return out.length() ? out : "-";
}

String listServerBusyZones() {
  String out;
  for (uint8_t i = 0; i < ZONE_LED_COUNT; i++) {
    if (zoneBusyByServer(i)) {
      out = addZoneToList(out, i);
    }
  }
  return out.length() ? out : "-";
}

String listRequestedZones() {
  String out;
  for (uint8_t i = 0; i < ZONE_LED_COUNT; i++) {
    if (requestedZones[i]) {
      out = addZoneToList(out, i);
    }
  }
  return out.length() ? out : "-";
}

String listBusyTouchedZones() {
  String out;
  for (uint8_t i = 0; i < ZONE_LED_COUNT; i++) {
    if (busyTouchedZones[i]) {
      out = addZoneToList(out, i);
    }
  }
  return out.length() ? out : "-";
}

String listBoolZones(const bool zones[]) {
  String out;
  for (uint8_t i = 0; i < ZONE_LED_COUNT; i++) {
    if (zones[i]) {
      out = addZoneToList(out, i);
    }
  }
  return out.length() ? out : "-";
}

void printLogicState(const char* label) {
  Serial.print(label);
  Serial.print(" selected=");
  Serial.print(listServerOkZones());
  Serial.print(" busy=");
  Serial.print(listServerBusyZones());
  Serial.print(" requested=");
  Serial.println(listRequestedZones());
}

void printSeparator() {
  Serial.println("------------------------------");
}

uint8_t calcPentaChecksum(const String &data) {
  uint16_t sum = 0;
  for (size_t i = 0; i < data.length(); i++) {
    sum += static_cast<uint8_t>(data[i]);
  }
  return static_cast<uint8_t>((sum & 0x3F) | 0x40);
}

String framePreview(const String &data) {
  uint8_t checksum = calcPentaChecksum(data);
  String checksumHex = String(checksum, HEX);
  checksumHex.toUpperCase();
  if (checksumHex.length() < 2) {
    checksumHex = "0" + checksumHex;
  }

  String out = "<STX>";
  out += data;
  out += char(checksum);
  out += "<ETX>";
  out += "  checksum=0x";
  out += checksumHex;
  return out;
}

String noChecksumFramePreview(const String &data) {
  String out = "<STX>";
  out += data;
  out += "<ETX>";
  out += "  no checksum";
  return out;
}

String bytesToHexText(const String &text) {
  String out;
  for (size_t i = 0; i < text.length(); i++) {
    if (i > 0) out += ' ';
    uint8_t value = static_cast<uint8_t>(text[i]);
    if (value < 16) out += '0';
    out += String(value, HEX);
  }
  out.toUpperCase();
  return out;
}

String bytesToBitText(const String &text) {
  String out;
  for (size_t i = 0; i < text.length(); i++) {
    if (i > 0) out += ' ';
    uint8_t value = static_cast<uint8_t>(text[i]);
    for (int bit = 7; bit >= 0; bit--) {
      out += (value & (1 << bit)) ? '1' : '0';
    }
  }
  return out;
}

String byteToBitText(uint8_t value) {
  String out;
  for (int bit = 7; bit >= 0; bit--) {
    out += (value & (1 << bit)) ? '1' : '0';
  }
  return out;
}

String commandFromPayload(const String &payload) {
  if (payload.length() == 2 && payload[0] == STATION_CHAR &&
      (payload[1] == '!' || payload[1] == '@')) {
    return String(payload[1]);
  }

  if (payload.length() >= 3 && payload[0] == STATION_CHAR) {
    return payload.substring(1, 3);
  }
  if (payload.length() >= 2) {
    return payload.substring(0, 2);
  }
  return payload;
}

String lookupCommandMeaning(const String &commandCode) {
  for (uint8_t i = 0; i < cmdn; i++) {
    if (commandCode == cmdt[i].code) {
      String out = cmdt[i].name;
      out += ": ";
      out += cmdt[i].meaning;
      return out;
    }
  }

  return "Unknown command: not found in firmware command table.";
}

void printCommandReference() {
  printSeparator();
  Serial.println("TABLA DE COMANDOS PACIS: interna en firmware");
  for (uint8_t i = 0; i < cmdn; i++) {
    Serial.print(cmdt[i].code);
    Serial.print(" ");
    Serial.print(cmdt[i].dir);
    Serial.print(" - ");
    Serial.print(cmdt[i].name);
    Serial.print(": ");
    Serial.println(cmdt[i].meaning);
  }
  printSeparator();
}

void printTrafficDecode(const char* direction, const String &payload, bool hasChecksum) {
  if (!dbg) return;

  String commandCode = commandFromPayload(payload);

  printSeparator();
  Serial.print(direction);
  Serial.println(" PACIS DECODE");
  Serial.println("ASCII: " + payload);
  Serial.println("COMMAND: " + commandCode);
  Serial.println("HEX: " + bytesToHexText(payload));
  Serial.println("BITS: " + bytesToBitText(payload));

  if (hasChecksum) {
    uint8_t checksum = calcPentaChecksum(payload);
    String checksumHex = String(checksum, HEX);
    checksumHex.toUpperCase();
    Serial.println("CHECKSUM ASCII: " + String(char(checksum)));
    Serial.println("CHECKSUM HEX: 0x" + checksumHex);
  } else {
    Serial.println("CHECKSUM: none");
  }

  Serial.println("MEANING: " + lookupCommandMeaning(commandCode));
  printSeparator();
}

String expectedPollText() {
  uint8_t pollByte = 0x80 | STATION_NUMBER;
  String pollHex = String(pollByte, HEX);
  pollHex.toUpperCase();

  String out = "RX EXPECTED: PACIS server poll byte 0x";
  out += pollHex;
  out += " before queued event is transmitted";
  if (!RS485_INSTALLED) {
    out += " (modo banco: RS485 apagado)";
  }
  return out;
}

void printActionDebug(const char* buttonName, bool pressed, const char* ledState, const String &eventData) {
  if (!dbg) {
    return;
  }

  printSeparator();
  Serial.println("ACTION DEBUG");
  Serial.print("BUTTON: ");
  Serial.print(buttonName);
  Serial.println(pressed ? " PRESS" : " RELEASE");
  Serial.print("LED: ");
  Serial.println(ledState);
  Serial.print("QUEUE: ");
  Serial.println(eventData);
  Serial.print("TX WHEN POLLED: ");
  Serial.println(framePreview(eventData));
  printTrafficDecode("TX QUEUED", eventData, true);
  Serial.println(expectedPollText());
  Serial.println("RX COMMANDS POSSIBLE: GK asks keys, SI sets indicators, GI asks indicators");
  printLocalZoneState();
  printSeparator();
}

void setFailIndicator() {
  failLedUntilMs = millis() + FAIL_LED_HOLD_MS;
}

bool setupMcp23017() {
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

  if (!mcp.begin_I2C(MCP23017_ADDRESS, &Wire)) {
    Serial.println("ERROR: MCP23017 not found at address 0x20");
    return false;
  }

  for (uint8_t i = 0; i < ZONE_LED_COUNT; i++) {
    mcp.pinMode(ZONE_LED_PINS[i], OUTPUT);
    mcp.digitalWrite(ZONE_LED_PINS[i], LOW);
    mcp.pinMode(ZONE_RED_LED_PINS[i], OUTPUT);
    mcp.digitalWrite(ZONE_RED_LED_PINS[i], LOW);
  }

  mcp.pinMode(WHITE_LED_PIN, OUTPUT);
  mcp.digitalWrite(WHITE_LED_PIN, LOW);
  mcpReady = true;
  return true;
}

void writeZoneLedColor(uint8_t zoneIndex, bool greenOn, bool redOn) {
  if (zoneIndex >= ZONE_LED_COUNT) return;
  if (!mcpReady) return;

  mcp.digitalWrite(ZONE_LED_PINS[zoneIndex], greenOn ? HIGH : LOW);
  mcp.digitalWrite(ZONE_RED_LED_PINS[zoneIndex], redOn ? HIGH : LOW);
}

void writeWhiteLedColor(bool greenOn, bool redOn) {
  (void)redOn;
  if (!mcpReady) return;

  mcp.digitalWrite(WHITE_LED_PIN, greenOn ? HIGH : LOW);
}

void writePttLed(bool on) {
  (void)on;
}

void rs485SendBegin() {
  if (!RS485_INSTALLED) return;
  digitalWrite(RS485_EN_PIN, HIGH);
  delayMicroseconds(20);
}

void rs485SendEnd() {
  if (!RS485_INSTALLED) return;
  rs485.flush();
  delayMicroseconds(20);
  digitalWrite(RS485_EN_PIN, LOW);
}

void sendFrameNoChecksum(const String &data) {
  printTrafficDecode("TX", data, false);
  rs485SendBegin();
  rs485.write(STX);
  rs485.print(data);
  rs485.write(ETX);
  rs485SendEnd();
}

void sendDataFrame(const String &data) {
  printTrafficDecode("TX", data, true);
  rs485SendBegin();
  rs485.write(STX);
  rs485.print(data);
  rs485.write(calcPentaChecksum(data));
  rs485.write(ETX);
  rs485SendEnd();
}

void sendAck() {
  String data;
  data += STATION_CHAR;
  data += '!';
  sendFrameNoChecksum(data);
}

void sendNak() {
  String data;
  data += STATION_CHAR;
  data += '@';
  sendFrameNoChecksum(data);
  setFailIndicator();
}

bool enqueueEvent(const String &eventData) {
  if (queueCount >= EVENT_QUEUE_SIZE) {
    setFailIndicator();
    Serial.println("Event queue full, dropped: " + eventData);
    return false;
  }

  eventQueue[queueTail] = eventData;
  queueTail = (queueTail + 1) % EVENT_QUEUE_SIZE;
  queueCount++;
  if (dbg) {
    Serial.println("QUEUED: " + eventData + " q=" + String(queueCount));
  }
  return true;
}

bool dequeueEvent(String &eventData) {
  if (queueCount == 0) return false;
  eventData = eventQueue[queueHead];
  queueHead = (queueHead + 1) % EVENT_QUEUE_SIZE;
  queueCount--;
  return true;
}

bool queuedZoneEvent(const String &eventData) {
  // Lugar donde se detecta evento KD/KU pendiente de zona 07-11.
  // Variables cortas usadas: eventData, bky.
  if (eventData.length() < 5) return false;
  String cmd = eventData.substring(1, 3);
  if (cmd != "KD" && cmd != "KU") return false;

  uint8_t key = eventData.substring(3, 5).toInt();
  return key >= 7 && key <= 11;
}

void dropQueuedZoneEvents(const char* why) {
  // Lugar donde se limpia buffer de zonas antes de empezar/terminar sesion.
  // Variables cortas usadas: qsz, eventQueue, queueHead, queueTail, queueCount.
  if (queueCount == 0) return;

  String kept[EVENT_QUEUE_SIZE];
  uint8_t keptCount = 0;
  uint8_t dropped = 0;
  uint8_t index = queueHead;

  for (uint8_t n = 0; n < queueCount; n++) {
    String eventData = eventQueue[index];
    if (queuedZoneEvent(eventData)) {
      dropped++;
    } else if (keptCount < EVENT_QUEUE_SIZE) {
      kept[keptCount++] = eventData;
    }
    index = (index + 1) % EVENT_QUEUE_SIZE;
  }

  queueHead = 0;
  queueTail = 0;
  queueCount = 0;

  for (uint8_t i = 0; i < keptCount; i++) {
    eventQueue[queueTail] = kept[i];
    queueTail = (queueTail + 1) % EVENT_QUEUE_SIZE;
    queueCount++;
  }

  if (dropped > 0) {
    Serial.println(String("QUEUE CLEAN ") + why + ": dropped zone events=" + String(dropped));
  }
}

String twoDigits(uint8_t value) {
  String out;
  if (value < 10) out += '0';
  out += String(value);
  return out;
}

String queueKeyEvent(uint8_t keyNumber, bool pressed) {
  // Lugar donde el sistema pide al servidor validar una tecla/zona.
  // La peticion real PACIS es KD/KU; la aprobacion de lampara llega por SI.
  String eventData;
  eventData += STATION_CHAR;
  eventData += pressed ? "KD" : "KU";
  eventData += twoDigits(keyNumber);
  enqueueEvent(eventData);
  printKeyBits();
  return eventData;
}

bool keyIsPressed(uint8_t keyNumber) {
  // Lugar donde GK/KS lee estado de tecla.
  // Usa ptk, pttActive y whiteActive para reportar PTT/W.N como sesion.
  if (keyNumber == PTT_KEY_NUMBER) return pttActive || pttButton.stablePressed;
  if (keyNumber == 12) return whiteActive;

  for (uint8_t i = 0; i < PACIS_BUTTON_COUNT; i++) {
    if (PACIS_BUTTON_KEYS[i] == keyNumber) {
      return pacisButtons[i].stablePressed;
    }
  }

  return false;
}

String buildKeyStatus() {
  String data;
  data += STATION_CHAR;
  data += "KS";

  for (uint8_t key = 0; key < KEY_STATUS_BITS; key++) {
    data += keyIsPressed(key) ? '1' : '0';
  }

  return data;
}

void printKeyBits() {
  if (!dbg) return;

  String bits;

  for (uint8_t key = 0; key < KEY_STATUS_BITS; key++) {
    bits += keyIsPressed(key) ? '1' : '0';
  }

  Serial.println("KEY BITS 00-23: " + bits);
  Serial.println("KS FRAME DATA: " + buildKeyStatus());
  printSeparator();
}

String buildIndicatorStatus() {
  String data;
  data += STATION_CHAR;
  data += "IS";

  for (uint8_t i = 0; i < INDICATOR_STATUS_COUNT; i++) {
    data += char('0' + constrain(indicatorState[i], 0, 4));
  }

  return data;
}

void printCommandDebug(const String &cmd, const String &data) {
  if (!dbg) return;

  String ackData;
  ackData += STATION_CHAR;
  ackData += '!';

  printSeparator();
  Serial.println("RX COMMAND DEBUG");
  Serial.println("RX DATA: " + data);
  Serial.println("COMMAND: " + cmd);
  Serial.println("TX ACK: " + noChecksumFramePreview(ackData));

  if (cmd == "GK") {
    Serial.println("MEANING: server asks current key/button status");
    Serial.println("TX DATA RESPONSE: " + framePreview(buildKeyStatus()));
  } else if (cmd == "SI") {
    Serial.println("MEANING: server sets indicator/LED states 00-23");
    Serial.println("TX DATA RESPONSE: none, ACK only");
  } else if (cmd == "GI") {
    Serial.println("MEANING: server asks current indicator/LED states");
    Serial.println("TX DATA RESPONSE: " + framePreview(buildIndicatorStatus()));
  } else if (cmd == "ST") {
    Serial.println("MEANING: server sets switch time");
    Serial.println("TX DATA RESPONSE: none, ACK only");
  } else if (cmd == "GT") {
    String response;
    response += STATION_CHAR;
    response += "TS";
    char value[4];
    snprintf(value, sizeof(value), "%03u", switchTimeHundredsMs);
    response += value;
    Serial.println("MEANING: server asks switch time");
    Serial.println("TX DATA RESPONSE: " + framePreview(response));
  } else if (cmd == "GV") {
    String response;
    response += STATION_CHAR;
    response += "VE";
    response += FIRMWARE_VERSION;
    Serial.println("MEANING: server asks firmware version");
    Serial.println("TX DATA RESPONSE: " + framePreview(response));
  } else if (cmd == "SP") {
    Serial.println("MEANING: server sets PTT timeout");
    Serial.println("TX DATA RESPONSE: none, ACK only");
  } else if (cmd == "BP") {
    Serial.println("MEANING: server buzzer/beep command");
    Serial.println("TX DATA RESPONSE: none, ACK only");
  } else if (cmd == "SO") {
    Serial.println("MEANING: server sets oscillator state");
    Serial.println("TX DATA RESPONSE: none, ACK only");
  } else if (cmd == "GO") {
    String response;
    response += STATION_CHAR;
    response += "OS";
    response += oscillatorOn ? '1' : '0';
    Serial.println("MEANING: server asks oscillator state");
    Serial.println("TX DATA RESPONSE: " + framePreview(response));
  } else {
    String nakData;
    nakData += STATION_CHAR;
    nakData += '@';
    Serial.println("MEANING: unknown/unsupported command");
    Serial.println("TX NAK: " + noChecksumFramePreview(nakData));
  }

  printSeparator();
}

void noteRxCommand(const String &cmd, const String &data) {
  if (dbg) {
    Serial.println("RX: " + cmd + " data=" + data);
    return;
  }

  if (cmd != "GO" && cmd != "GK" && cmd != "SI") {
    Serial.println("RX: " + cmd + " data=" + data);
  }
}

void requestAllNonBusyZones(const char* name) {
  // Lugar donde PTT y W.N empiezan rutina NO-ZONE paso a paso.
  // Variables cortas usadas: nzActive, nzWait, nzIndex, nzZone, pttAllRequest.
  dropQueuedZoneEvents("NO-ZONE START");
  pttAllRequest = true;
  nzActive = true;
  nzWait = false;
  nzIndex = 0;
  nzZone = 0;

  Serial.println(String(name) + " NO-ZONE: step request all non-busy zones");
  sendNextNoZoneRequest(name);
  printLogicState("STATE");
}

void sendNextNoZoneRequest(const char* name) {
  // Lugar donde se envia SOLO una zona por vez en NO-ZONE.
  // Variables cortas usadas: nzActive, nzWait, nzIndex, nzZone, bky.
  if (!nzActive || nzWait) return;

  while (nzIndex < ZONE_LED_COUNT) {
    uint8_t i = nzIndex;
    nzIndex++;

    if (zoneBusyByServer(i)) {
      busyTouchedZones[i] = true;
      busyMutedZones[i] = false;
      localBlinkZones[i] = true;
      busyLockZones[i] = true;
      busyLockClearSent[i] = false;
      continue;
    }

    markZoneRequested(i);
    queueKeyEvent(PACIS_BUTTON_KEYS[i], true);
    queueKeyEvent(PACIS_BUTTON_KEYS[i], false);
    nzWait = true;
    nzZone = i;
    Serial.println(String(name) + " NO-ZONE SEND Z" + String(i + 1));
    return;
  }

  nzActive = false;
  nzWait = false;
  Serial.println(String(name) + " NO-ZONE DONE");

  if (pttReleasePending && pttActive) {
    pttReleasePending = false;
    Serial.println("PTT RELEASE PENDING: NO-ZONE done, clear now");
    endSession("PTT", PTT_KEY_NUMBER);
  }
}

void sendNextClearRequest() {
  // Lugar donde se limpian zonas una por una antes de KU06/KU12.
  // Variables cortas usadas: clrActive, clrWait, clrNeed, clrIndex, clrZone, clrKey.
  if (!clrActive || clrWait) return;

  while (clrIndex < ZONE_LED_COUNT) {
    uint8_t i = clrIndex;
    clrIndex++;

    if (!clrNeed[i]) continue;

    queueKeyEvent(PACIS_BUTTON_KEYS[i], true);
    queueKeyEvent(PACIS_BUTTON_KEYS[i], false);
    clrWait = true;
    clrZone = i;
    Serial.println("CLEAR SEND Z" + String(i + 1));
    return;
  }

  queueKeyEvent(clrKey, false);
  Serial.println(String(clrKey == 12 ? "W.N" : "PTT") + " RELEASE FINAL");
  Serial.print(clrName);
  Serial.print(" END: serverClearToggle=");
  Serial.print(listBoolZones(clrServerZones));
  Serial.print(" prototypeClear=");
  Serial.print(listBoolZones(clrLocalZones));
  Serial.print(" busyKeptServer=");
  Serial.println(listBoolZones(clrKeepBusyZones));
  printLogicState("AFTER SESSION");
  clrActive = false;
  clrWait = false;
  clrIndex = 0;
  clrZone = 0;
  clrKey = 0;
  clrName = "";
}

void markZoneRequested(uint8_t zoneIndex) {
  // Lugar donde se marca zona pedida y se arma retry si servidor la pierde.
  // Variables cortas usadas: requestedZones, requestRetryCount, requestRetryMs.
  requestedZones[zoneIndex] = true;
  requestRetryCount[zoneIndex] = 0;
  requestRetryMs[zoneIndex] = millis();
}

void retryMissedRequestedZones() {
  // Lugar donde se reintenta una zona que sigue OFF durante PTT/W.N.
  // Variables cortas usadas: pttActive, whiteActive, requestedZones, rty, bky.
  if (!pttActive && !whiteActive) return;
  if (nzActive) return;

  uint32_t now = millis();
  for (uint8_t i = 0; i < ZONE_LED_COUNT; i++) {
    if (!requestedZones[i]) continue;
    if (indicatorState[ZONE_LED_INDICATORS[i]] != 0) continue;
    if (requestRetryCount[i] >= 2) continue;
    if (now - requestRetryMs[i] < ZONE_RETRY_MS) continue;

    requestRetryCount[i]++;
    requestRetryMs[i] = now;
    queueKeyEvent(PACIS_BUTTON_KEYS[i], true);
    queueKeyEvent(PACIS_BUTTON_KEYS[i], false);
    Serial.println("RETRY Z" + String(i + 1) + ": still OFF, resend request");
  }
}

String startSession(const char* name, uint8_t keyNumber) {
  // Lugar donde empieza una sesion de audio/ruido.
  // Variables cortas usadas: ptk/PTT, keyNumber, requestedZones, busyTouchedZones,
  // busyLockZones, localBlinkZones, pttAllRequest, pttActive, whiteActive, whiteStartMs.
  if (!anyServerApprovedZone()) {
    requestAllNonBusyZones(name);
  }

  String eventData = queueKeyEvent(keyNumber, true);

  if (keyNumber == PTT_KEY_NUMBER) {
    pttActive = true;
  }

  if (keyNumber == 12) {
    whiteActive = true;
    whiteStartMs = millis();
    writeWhiteLedColor(true, false);
  }

  Serial.println(String(name) + " START: active selected=" + listServerOkZones() + " busy=" + listServerBusyZones());
  return eventData;
}

String endSession(const char* name, uint8_t keyNumber) {
  // Lugar donde termina una sesion de audio/ruido.
  // Variables cortas usadas: requestedZones, busyTouchedZones, localBlinkZones,
  // indicatorState, selectedZones, clrNeed, clrKey, pttActive, whiteActive.
  dropQueuedZoneEvents("SESSION END");
  String eventData;
  eventData += STATION_CHAR;
  eventData += "KU";
  eventData += twoDigits(keyNumber);

  clrActive = true;
  clrWait = false;
  clrIndex = 0;
  clrZone = 0;
  clrKey = keyNumber;
  clrName = name;

  for (uint8_t i = 0; i < ZONE_LED_COUNT; i++) {
    clrNeed[i] = false;
    clrServerZones[i] = false;
    clrLocalZones[i] = false;
    clrKeepBusyZones[i] = false;

    if (zoneApprovedByServer(i)) {
      // Lugar donde se limpia cualquier zona aprobada por servidor.
      // No depende de requestedZones; si esta selected, se limpia.
      clrServerZones[i] = true;
      clrLocalZones[i] = true;
      clrNeed[i] = true;
      localBlinkZones[i] = false;
    } else if (requestedZones[i] && zoneBusyByServer(i)) {
      busyMutedZones[i] = true;
      localBlinkZones[i] = false;
      clrLocalZones[i] = true;
      clrKeepBusyZones[i] = true;
    }

    if (!requestedZones[i] && busyTouchedZones[i] && zoneBusyByServer(i)) {
      busyMutedZones[i] = true;
      localBlinkZones[i] = false;
      clrLocalZones[i] = true;
      clrKeepBusyZones[i] = true;
    }

    requestedZones[i] = false;
    requestRetryCount[i] = 0;
    requestRetryMs[i] = 0;
    busyTouchedZones[i] = false;
    blockedBusyPress[i] = false;
  }

  if (pttAllRequest) {
    pttAllRequest = false;
  }
  nzActive = false;
  nzWait = false;
  nzIndex = 0;
  nzZone = 0;

  clearLocalBlinkZones();

  if (keyNumber == PTT_KEY_NUMBER) {
    pttActive = false;
    pttReleasePending = false;
  }

  if (keyNumber == 12) {
    whiteActive = false;
    whiteStartMs = 0;
    writeWhiteLedColor(false, false);
  }

  sendNextClearRequest();

  return eventData;
}

void checkWhiteTimeout() {
  // Lugar donde W.N usa timeout SP/pto igual que PTT futuro.
  // Variables cortas usadas: whiteActive, whiteStartMs, pto.
  if (!whiteActive) return;

  uint32_t timeoutMs = static_cast<uint32_t>(pttTimeoutSeconds) * 1000UL;
  if (timeoutMs == 0) return;

  if (millis() - whiteStartMs >= timeoutMs) {
    String eventData = endSession("W.N TIMEOUT", 12);
    printActionDebug("WHITE", false, "WHITE timeout, relay OFF", eventData);
  }
}

void scanPacisButtons() {
  uint32_t now = millis();

  for (uint8_t i = 0; i < PACIS_BUTTON_COUNT; i++) {
    bool rawPressed = digitalRead(PACIS_BUTTON_PINS[i]) == LOW;

    if (rawPressed != pacisButtons[i].lastRawPressed) {
      pacisButtons[i].lastRawPressed = rawPressed;
      pacisButtons[i].lastChangeMs = now;
    }

    uint16_t debounceMs;
    if (i >= ZONE_LED_COUNT) {
      debounceMs = rawPressed ? MOMENT_PRESS_DEBOUNCE_MS : MOMENT_RELEASE_DEBOUNCE_MS;
    } else {
      debounceMs = rawPressed ? ZONE_PRESS_DEBOUNCE_MS : DEBOUNCE_MS;
    }

    if ((now - pacisButtons[i].lastChangeMs) >= debounceMs &&
        rawPressed != pacisButtons[i].stablePressed) {
      pacisButtons[i].stablePressed = rawPressed;

      if (i >= ZONE_LED_COUNT) {
        // Lugar donde W.N deja de ser momentario.
        // Variables cortas usadas: whiteActive, pttActive, whiteLastToggleMs, bky/key 12.
        if (rawPressed) {
          if (now - whiteLastToggleMs < WHITE_TOGGLE_GUARD_MS) {
            Serial.println("WHITE PRESS: ignored bounce/guard");
            continue;
          }
          whiteLastToggleMs = now;

          if (pttActive) {
            Serial.println("WHITE PRESS: blocked because PTT is active");
            printLogicState("STATE");
            continue;
          }

          String eventData = whiteActive
            ? endSession("W.N", PACIS_BUTTON_KEYS[i])
            : startSession("W.N", PACIS_BUTTON_KEYS[i]);

          const char* ledState = whiteActive
            ? "WHITE latch ON, relay/output ON"
            : "WHITE latch OFF, relay/output OFF";

          printActionDebug("WHITE", whiteActive, ledState, eventData);
        }
        continue;
      }

      if (rawPressed && i < ZONE_LED_COUNT) {
        if (busyLockZones[i] || zoneNotOffByServer(i) || selectedZones[i] || requestedZones[i]) {
          busyTouchedZones[i] = true;
          busyMutedZones[i] = false;
          localBlinkZones[i] = true;
          blockedBusyPress[i] = true;
          busyLockClearSent[i] = false;
          if (zoneBusyByServer(i) || busyLockZones[i]) {
            Serial.println("ZONE PRESS Z" + String(i + 1) + ": BUSY/BLINK LOCK, not selected");
          } else if (zoneApprovedByServer(i)) {
            Serial.println("ZONE PRESS Z" + String(i + 1) + ": already selected by server, no second click");
          } else {
            Serial.println("ZONE PRESS Z" + String(i + 1) + ": already requested/selected, no second click");
          }
          printLogicState("STATE");
          continue;
        }

        // Lugar donde se recuerda que esta zona fue pedida por el operador.
        // No prende LED aqui; se espera aprobacion SI del servidor.
        markZoneRequested(i);
        Serial.println("ZONE PRESS Z" + String(i + 1) + ": request sent to server");
        printLogicState("STATE");
      }

      if (!rawPressed && i < ZONE_LED_COUNT && blockedBusyPress[i]) {
        blockedBusyPress[i] = false;
        Serial.println("ZONE RELEASE Z" + String(i + 1) + ": busy was blocked, no server clear");
        printLogicState("STATE");
        continue;
      }

      String eventData = queueKeyEvent(PACIS_BUTTON_KEYS[i], rawPressed);

      const char* buttonName = "WHITE";
      const char* ledState = rawPressed ? "WHITE LED ON while button is held" : "WHITE LED OFF";

      if (i < ZONE_LED_COUNT) {
        buttonName = i == 0 ? "ZONE 1" :
                     i == 1 ? "ZONE 2" :
                     i == 2 ? "ZONE 3" :
                     i == 3 ? "ZONE 4" : "ZONE 5";
        ledState = rawPressed ? "request sent, waiting server SI approval" : "release sent, lamp still follows server SI";
      }

      printActionDebug(buttonName, rawPressed, ledState, eventData);
    }
  }
}

void scanPttButton() {
  // Lugar donde PTT actua momentario.
  // Variables cortas usadas: pttButton, pttActive, whiteActive, ptk.
  uint32_t now = millis();
  bool rawPressed = digitalRead(PTT_BUTTON_PIN) == LOW;

  if (rawPressed != pttButton.lastRawPressed) {
    pttButton.lastRawPressed = rawPressed;
    pttButton.lastChangeMs = now;
  }

  uint16_t debounceMs = rawPressed ? MOMENT_PRESS_DEBOUNCE_MS : MOMENT_RELEASE_DEBOUNCE_MS;

  if ((now - pttButton.lastChangeMs) >= debounceMs &&
      rawPressed != pttButton.stablePressed) {
    pttButton.stablePressed = rawPressed;

    if (rawPressed && whiteActive) {
      Serial.println("PTT PRESS: blocked because W.N is active");
      printLogicState("STATE");
      return;
    }

    if (!rawPressed && !pttActive) {
      Serial.println("PTT RELEASE: no active PTT session");
      return;
    }

    if (!rawPressed && nzActive) {
      pttReleasePending = true;
      Serial.println("PTT RELEASE PENDING: wait NO-ZONE DONE");
      return;
    }

    String eventData = rawPressed
      ? startSession("PTT", PTT_KEY_NUMBER)
      : endSession("PTT", PTT_KEY_NUMBER);

    const char* pttLedState = rawPressed
      ? "PTT session ON while held"
      : "PTT session OFF, clear sent";

    printActionDebug("PTT", rawPressed, pttLedState, eventData);
  }
}

void updateZoneLeds() {
  for (uint8_t i = 0; i < ZONE_LED_COUNT; i++) {
    uint8_t indicatorNumber = ZONE_LED_INDICATORS[i];
    uint8_t state = indicatorState[indicatorNumber];
    bool approved = zoneApprovedByServer(i);
    selectedZones[i] = approved;
    if (state == 0 || state == 1) {
      busyMutedZones[i] = false;
    }

    bool greenOn = false;
    bool redOn = false;
    bool localBlink = localBlinkZones[i];

    if (localBlink) {
      redOn = true;
    } else if (state == 1) {
      greenOn = true;
    } else if (state == 3) {
      redOn = true;
    } else if (state == 2 || state == 4) {
      // Lugar donde zona ocupada por servidor cambia a rojo solido.
      // Si fue busy antes de nosotros y ya soltamos PTT, se puede mutear localmente.
      // El servidor sigue con busy; aqui solo se apaga nuestra luz local.
      redOn = busyMutedZones[i] ? false : true;
    }

    writeZoneLedColor(i, greenOn, redOn);
  }
}

void clearLocalBlinkZones() {
  // Lugar donde PTT o White/noise limpian blink local despues de la accion.
  for (uint8_t i = 0; i < ZONE_LED_COUNT; i++) {
    localBlinkZones[i] = false;
    requestRetryCount[i] = 0;
    requestRetryMs[i] = 0;
  }
}

void updateLocalLeds() {
  // Lugar donde LEDs locales muestran sesion, no solo boton fisico.
  // Variables cortas usadas: pttActive, whiteActive, lpt, lwn.
  writePttLed(pttActive);
  writeWhiteLedColor(whiteActive, false);
}

void updateStatusLeds() {
  uint32_t now = millis();
  uint32_t lastPollSnapshot = lastPollTime;
  uint32_t failUntilSnapshot = failLedUntilMs;
  bool pollingActive = lastPollSnapshot != 0 && (now - lastPollSnapshot) <= POLL_HEARTBEAT_TIMEOUT_MS;
  bool heartbeatActive = RS485_INSTALLED ? pollingActive : true;
  bool heartbeatOn = heartbeatActive && ((now / HEARTBEAT_MS) % 2 == 0);
  bool failActive = failUntilSnapshot != 0 && static_cast<int32_t>(now - failUntilSnapshot) < 0;

  if (failUntilSnapshot != 0 && !failActive) {
    failLedUntilMs = 0;
  }

  bool statusOn = failActive ? ((now / 120) % 2 == 0) : heartbeatOn;
  uint8_t brightness = failActive ? FAIL_BRIGHTNESS_PERCENT : HEARTBEAT_BRIGHTNESS_PERCENT;
  writeStatusLed(statusOn, brightness);
}

void writeStatusLed(bool on, uint8_t brightnessPercent) {
  if (!on || brightnessPercent == 0) {
    digitalWrite(STATUS_LED_PIN, LOW);
    return;
  }

  if (brightnessPercent >= 100) {
    digitalWrite(STATUS_LED_PIN, HIGH);
    return;
  }

  uint8_t onTime = (SOFT_PWM_MS * brightnessPercent) / 100;
  if (onTime < 1) {
    onTime = 1;
  }

  bool pwmOn = (millis() % SOFT_PWM_MS) < onTime;
  digitalWrite(STATUS_LED_PIN, pwmOn ? HIGH : LOW);
}

void heartbeatTask(void *parameter) {
  (void)parameter;

  for (;;) {
    updateStatusLeds();
    vTaskDelay(1);
  }
}

void zoneLedTask(void *parameter) {
  (void)parameter;

  for (;;) {
    updateZoneLeds();
    vTaskDelay(5);
  }
}

void printLocalZoneState() {
  Serial.print("LOCAL ZONES: ");
  for (uint8_t i = 0; i < ZONE_LED_COUNT; i++) {
    Serial.print(selectedZones[i] ? '1' : '0');
  }
  Serial.println();
}

const char* siStateName(uint8_t state) {
  if (state == 0) return "OFF";
  if (state == 1) return "OK";
  if (state == 2) return "BUSY";
  if (state == 3) return "OK_BLINK";
  if (state == 4) return "BUSY_BLINK";
  return "?";
}

void captureZoneSi() {
  // Lugar donde se captura lo que el servidor manda para LEDs 07-11.
  // Esto prueba si el servidor limpia las zonas despues de soltar PTT.
  bool changed = false;
  for (uint8_t i = 0; i < ZONE_LED_COUNT; i++) {
    uint8_t state = indicatorState[ZONE_LED_INDICATORS[i]];
    bool wasSelected = selectedZones[i];
    bool blinkState = state == 3 || state == 4;

    selectedZones[i] = zoneApprovedByServer(i);

    if (zoneBusyByServer(i)) {
      // Lugar donde cualquier blink/busy pone candado 0/1 al boton.
      // Mientras este candado esta activo, un segundo push no manda comando.
      if (!busyLockZones[i]) {
        busyLockClearSent[i] = false;
      }
      busyLockZones[i] = true;
      if (requestedZones[i] || wasSelected || busyTouchedZones[i]) {
        localBlinkZones[i] = true;
      }
    }

    if (blinkState && (requestedZones[i] || wasSelected || busyTouchedZones[i]) && !busyLockClearSent[i]) {
      // Lugar donde la pista del servidor es el blink:
      // si una zona tocada/seleccionada empieza a parpadear, se deselecciona.
      requestedZones[i] = false;
      requestRetryCount[i] = 0;
      requestRetryMs[i] = 0;
      busyTouchedZones[i] = true;
      localBlinkZones[i] = true;
      blockedBusyPress[i] = true;
      busyLockZones[i] = true;
      busyLockClearSent[i] = true;
      queueKeyEvent(PACIS_BUTTON_KEYS[i], false);
      queueKeyEvent(PACIS_BUTTON_KEYS[i], true);
      queueKeyEvent(PACIS_BUTTON_KEYS[i], false);
      Serial.println("BLINK DUPLICATE CLEAR Z" + String(i + 1) + ": deselect sent");
    }

    if (requestedZones[i] && zoneBusyByServer(i)) {
      // Lugar donde una zona pedida empieza a parpadear o vuelve BUSY.
      // Regla simple: blink/busy significa no permitido, se deselecciona enseguida.
      requestedZones[i] = false;
      requestRetryCount[i] = 0;
      requestRetryMs[i] = 0;
      busyTouchedZones[i] = true;
      busyMutedZones[i] = false;
      localBlinkZones[i] = true;
      blockedBusyPress[i] = true;
      busyLockZones[i] = true;
      if (!busyLockClearSent[i]) {
        queueKeyEvent(PACIS_BUTTON_KEYS[i], false);
        queueKeyEvent(PACIS_BUTTON_KEYS[i], true);
        queueKeyEvent(PACIS_BUTTON_KEYS[i], false);
        busyLockClearSent[i] = true;
        Serial.println("BLINK CLEAR Z" + String(i + 1) + ": not allowed, deselect sent");
      } else {
        Serial.println("SERVER DENIED Z" + String(i + 1) + ": busy, request canceled");
      }
    }

    if (busyLockZones[i] && zoneApprovedByServer(i) && !busyLockClearSent[i]) {
      // Lugar donde se corrige una zona locked que el servidor dejo selected.
      // Se manda solo un click para quitar nuestra seleccion accidental.
      queueKeyEvent(PACIS_BUTTON_KEYS[i], true);
      queueKeyEvent(PACIS_BUTTON_KEYS[i], false);
      indicatorState[ZONE_LED_INDICATORS[i]] = 0;
      selectedZones[i] = false;
      busyLockClearSent[i] = true;
      Serial.println("BUSY LOCK CLEAR Z" + String(i + 1) + ": selected while locked");
    }

    if (busyLockZones[i] && state == 0 && digitalRead(PACIS_BUTTON_PINS[i]) == HIGH) {
      // Lugar donde se libera el lock cuando el servidor ya no marca selected/busy.
      // Nota: el blink local NO se limpia aqui. Solo PTT o W.N lo limpian.
      busyLockZones[i] = false;
      busyTouchedZones[i] = false;
      busyMutedZones[i] = false;
      blockedBusyPress[i] = false;
      busyLockClearSent[i] = false;
      Serial.println("BUSY LOCK OFF Z" + String(i + 1));
    }

    if (state != lastCapturedZoneSi[i]) {
      changed = true;
      lastCapturedZoneSi[i] = state;
    }
  }

  if (!changed) return;

  Serial.print("SERVER SI: selected=");
  Serial.print(listServerOkZones());
  Serial.print(" busy=");
  Serial.println(listServerBusyZones());

  if (nzActive && nzWait) {
    uint8_t state = indicatorState[ZONE_LED_INDICATORS[nzZone]];
    if (state != 0 || !requestedZones[nzZone]) {
      nzWait = false;
      sendNextNoZoneRequest(pttActive ? "PTT" : "W.N");
    }
  }

  if (clrActive && clrWait) {
    if (!zoneApprovedByServer(clrZone)) {
      clrNeed[clrZone] = false;
      clrWait = false;
      sendNextClearRequest();
    }
  }

  retryMissedRequestedZones();
}

void setAllPanelLeds(bool on) {
  for (uint8_t i = 0; i < ZONE_LED_COUNT; i++) {
    writeZoneLedColor(i, on, on);
  }

  writePttLed(on);
  writeWhiteLedColor(on, false);
  digitalWrite(STATUS_LED_PIN, on ? HIGH : LOW);
}

void runLampTest() {
  Serial.println("LAMP TEST: start");
  setAllPanelLeds(true);
  delay(LAMP_TEST_ALL_ON_MS);

  setAllPanelLeds(false);
  delay(150);

  for (uint8_t i = 0; i < ZONE_LED_COUNT; i++) {
    writeZoneLedColor(i, true, false);
    delay(LAMP_TEST_STEP_MS);
    writeZoneLedColor(i, false, true);
    delay(LAMP_TEST_STEP_MS);
    writeZoneLedColor(i, false, false);
  }

  writeWhiteLedColor(true, false);
  delay(LAMP_TEST_STEP_MS);
  writeWhiteLedColor(false, false);

  digitalWrite(STATUS_LED_PIN, HIGH);
  delay(LAMP_TEST_STEP_MS);
  digitalWrite(STATUS_LED_PIN, LOW);

  setAllPanelLeds(false);
  Serial.println("LAMP TEST: done");
  printSeparator();
}

void handlePollByte(uint8_t value) {
  uint8_t address = value & 0x07;

  if ((value & 0x80) == 0 || address != STATION_NUMBER) {
    return;
  }

  lastPollTime = millis();

  String eventData;
  if (dequeueEvent(eventData)) {
    if (dbg) {
      Serial.println("POLL 0x" + String(value, HEX) + " -> TX " + eventData);
      Serial.println("FRAME: " + framePreview(eventData));
    }
    sendDataFrame(eventData);
    if (dbg) {
      Serial.println("SENT: " + eventData);
      printSeparator();
    }
  } else {
    uint32_t now = millis();
    if (dbg && now - lastIdlePollDebugMs >= pld) {
      lastIdlePollDebugMs = now;
      printSeparator();
      Serial.println("RX POLL DEBUG");
      Serial.println("RX POLL: 0x" + String(value, HEX));
      Serial.println("RX POLL BITS: " + byteToBitText(value));
      Serial.println("MEANING: server poll for this station; no event waiting");
      Serial.println("TX: no event queued, echo poll byte");
      printSeparator();
    }
    rs485SendBegin();
    rs485.write(value);
    rs485SendEnd();
  }
}

bool validateDataFrame(String &data) {
  if (rxLen < 5) return false;

  if (rxBuffer[0] != STX || rxBuffer[rxLen - 1] != ETX) {
    return false;
  }

  uint8_t receivedChecksum = rxBuffer[rxLen - 2];
  data = "";

  for (uint8_t i = 1; i < rxLen - 2; i++) {
    data += char(rxBuffer[i]);
  }

  return receivedChecksum == calcPentaChecksum(data);
}

void handleSetIndicators(const String &data) {
  uint8_t payloadLength = data.length() > 3 ? data.length() - 3 : 0;
  uint8_t count = payloadLength < INDICATOR_STATUS_COUNT ? payloadLength : INDICATOR_STATUS_COUNT;

  for (uint8_t i = 0; i < count; i++) {
    char c = data[3 + i];
    if (c >= '0' && c <= '4') {
      indicatorState[i] = c - '0';
    }
  }

  captureZoneSi();
}
//--------------------------------------------------------

void handleCommand(const String &data) {
  if (data.length() < 3 || data[0] != STATION_CHAR) {
    Serial.println("RX BAD: " + data);
    sendNak();
    return;
  }

  String cmd = data.substring(1, 3);
  noteRxCommand(cmd, data);
  printCommandDebug(cmd, data);

  if (cmd == "GK") {
    sendAck();
    sendDataFrame(buildKeyStatus());
  } else if (cmd == "SI") {
    handleSetIndicators(data);
    sendAck();
  } else if (cmd == "GI") {
    sendAck();
    sendDataFrame(buildIndicatorStatus());
  } else if (cmd == "ST") {
    if (data.length() >= 6) {
      switchTimeHundredsMs = data.substring(3, 6).toInt();
    }
    sendAck();
  } else if (cmd == "GT") {
    sendAck();
    String response;
    response += STATION_CHAR;
    response += "TS";
    char value[4];
    snprintf(value, sizeof(value), "%03u", switchTimeHundredsMs);
    response += value;
    sendDataFrame(response);
  } else if (cmd == "GV") {
    sendAck();
    String response;
    response += STATION_CHAR;
    response += "VE";
    response += FIRMWARE_VERSION;
    sendDataFrame(response);
  } else if (cmd == "SP") {
    if (data.length() >= 5) {
      pttTimeoutSeconds = data.substring(3, 5).toInt();
    }
    sendAck();
  } else if (cmd == "BP") {
    sendAck();
  } else if (cmd == "SO") {
    if (data.length() >= 4) {
      oscillatorOn = data[3] == '1';
    }
    sendAck();
  } else if (cmd == "GO") {
    sendAck();
    String response;
    response += STATION_CHAR;
    response += "OS";
    response += oscillatorOn ? '1' : '0';
    sendDataFrame(response);
  } else {
    sendNak();
  }
}

void handleRxByte(uint8_t value) {
  if (!receivingFrame && (value & 0x80)) {
    handlePollByte(value);
    return;
  }

  if (value == STX) {
    receivingFrame = true;
    rxLen = 0;
    rxBuffer[rxLen++] = value;
    return;
  }

  if (!receivingFrame) return;

  if (rxLen < sizeof(rxBuffer)) {
    rxBuffer[rxLen++] = value;
  } else {
    receivingFrame = false;
    rxLen = 0;
    sendNak();
    return;
  }

  if (value == ETX) {
    String data;
    if (validateDataFrame(data)) {
      printTrafficDecode("RX", data, true);
      handleCommand(data);
    } else {
      sendNak();
    }

    receivingFrame = false;
    rxLen = 0;
  }
}

void setupButtonState(ButtonState &button, int pin, bool internalPullup) {
  pinMode(pin, internalPullup ? INPUT_PULLUP : INPUT);

  bool pressed = digitalRead(pin) == LOW;
  button.stablePressed = pressed;
  button.lastRawPressed = pressed;
  button.lastChangeMs = millis();
}

void setup() {
  Serial.begin(9600);
  delay(600);

  if (!setupMcp23017()) {
    while (true) {
      delay(1000);
    }
  }

  for (uint8_t i = 0; i < PACIS_BUTTON_COUNT; i++) {
    setupButtonState(pacisButtons[i], PACIS_BUTTON_PINS[i], PACIS_BUTTON_PINS[i] < 34);
  }

  setupButtonState(pttButton, PTT_BUTTON_PIN, PTT_BUTTON_PIN < 34);

  for (uint8_t i = 0; i < ZONE_LED_COUNT; i++) {
    writeZoneLedColor(i, false, false);
  }

  pinMode(STATUS_LED_PIN, OUTPUT);
  writePttLed(false);
  writeWhiteLedColor(false, false);
  digitalWrite(STATUS_LED_PIN, LOW);

  runLampTest();

  xTaskCreatePinnedToCore(
    heartbeatTask,
    "HeartbeatTask",
    2048,
    nullptr,
    1,
    &heartbeatTaskHandle,
    0
  );

  xTaskCreatePinnedToCore(
    zoneLedTask,
    "ZoneLedTask",
    2048,
    nullptr,
    1,
    &zoneLedTaskHandle,
    0
  );

  for (uint8_t i = 0; i < INDICATOR_STATUS_COUNT; i++) {
    indicatorState[i] = 0;
  }

  for (uint8_t i = 0; i < ZONE_LED_COUNT; i++) {
    selectedZones[i] = false;
    requestedZones[i] = false;
    busyTouchedZones[i] = false;
    busyMutedZones[i] = false;
    blockedBusyPress[i] = false;
    busyLockZones[i] = false;
    busyLockClearSent[i] = false;
    localBlinkZones[i] = false;
    requestRetryCount[i] = 0;
    requestRetryMs[i] = 0;
    clrNeed[i] = false;
    clrServerZones[i] = false;
    clrLocalZones[i] = false;
    clrKeepBusyZones[i] = false;
  }

  pttActive = false;
  nzActive = false;
  nzWait = false;
  nzIndex = 0;
  nzZone = 0;
  clrActive = false;
  clrWait = false;
  clrIndex = 0;
  clrZone = 0;
  clrKey = 0;
  clrName = "";
  whiteActive = false;
  pttReleasePending = false;
  whiteStartMs = 0;

  if (RS485_INSTALLED) {
    pinMode(RS485_EN_PIN, OUTPUT);
    digitalWrite(RS485_EN_PIN, LOW);
    rs485.begin(PACIS_BAUD, SERIAL_8N1, RS485_RX_PIN, RS485_TX_PIN);
  }

  Serial.println("PACIS JR ready");
  Serial.println("BUILD: " + String(LOCAL_BUILD_TAG));
  Serial.println("BUILD TIME: " + String(BUILD_STAMP));
  Serial.println("DEBUG: SERVER SI / ZONE PRESS / PTT RELEASE only");
  Serial.println("RS485: RX" + String(RS485_RX_PIN) + " TX" + String(RS485_TX_PIN) + " EN" + String(RS485_EN_PIN));
  Serial.println("I2C/MCP23017 LEDS: SDA" + String(I2C_SDA_PIN) + " SCL" + String(I2C_SCL_PIN) + " ADDR 0x20");
  Serial.println("ZONE LED TASK: core 0 color display");
  Serial.println("W.N: toggle session, first press ON, second press OFF, timeout uses SP/pto");
  printSeparator();
}

void loop() {
  scanPacisButtons();
  scanPttButton();
  checkWhiteTimeout();
  updateLocalLeds();

  if (RS485_INSTALLED) {
    uint8_t rxLimit = 24;
    while (rs485.available() && rxLimit > 0) {
      handleRxByte(static_cast<uint8_t>(rs485.read()));
      rxLimit--;
    }
  }
}

/*
  REMARKS DE NOMBRES CORTOS

  stx  = Lugar donde se guarda byte de inicio, 0x02
  etx  = Lugar donde se guarda byte de final, 0x03
  adr  = Lugar donde se guarda direccion PACIS de la estacion
  ach  = Lugar donde se guarda direccion PACIS como texto ASCII
  rx   = Lugar donde se lee RS485 desde el MAX3485
  tx   = Lugar donde se envia RS485 hacia el MAX3485
  en   = Lugar donde se controla MAX3485: LOW recibe, HIGH transmite
  br   = Lugar donde se guarda velocidad serial PACIS, 9600
  r485 = Lugar donde se activa o apaga RS485
  bct  = Lugar donde se guarda cantidad de botones PACIS sin PTT
  bpn  = Lugar donde se guarda lista de pines de botones
  bky  = Lugar donde se guarda lista de teclas PACIS
  ptt  = Lugar donde se lee boton PTT
  ptk  = Lugar donde se guarda numero de tecla PTT
  zct  = Lugar donde se guarda cantidad de LEDs de zona
  lpn  = Lugar donde se guarda lista de pines de LEDs
  ind  = Lugar donde se guarda lista de indicadores PACIS
  lpt  = Lugar donde se prende LED PTT
  lwn  = Lugar donde se prende LED White noise
  lhb  = Lugar donde se prende LED Heartbeat/fail
  kbt  = Lugar donde se guarda cantidad de bits de teclas
  ict  = Lugar donde se guarda cantidad de indicadores
  qsz  = Lugar donde se guarda tamano de cola de eventos
  db   = Lugar donde se guarda tiempo de debounce en ms
  dbz  = Lugar donde se guarda debounce rapido solo para press de zonas 1-5
  dbm  = Lugar donde se guarda debounce rapido para press PTT/W.N
  dbr  = Lugar donde se guarda debounce rapido para release PTT/W.N
  wgd  = Lugar donde se evita doble toggle por rebote del W.N
  rty  = Lugar donde se guarda espera antes de retry de zona perdida
  bl   = Lugar donde se guarda tiempo de blink normal en ms
  hb   = Lugar donde se guarda tiempo de heartbeat en ms
  hbp  = Lugar donde se guarda brillo de heartbeat
  flp  = Lugar donde se guarda brillo de fail
  pwm  = Lugar donde se guarda periodo PWM por software
  flt  = Lugar donde se guarda tiempo que fail queda prendido
  hbt  = Lugar donde se guarda timeout de poll para heartbeat
  lon  = Lugar donde se guarda tiempo de prueba con todos LEDs ON
  lst  = Lugar donde se guarda tiempo de prueba LED por LED
  pld  = Lugar donde se baja el ruido de debug de poll sin evento
  rsd  = Lugar donde se baja el ruido de debug RX repetido
  swt  = Lugar donde se guarda tiempo switch en unidades de 100 ms
  pto  = Lugar donde se guarda timeout PTT en segundos
  osc  = Lugar donde se guarda estado del oscilador
  ver  = Lugar donde se guarda version del firmware
  tag  = Lugar donde se guarda etiqueta/build del codigo
  bld  = Lugar donde se guarda dia y hora de cambio del codigo
  requestedZones = Lugar donde se recuerda zona pedida por este prototipo
  requestRetryCount = Lugar donde se cuenta retry de zona pedida que sigue OFF
  requestRetryMs = Lugar donde se guarda tiempo del ultimo retry de zona
  busyTouchedZones = Lugar donde se recuerda zona busy que el operador intento usar
  busyMutedZones = Lugar donde se apaga localmente blink busy despues de PTT
  blockedBusyPress = Lugar donde se bloquea KU si no se mando KD por busy previo
  cmdt = Lugar donde lee comandos y significados PACIS
  cmdn = Lugar donde se guarda cantidad de comandos en la tabla
*/


