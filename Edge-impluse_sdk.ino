#include <pc_inferencing.h>
#include <edge-impulse-sdk/classifier/ei_run_classifier.h>
float input_values[13];
void setup() {
   Serial.begin(115200);
    Serial.println("Initializing Edge Impulse Model...");

    signal_t signal;
    prepare_input(&signal);

    ei_impulse_result_t result;
    EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false);
    if (res != EI_IMPULSE_OK) {
        Serial.println("Error running Edge Impulse Model!");
        return;
  }

  Serial.println("Found MPU6050");
  pinMode(ledPin, OUTPUT);
   Serial.begin(115200);
    Serial.println("Initializing Edge Impulse Model...");

}
void prepare_input(signal_t *signal) {
    for (int i = 0; i < 13; i++) {
        signal->total_length = 13;
        signal->get_data = [](size_t offset, size_t length, float *out) {
            for (size_t i = 0; i < length; i++) {
                out[i] = input_values[offset + i];
            }
            return EI_IMPULSE_OK;
        };
    }
}
void loop() {
    delay(2000);  // Wait before running again
}