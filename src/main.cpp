#include <AccelStepper.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

AsyncWebServer server(80);
AsyncWebSocket socket("/ws");

struct AxisConfig
{
    int dir;
    int pul;
    int hom;
    int lim;
    int max;
    AccelStepper stepper;
};

struct Config
{
    AxisConfig x;
    AxisConfig y;
    AxisConfig z;
    AxisConfig a;
};

Config config = {
    .x = {
        .dir = 17,
        .pul = 5,
        .hom = 22,
        .lim = 23,
        .max = 10000000,
    },
    .y = {
        .dir = 27,
        .pul = 14,
        .hom = 32,
        .lim = 33,
        .max = 10000000,
    },
    .z = {
        .dir = 4,
        .pul = 16,
        .hom = 18,
        .lim = 19,
        .max = 10000000,
    },
    .a = {
        .dir = 25,
        .pul = 26,
        .hom = 34,
        .lim = 35,
        .max = 10000000,
    },

};

void processG01(String parameters)
{
    int xIndex = parameters.indexOf('X');
    int yIndex = parameters.indexOf('Y');
    int zIndex = parameters.indexOf('Z');
    int fIndex = parameters.indexOf('F');

    float x = parameters.substring(xIndex + 1, yIndex).toFloat();
    float y = parameters.substring(yIndex + 1, zIndex).toFloat();
    float z = parameters.substring(zIndex + 1, fIndex).toFloat();
    float f = parameters.substring(fIndex + 1).toFloat();

    config.x.stepper.moveTo(x);
    config.y.stepper.moveTo(y);
    config.z.stepper.moveTo(z);
    config.a.stepper.moveTo(x);

    for (const auto &axis : {
             &config.x,
             &config.y,
             &config.z,
             &config.a,
         })
    {
        axis->stepper.setSpeed(f);
    }
}

void processGCode(String command)
{
    char commandType = command.charAt(0);
    command.remove(0, 1);

    switch (commandType)
    {
    case 'G':
        if (command.startsWith("01"))
        {
            processG01(command.substring(2));
        }

        break;
    }
}
void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    switch (type)
    {
    case WS_EVT_CONNECT:
        Serial.println("WebSocket client connected");
        break;
    case WS_EVT_DISCONNECT:
        Serial.println("WebSocket client disconnected");
        break;
    case WS_EVT_ERROR:
        Serial.println("WebSocket error");
        break;
    case WS_EVT_DATA:
    {
        AwsFrameInfo *info = (AwsFrameInfo *)arg;

        if (info->opcode == WS_TEXT)
        {
            String message = String((char *)data);
            Serial.println("Received WebSocket message");
            processGCode(message);
        }

        break;
    }
    default:
        break;
    }
}
void setup()
{
    Serial.begin(115200);

    IPAddress localIP(192, 168, 0, 182);
    IPAddress gateway(192, 168, 0, 1);
    IPAddress subnet(255, 255, 0, 0);
    IPAddress primaryDNS(8, 8, 8, 8);
    IPAddress secondaryDNS(8, 8, 4, 4);

    WiFi.config(localIP, gateway, subnet, primaryDNS, secondaryDNS);

    WiFi.mode(WIFI_STA);
    WiFi.begin("", "");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
    }

    Serial.println("Wifi connected");

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/plain", "Hello, world"); });

    server.onNotFound([](AsyncWebServerRequest *request)
                      { request->send(404, "text/plain", "Not found"); });

    socket.onEvent(onWebSocketEvent);

    server.addHandler(&socket);

    server.begin();

    for (const auto &axis : {
             &config.x,
             &config.y,
             &config.z,
             &config.a,
         })
    {
        AccelStepper stepper(AccelStepper::FULL2WIRE, axis->pul, axis->dir);

        stepper.setMaxSpeed(axis->max);

        axis->stepper = stepper;

        pinMode(axis->hom, INPUT_PULLDOWN);
        pinMode(axis->lim, INPUT_PULLDOWN);
    }
}

void loop()
{
    for (const auto &axis : {
             &config.x,
             &config.y,
             &config.z,
             &config.a,
         })
    {
        axis->stepper.runSpeedToPosition();
    }

    socket.cleanupClients();
}
