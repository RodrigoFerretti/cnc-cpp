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
    for (const auto &axis : {
             &config.x,
             &config.y,
             &config.z,
             &config.a,
         })
    {
        AccelStepper stepper(AccelStepper::FULL2WIRE, axis->pul, axis->dir);

        stepper.setMaxSpeed(axis->max);

        stepper.moveTo(1000000);
        stepper.setSpeed(axis->max);

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
}
