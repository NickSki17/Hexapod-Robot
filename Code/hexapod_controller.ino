#include <Servo.h>

// ────────────────────────────────────────────────
// SERVO PIN ASSIGNMENTS
// ────────────────────────────────────────────────
const int LF_PIN = 2;
const int LM_PIN = 4;
const int LR_PIN = 7;
const int RF_PIN = 8;
const int RM_PIN = 10;
const int RR_PIN = 12;

// ────────────────────────────────────────────────
// SERVO OBJECTS
// ────────────────────────────────────────────────
Servo servoLF;
Servo servoLM;
Servo servoLR;
Servo servoRF;
Servo servoRM;
Servo servoRR;

// ────────────────────────────────────────────────
// CALIBRATION
// ────────────────────────────────────────────────
int neutralAngles[6] = { 90, 90, 90, 90, 90, 90 };  // LF, LM, LR, RF, RM, RR

int endGaitAngles[6] = { 70, 110, 68, 70, 110, 67 };

int servoDirection[6] = { 1, 1, 1, 1, 1, 1 };

const int STEP_TIME = 225;
const int SMOOTH_STEPS = 25;
int currentAngles[6];

// ────────────────────────────────────────────────
// HELPER FUNCTIONS
// ────────────────────────────────────────────────
int constrainAngle(int angle) {
  if (angle < 0) return 0;
  if (angle > 180) return 180;
  return angle;
}

void writeServoSafe(int servoIndex, int angle) {
  int neutral = neutralAngles[servoIndex];
  int offset = angle - neutral;
  offset *= servoDirection[servoIndex];
  angle = neutral + offset;

  angle = constrainAngle(angle);
  currentAngles[servoIndex] = angle;

  switch (servoIndex) {
    case 0: servoLF.write(angle); break;
    case 1: servoLM.write(angle); break;
    case 2: servoLR.write(angle); break;
    case 3: servoRF.write(angle); break;
    case 4: servoRM.write(angle); break;
    case 5: servoRR.write(angle); break;
  }
}

void setAllServosSmooth(int targetAngles[], int duration) {
  int startAngles[6];
  for (int i = 0; i < 6; i++) startAngles[i] = currentAngles[i];

  int steps = duration / 20;
  if (steps < 1) steps = 1;

  for (int step = 1; step <= steps; step++) {
    for (int i = 0; i < 6; i++) {
      int angle = startAngles[i] + ((targetAngles[i] - startAngles[i]) * step) / steps;
      writeServoSafe(i, angle);
    }
    delay(20);
  }
}

// ────────────────────────────────────────────────
// SETUP
// ────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.println("════════ HEXAPOD ANT ROBOT - START ════════");
  Serial.println();

  delay(2000);

  servoLF.attach(LF_PIN);
  servoLM.attach(LM_PIN);
  servoLR.attach(LR_PIN);
  servoRF.attach(RF_PIN);
  servoRM.attach(RM_PIN);
  servoRR.attach(RR_PIN);

  // 1. Initialize current angles to 0 before smooth move
  for (int i = 0; i < 6; i++) currentAngles[i] = 0;

  // 2. Move all servos smoothly to the calculated end-gait position
  Serial.print("Moving to End-Gait Position: {");
  for (int i = 0; i < 6; i++) {
    Serial.print(endGaitAngles[i]);
    if (i < 5) Serial.print(", ");
  }
  Serial.println("}");
  setAllServosSmooth(endGaitAngles, 1000);  // 1 second smooth transition

  // 3. Hold the end-gait position for approximately 7 seconds
  Serial.println("✓ All systems ready! Holding position for 7 seconds...");
  delay(7000);
  Serial.println("Starting continuous walk.");
}

// ────────────────────────────────────────────────
// TRIPOD GAIT
// ────────────────────────────────────────────────
void tripodStepContinuous() {

  // Tripod A (LF, RM, LR) Extremes
  int LF_start = neutralAngles[0] + 20;
  int LF_end = neutralAngles[0] - 20;
  int RM_start = neutralAngles[4] - 21;
  int RM_end = neutralAngles[4] + 20;
  int LR_start = neutralAngles[2] + 22;
  int LR_end = neutralAngles[2] - 22;

  // Tripod B (RF, RR, LM) Extremes
  int RF_start = neutralAngles[3] + 20;
  int RF_end = neutralAngles[3] - 20;
  int RR_start = neutralAngles[5] + 20;
  int RR_end = neutralAngles[5] - 23;
  int LM_start = neutralAngles[1] - 20;
  int LM_end = neutralAngles[1] + 20;

  const int steps = SMOOTH_STEPS;

  // Phase 2 (Starts first since we initialized to the end-of-Phase-1 position)
  // Tripod B (Stance/Push) SLOW, Tripod A (Swing/Return) FAST
  for (int step = 0; step <= steps; step++) {
    float tSlow = (float)step / steps;
    float tFast = (float)step / (steps / 4.0);
    if (tFast > 1.0) tFast = 1.0;

    // Tripod B - SLOW push (Stance phase)
    int angleRF = RF_end + tSlow * (RF_start - RF_end);
    int angleRR = RR_end + tSlow * (RR_start - RR_end);
    int angleLM = LM_end + tSlow * (LM_start - LM_end);

    writeServoSafe(3, angleRF);  // RF
    writeServoSafe(5, angleRR);  // RR
    writeServoSafe(1, angleLM);  // LM

    // Tripod A - FAST return (Swing phase)
    int angleLF = LF_end + tFast * (LF_start - LF_end);
    int angleRM = RM_end + tFast * (RM_start - RM_end);
    int angleLR = LR_end + tFast * (LR_start - LR_end);

    writeServoSafe(0, angleLF);  // LF
    writeServoSafe(4, angleRM);  // RM
    writeServoSafe(2, angleLR);  // LR

    delay(STEP_TIME / steps);
  }

  // Phase 1
  // Tripod A (Stance/Push) SLOW, Tripod B (Swing/Return) FAST
  for (int step = 0; step <= steps; step++) {
    float tSlow = (float)step / steps;
    float tFast = (float)step / (steps / 4.0);
    if (tFast > 1.0) tFast = 1.0;

    // Tripod A - SLOW push (Stance phase)
    int angleLF = LF_start + tSlow * (LF_end - LF_start);
    int angleRM = RM_start + tSlow * (RM_end - RM_start);
    int angleLR = LR_start + tSlow * (LR_end - LR_start);

    writeServoSafe(0, angleLF);  // LF
    writeServoSafe(4, angleRM);  // RM
    writeServoSafe(2, angleLR);  // LR

    // Tripod B - FAST return (Swing phase)
    int angleRF = RF_start + tFast * (RF_end - RF_start);
    int angleRR = RR_start + tFast * (RR_end - RR_start);
    int angleLM = LM_start + tFast * (LM_end - LM_start);

    writeServoSafe(3, angleRF);  // RF
    writeServoSafe(5, angleRR);  // RR
    writeServoSafe(1, angleLM);  // LM

    delay(STEP_TIME / steps);
  }
}

// ────────────────────────────────────────────────
// MAIN LOOP
// ────────────────────────────────────────────────
void loop() {
  // Continuous loop after the 10-second pause in setup
  tripodStepContinuous();
}
