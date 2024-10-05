#include "PIController.h"

PIController::PIController(double kp, double Ti, double dt)
    : kp(kp), Ti(Ti), dt(dt), integral(0), maxOutput(255) {}

double PIController::update(double ref, double actual) {
    double error = ref - actual;          
    integral += error * dt / Ti;  

    double controlSignal = kp * (error + integral);
    controlSignal = antiWindup(controlSignal);
    return controlSignal;

}

double PIController::antiWindup(double controlSignal) {
    if (controlSignal > maxOutput) {
        integral = maxOutput / kp - (1 / Ti);
        controlSignal = maxOutput;
    } else if (controlSignal < -maxOutput) {
        integral = -maxOutput / kp - (1 / Ti);
        controlSignal = -maxOutput;
    }
    return controlSignal;
}
