#ifndef PI_CONTROLLER_H
#define PI_CONTROLLER_H

#include "Controller.h"

class PIController : public Controller {
public:
    PIController(double kp, double Ti, double dt);
    double update(double ref, double actual) override;

private:
    double kp;  
    double Ti;
    double dt;
    double integral;
    double maxOutput;

    double antiWindup(double controlSignal);
};

#endif
