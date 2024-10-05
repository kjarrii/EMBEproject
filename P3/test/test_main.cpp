#include <Arduino.h>
#include <unity.h>

#include "PIcontroller.h"

void test_normal_flow(void) {
    PIController piController(0.5f, 1.0f, 0.1f);
    double controlSignal = piController.update(1000, 800);
    TEST_ASSERT_EQUAL(110, controlSignal);
}

void test_windup_prevention(void) {
    PIController piController(0.5f, 1.0f, 0.1f);
    double controlSignal;
    for (int i = 0; i < 50; i++) {
        controlSignal = piController.update(1000, 100); 
    }

    TEST_ASSERT_LESS_OR_EQUAL(255, controlSignal);
}

void test_zero_error(void) {
    PIController piController(0.5f, 1.0f, 0.1f);
    double controlSignal = piController.update(1000, 1000);
    TEST_ASSERT_EQUAL(0, controlSignal);
}

void test_reverse_direction(void) {
    PIController piController(0.5f, 1.0f, 0.1f);
    double controlSignal = piController.update(800, 1000);
    TEST_ASSERT_LESS_THAN(0, controlSignal);
}

void setup() {
    delay(2000);

    UNITY_BEGIN(); 

    RUN_TEST(test_normal_flow);
    RUN_TEST(test_windup_prevention);
    RUN_TEST(test_zero_error);
    RUN_TEST(test_reverse_direction);

    UNITY_END();
}

void loop() {
    delay(500);
}
