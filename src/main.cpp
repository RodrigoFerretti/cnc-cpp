#include <AccelStepper.h>
#include <MultiStepper.h>

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
        .max = 10000,
    },
    .y = {
        .dir = 27,
        .pul = 14,
        .hom = 32,
        .lim = 33,
        .max = 10000,
    },
    .z = {
        .dir = 4,
        .pul = 16,
        .hom = 18,
        .lim = 19,
        .max = 10000,
    },
    .a = {
        .dir = 25,
        .pul = 26,
        .hom = 34,
        .lim = 35,
        .max = 10000,
    },

};

void setup()
{
    Serial.begin(115200);

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
    if (Serial.available() > 0)
    {
        String gCodeCommand = Serial.readStringUntil('\n');
        processGCode(gCodeCommand);
    }

    for (const auto &axis : {
             &config.x,
             &config.y,
             &config.z,
             &config.a,
         })
    {
        axis->stepper.runSpeedToPosition();
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

void processG01(String parameters)
{
    int xIndex = parameters.indexOf('X');
    int yIndex = parameters.indexOf('Y');
    int zIndex = parameters.indexOf('Z');

    float x = (xIndex != -1) ? parameters.substring(xIndex + 1, yIndex).toFloat() : 0.0;
    float y = (yIndex != -1) ? parameters.substring(yIndex + 1, zIndex).toFloat() : 0.0;
    float z = (zIndex != -1) ? parameters.substring(zIndex + 1).toFloat() : 0.0;

    config.x.stepper.moveTo(x);
    config.y.stepper.moveTo(y);
    config.z.stepper.moveTo(z);
    config.a.stepper.moveTo(x);
}
