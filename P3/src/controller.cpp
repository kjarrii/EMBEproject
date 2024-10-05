class PController {
public:
    PController(double kp);  // Constructor to set proportional gain
    double update(double ref, double actual);  // Update control law
    
private:
    double kp;  // Proportional gain
};

// Constructor
PController::PController(double kp) : kp(kp) {}

// Update method for proportional control
double PController::update(double ref, double actual) {
    double error = ref - actual;  // Calculate the error
    return kp * error;            // Apply proportional control law
}
