#include <Ethernet.h>
#include <ArduinoJson.h>
#include <IRremote.h>
#include <WebServer.h>

// Enter a MAC address for your controller below.
byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };
static uint8_t ip[] = { 192, 168, 1, 25 };
WebServer webserver("", 80);

IRsend irsend;
// color codes from - https://github.com/alistairallan/RgbIrLed/blob/master/readme
unsigned long WHITE = 0xF7E01F;
unsigned long GREEN = 0xF7A05F;
unsigned long RED = 0xF720DF;
unsigned long YELLOW = 0xF730CF;

enum State {PENDING, SUCCESS, UNSTABLE, FAILURE};

#ifdef DEBUG
/** 
 * Utility function to print address
 */
inline static void printIPAddress()
{
  Serial.println(F("*****"));
  Serial.println(Ethernet.localIP());
  Serial.println(F("*****"));
}
#else
#define printIPAddress()
#endif

/**
 * Callback for a state change
 */
void updateStatus(State state)
{
  switch(state)
  {
    case SUCCESS:
      irsend.sendNEC(GREEN, 32);
      break;
    case UNSTABLE:
      irsend.sendNEC(YELLOW, 32);
      break;
    case FAILURE:
      irsend.sendNEC(RED, 32);
      break;
    default:
      irsend.sendNEC(WHITE, 32);
      break;
  }
}

/**
 * Process a http request
 */
void statusCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (type != WebServer::POST)
  {
    server.httpFail();
    server.printf(F("invalid connection type"));
    return;
  }

  // read all data
  char json[4096] = {0};
  int count = 0;
  char ch = 0;
  while ((ch = server.read()) != -1) {
    json[count++] = ch;
  }

  // parse to json
  StaticJsonBuffer<1024> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  if (!root.success())
  {
    server.httpFail();
    server.printf(F("parseObject() failed"));
    return;
  }

  statusJsonCmd(server, root);
}

/**
 * Process a json post request
 */
void statusJsonCmd(WebServer &server, const JsonObject& root)
{
  // extract args from request
  const char* build_status = root["build"]["status"];
  const char* build_phase = root["build"]["phase"];

  // convert to state
  State cur_status = PENDING;
  if (strcmp(build_phase, "COMPLETED") == 0)
  {
    if (strcmp(build_status, "SUCCESS") == 0)
    {
      cur_status = SUCCESS;
    }
    else if (strcmp(build_status, "UNSTABLE") == 0)
    {
      cur_status = UNSTABLE;
    }
    else if (strcmp(build_status, "FAILURE") == 0)
    {
      cur_status = FAILURE;
    }
  }

  updateStatus(cur_status);
  server.httpSuccess();
  server.printf(F("status:%d"), cur_status);
}

/**
 * Arduino init entry point
 */
void setup()
{
#ifdef DEBUG
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  // this check is only needed on the Leonardo:
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
#endif

  // configure
  Ethernet.begin(mac, ip);
  webserver.addCommand("status", &statusCmd);
  webserver.begin();

  // set initial state
  updateStatus(PENDING);

  printIPAddress();
}

/**
 * Arduino event loop entry point
 */
void loop()
{
  // process incoming connections one at a time forever
  webserver.processConnection();
}
