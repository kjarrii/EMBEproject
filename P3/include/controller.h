#ifndef P_CONTROLLER_H
#define P_CONTROLLER_H

class PController {
public:
    PController(double kp);  // Constructor to set proportional gain
    double update(double ref, double actual);  // Update control signal based on reference and actual speed
    
private:
    double kp;  // Proportional gain
};

#endif
