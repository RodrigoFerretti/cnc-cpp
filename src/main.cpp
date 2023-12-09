#include <AccelStepper.h>
#include <MultiStepper.h>

struct AxisConfig
{
    int dir;
    int pul;
    int hom;
    int lim;
    int acc;
    int max;
    int inv;
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
        .dir = 25,
        .pul = 26,
        .hom = 34,
        .lim = 35,
        .acc = 2000,
        .max = 2000,
        .inv = 1,
    },
    .y = {
        .dir = 27,
        .pul = 14,
        .hom = 32,
        .lim = 33,
        .acc = 2000,
        .max = 2000,
        .inv = 0,
    },
    .z = {
        .dir = 4,
        .pul = 16,
        .hom = 18,
        .lim = 19,
        .acc = 2000,
        .max = 2000,
        .inv = 0,
    },
    .a = {
        .dir = 17,
        .pul = 5,
        .hom = 22,
        .lim = 23,
        .acc = 2000,
        .max = 2000,
        .inv = 0,
    },
};

MultiStepper steppers = MultiStepper();

void setup()
{
    Serial.begin(115200);

    for (const auto &axis : {&config.x, &config.y, &config.z, &config.a})
    {
        AccelStepper stepper(AccelStepper::FULL2WIRE, axis->pul, axis->dir);

        stepper.setMaxSpeed(axis->max);
        stepper.setAcceleration(axis->acc);
        stepper.setPinsInverted(axis->inv);

        pinMode(axis->hom, INPUT_PULLDOWN);
        pinMode(axis->lim, INPUT_PULLDOWN);

        steppers.addStepper(stepper);
    }

    long position[4] = {
        100000,
        100000,
        100000,
        100000,
    };

    steppers.moveTo(position);
}

void loop()
{
    steppers.run();
}
