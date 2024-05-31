// Create a union to easily convert float to byte
typedef union {
  float number;
  uint8_t bytes[4];
} FLOATUNION_t;

// Create the variables you want to receive
FLOATUNION_t myValue1; // Actual value
FLOATUNION_t myValue2; // Setpoint

// Create the variables to send
FLOATUNION_t send1; // PID output
//FLOATUNION_t send2; // Setpoint

void setup() {
  // initialize serial, use the same baud rate in the Simulink Config block
  Serial.begin(115200);
}

void loop() {
  // Get the floats from serial
  myValue1.number = getFloat(); // Actual value
  myValue2.number = getFloat(); // Setpoint

  // PID Calculation
  float input = myValue1.number;
  float setpoint = myValue2.number;
  float output = calculatePID(input, setpoint);

  // Prepare values to send
  send1.number = output; // Sending PID output
 // send2.number = setpoint; // Sending setpoint
  
  // Print header: Important to avoid sync errors!
  Serial.write('A'); 
  
  // Print float data for PID output
  for (int i = 0; i < 4; i++) {
    Serial.write(send1.bytes[i]); 
  }

  // Print float data for setpoint
 // for (int i = 0; i < 4; i++) {
//    Serial.write(send2.bytes[i]); 
//  }
  
  // Print terminator
  Serial.print('\n');
  
  // Use the same delay in the Serial Receive block
  delay(50);
}

float getFloat() {
  int cont = 0;
  FLOATUNION_t f;
  while (cont < 4) {
    if (Serial.available()) {
      f.bytes[cont] = Serial.read();
      cont++;
    }
  }
  return f.number;
}

// PID control variables
float previousError = 0;
float integral = 0;

// PID constants
const float Kp = 30.0;  // Proportional gain
const float Ki = 6.0;  // Integral gain
const float Kd = 2.0; // Derivative gain

float calculatePID(float input, float setpoint) {
  // Calculate error
  float error = setpoint - input;

  // Proportional term
  float P = Kp * error;

  // Integral term
  integral += error * Ki;

  // Derivative term
  float derivative = (error - previousError) * Kd;

  // Calculate total output
  float output = P + integral + derivative;

  // Save the current error as previous error for next iteration
  previousError = error;

  return output;
}