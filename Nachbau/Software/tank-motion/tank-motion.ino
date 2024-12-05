// Enable debug messages. Comment this line to disable debugging.
#define DEBUG

// Macros for debug printing. If DEBUG is not defined, these will do nothing.
#ifdef DEBUG
  #define DEBUG_PRINT(x) Serial.print(x)      // Debug: Print without newline
  #define DEBUG_PRINTLN(x) Serial.println(x) // Debug: Print with newline
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

// Include necessary libraries for stepper motor control and mathematical functions
#include <AccelStepper.h>
#include <math.h>

// Pin definitions for stepper motor drivers and homing switches
#define STEP_PIN_X 3
#define DIR_PIN_X 4
#define BUTTON_PIN_X 13 // Homing switch for the X-axis

#define STEP_PIN_Y 8
#define DIR_PIN_Y 9
#define BUTTON_PIN_Y 2  // Homing switch for the Y-axis

// Create stepper motor objects for the X and Y axes
AccelStepper stepperX(AccelStepper::DRIVER, STEP_PIN_X, DIR_PIN_X);
AccelStepper stepperY(AccelStepper::DRIVER, STEP_PIN_Y, DIR_PIN_Y);

// Steps per millimeter for each axis
const float stepsPerMmX = 38.7;
const float stepsPerMmY = 87.79;

// Global variables for position tracking and mode
bool isAbsoluteMode = true; // Default mode: Absolute positioning
float currentX = 0;         // Current X position relative to the homing position
float currentY = 0;         // Current Y position relative to the homing position

// Idle timer variables
unsigned long lastCommandTime = 0;  // Timestamp of the last received command
const unsigned long idleTimeout = 10000; // 10 seconds idle timeout
bool motorsEnabled = true;          // Motor status: true if enabled

// G-code script to be executed automatically
const char* gCodeScript[] = {
    "G28",                // Homing
    "G1 X1 Y20 F800",     // Move to the desired start position
    "G92 X0 Y0",          // Set current position to (0,0)
    "G91",                // Switch to relative mode

    // 1. Line to the right
    "G1 X25 Y0 F800",     // Move 20 mm to the left
    "G1 X0 Y5 F800",     // Move 5 mm to the front
    "G1 X-25 Y0 F800",     // Move 20 mm to the right
    "G1 X0 Y5 F800",     // Move 20 mm to the front
    "G1 X25 Y0 F800",     // Move 20 mm to the left
    "G1 X0 Y5 F800",     // Move 5 mm to the front
    "G1 X-25 Y0 F800",     // Move 20 mm to the right
    "G1 X0 Y5 F800",     // Move 20 mm to the front
    "G1 X25 Y0 F800",     // Move 20 mm to the left
    "G1 X0 Y5 F800",     // Move 5 mm to the front
    "G1 X-25 Y0 F800",     // Move 20 mm to the right
    "G1 X0 Y5 F800",     // Move 20 mm to the front
    "G1 X25 Y0 F800",     // Move 20 mm to the left
    "G1 X0 Y5 F800",     // Move 5 mm to the front
    "G1 X-25 Y0 F800",     // Move 20 mm to the right
    // "G1 X0 Y5 F800",     // Move 5 mm to the front
    // "G1 X-25 Y0 F800",     // Move 20 mm to the right
    // "G1 X0 Y5 F800",     // Move 20 mm to the front
    // "G1 X25 Y0 F800",     // Move 20 mm to the left
    // "G1 X0 Y5 F800",     // Move 5 mm to the front
    // "G1 X-25 Y0 F800",     // Move 20 mm to the right
    // "G1 X0 Y5 F800",     // Move 20 mm to the front
    // "G1 X25 Y0 F800",     // Move 20 mm to the left
    // "G1 X0 Y5 F800",     // Move 5 mm to the front
    // "G1 X-25 Y0 F800",     // Move 20 mm to the right
    // "G1 X0 Y5 F800",     // Move 20 mm to the front
    // "G1 X25 Y0 F800",     // Move 20 mm to the left
    // "G1 X0 Y5 F800",     // Move 5 mm to the front
    // "G1 X-25 Y0 F800",     // Move 20 mm to the right
    // "G1 X0 Y5 F800",     // Move 20 mm to the front
    // "G1 X25 Y0 F800",     // Move 20 mm to the left
    // "G1 X0 Y5 F800",     // Move 5 mm to the front
    // "G1 X-25 Y0 F800",     // Move 20 mm to the right

    "G1 X0 Y-40 F800",
    "G1 X0.5 Y0 F800",
    "G1 X0 Y45 F800",
    "G1 X0.5 Y0 F800",
    "G1 X0 Y-45 F800",
    "G1 X0.5 Y0 F800",
    "G1 X0 Y45 F800",
    "G1 X0.5 Y0 F800",
    "G1 X0 Y-45 F800",
    "G1 X0.5 Y0 F800",
    "G1 X0 Y45 F800",
    "G1 X0.5 Y0 F800",
    "G1 X0 Y-45 F800",
    "G1 X0.5 Y0 F800",
    "G1 X0 Y45 F800",
    "G1 X0.5 Y0 F800",
    "G1 X0 Y-45 F800",
    "G1 X0.5 Y0 F800",
    "G1 X0 Y45 F800",
    "G1 X0.5 Y0 F800",
    "G1 X0 Y-45 F800",
    "G1 X0.5 Y0 F800",
    "G1 X0 Y45 F800",
    "G1 X0.5 Y0 F800",
    "G1 X0 Y-45 F800",
    "G1 X0.5 Y0 F800",
    "G1 X0 Y45 F800",
    "G1 X0.5 Y0 F800",
    "G1 X0 Y-45 F800",
    "G1 X0.5 Y0 F800",
    "G1 X0 Y45 F800",
    "G1 X0.5 Y0 F800",
    "G1 X0 Y-45 F800",
    "G1 X0.5 Y0 F800",
    "G1 X0 Y45 F800",
    "G1 X0.5 Y0 F800",
    "G1 X0 Y-45 F800",
    "G1 X0.5 Y0 F800",
    "G1 X0 Y45 F800",
    "M2"                  // Program end, disable motors
};
const int gCodeScriptSize = sizeof(gCodeScript) / sizeof(gCodeScript[0]); // Script size
int currentScriptLine = 0; // Current line in the G-code script
bool scriptRunning = true; // Flag indicating whether the script is running

const unsigned long commandDelay = 100; // Delay (in ms) between commands

// Arduino setup function
void setup() {
    Serial.begin(115200); // Initialize serial communication at 115200 baud

    pinMode(BUTTON_PIN_X, INPUT_PULLUP); // Configure X-axis button as input with pull-up resistor
    pinMode(BUTTON_PIN_Y, INPUT_PULLUP); // Configure Y-axis button as input with pull-up resistor

    // Set stepper motor parameters: maximum speed and acceleration
    stepperX.setMaxSpeed(300);
    stepperX.setAcceleration(200);
    stepperY.setMaxSpeed(300);
    stepperY.setAcceleration(300);

    // Enable motor outputs
    stepperX.enableOutputs();
    stepperY.enableOutputs();

    Serial.println("G-Code-Skript wird ausgeführt..."); // Notify script start
    lastCommandTime = millis(); // Initialize the last command timestamp

    scriptRunning = true; // Start the G-code script
}

// Arduino loop function
void loop() {
    // Execute the script if it is running
    static unsigned long lastCommandMillis = 0; // Last command execution timestamp
    if (scriptRunning) {
        if (currentScriptLine < gCodeScriptSize) { // Check if there are commands left
            if (millis() - lastCommandMillis >= commandDelay) { // Ensure delay between commands
                String command = String(gCodeScript[currentScriptLine]); // Get the current command
                lastCommandTime = millis(); // Update the last command timestamp
                enableMotors(); // Re-enable motors if they were disabled
                processGCode(command); // Process the G-code command
                currentScriptLine++; // Move to the next line
                lastCommandMillis = millis(); // Update the timestamp
            }
        } else {
            scriptRunning = false; // Script execution finished
        }
    }

    // Check for motor idle timeout
    if (motorsEnabled && (millis() - lastCommandTime > idleTimeout)) {
        disableMotors(); // Disable motors if inactive
    }

    // Optional: Process commands received via serial input
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n'); // Read input command
        lastCommandTime = millis(); // Update the last command timestamp
        enableMotors(); // Re-enable motors if they were disabled
        processGCode(input); // Process the received command
    }
}

// Process G-code commands
void processGCode(String command) {
    command.trim(); // Remove whitespace
    if (command.length() == 0) return; // Ignore empty commands

    DEBUG_PRINTLN("Empfangener Befehl: " + command); // Debug: Print received command

    // Split command into code and parameters
    int spaceIndex = command.indexOf(' ');
    String code;
    String params;
    if (spaceIndex > 0) {
        code = command.substring(0, spaceIndex);
        params = command.substring(spaceIndex + 1);
    } else {
        code = command;
        params = "";
    }
    code.toUpperCase(); // Convert command code to uppercase

    // Handle specific G-code commands
    if (code == "G28") {
        startHoming(); // Homing command
    } else if (code == "G90") {
        isAbsoluteMode = true; // Switch to absolute mode
        DEBUG_PRINTLN("Absoluter Modus aktiviert.");
    } else if (code == "G91") {
        isAbsoluteMode = false; // Switch to relative mode
        DEBUG_PRINTLN("Relativer Modus aktiviert.");
    } else if (code == "G92") {
        setCurrentPosition(params); // Set the current position
    } else if (code == "G1") {
        handleLinearMove(params); // Linear movement
    } else if (code == "G3") {
        handleArcMove(params, true); // Arc movement (clockwise or counterclockwise)
    } else if (code == "M2") {
        disableMotors(); // End program and disable motors
        scriptRunning = false;
        DEBUG_PRINTLN("Programm beendet. Motoren deaktiviert.");
    } else {
        DEBUG_PRINTLN("Unbekannter Befehl: " + code); // Unknown command
    }
}
// Start the homing process for X and Y axes
void startHoming() {
    DEBUG_PRINTLN("Referenzfahrt gestartet..."); // Debug: Homing process started
    enableMotors(); // Ensure motors are enabled

    // Homing X-axis
    DEBUG_PRINTLN("Homing X-Achse beginnt...");
    stepperX.setSpeed(-200); // Set a negative speed to move towards the homing switch
    while (digitalRead(BUTTON_PIN_X) == HIGH) { // Continue moving until the switch is triggered
        stepperX.runSpeed();
    }
    stepperX.stop(); // Stop the motor
    stepperX.setCurrentPosition(0); // Reset the current position to 0
    DEBUG_PRINTLN("Homing X-Achse abgeschlossen.");

    // Homing Y-axis
    DEBUG_PRINTLN("Homing Y-Achse beginnt...");
    stepperY.setSpeed(-200); // Set a negative speed to move towards the homing switch
    while (digitalRead(BUTTON_PIN_Y) != HIGH) { // Continue moving until the switch is triggered
        stepperY.runSpeed();
    }
    stepperY.stop(); // Stop the motor
    stepperY.setCurrentPosition(0); // Reset the current position to 0
    DEBUG_PRINTLN("Homing Y-Achse abgeschlossen.");

    // Reset global position variables
    currentX = 0;
    currentY = 0;

    DEBUG_PRINTLN("Referenzpunkt gesetzt auf (0,0)."); // Debug: Homing point set
}

// Set the current position based on G92 parameters
void setCurrentPosition(String params) {
    float x = currentX; // Default X position
    float y = currentY; // Default Y position

    // Parse the parameters for new positions
    parseParameters(params, &x, &y, nullptr);

    // Update global position variables
    currentX = x;
    currentY = y;

    // Update stepper motor positions
    stepperX.setCurrentPosition(currentX * stepsPerMmX);
    stepperY.setCurrentPosition(currentY * stepsPerMmY);

    DEBUG_PRINTLN("Aktuelle Position gesetzt auf X=" + String(currentX) + ", Y=" + String(currentY)); // Debug
}

// Handle linear movements (G1 command)
void handleLinearMove(String params) {
    // Start with current positions
    float x = isAbsoluteMode ? currentX : 0;
    float y = isAbsoluteMode ? currentY : 0;
    float f = -1; // Default speed: no change

    // Parse movement parameters
    parseParameters(params, &x, &y, &f);

    // If in relative mode, calculate target positions relative to the current positions
    if (!isAbsoluteMode) {
        x += currentX;
        y += currentY;
    }

    // Set speed if specified
    if (f > 0) {
        float speedX = f / 60.0 * stepsPerMmX; // Convert feed rate to steps/second
        float speedY = f / 60.0 * stepsPerMmY;
        stepperX.setMaxSpeed(speedX);
        stepperY.setMaxSpeed(speedY);
    }

    // Move to the calculated coordinates
    moveToCoordinates(x, y);

    // Update current positions
    currentX = x;
    currentY = y;
}

// Move steppers to specified coordinates
void moveToCoordinates(float xTarget, float yTarget) {
    enableMotors(); // Ensure motors are enabled

    // Convert target positions to steps
    long xSteps = xTarget * stepsPerMmX;
    long ySteps = yTarget * stepsPerMmY;

    // Set target positions for each stepper
    stepperX.moveTo(xSteps);
    stepperY.moveTo(ySteps);

    // Run motors until they reach the target positions
    while (stepperX.distanceToGo() != 0 || stepperY.distanceToGo() != 0) {
        stepperX.run();
        stepperY.run();
    }

    DEBUG_PRINTLN("Bewegung abgeschlossen."); // Debug: Movement completed
    DEBUG_PRINT("Neue Position: X = ");
    DEBUG_PRINT(currentX);
    DEBUG_PRINT(", Y = ");
    DEBUG_PRINTLN(currentY);
}

// Parse parameters for linear or arc movements
void parseParameters(String params, float *x, float *y, float *f) {
    int index = 0;
    params += " "; // Add a trailing space to simplify parsing
    while (index < params.length()) {
        char code = params.charAt(index); // Read the parameter letter (e.g., X, Y, F)
        index++;
        int start = index;

        // Find the end of the parameter value
        while (index < params.length() && params.charAt(index) != ' ') {
            index++;
        }
        String valueStr = params.substring(start, index); // Extract value as string
        float value = valueStr.toFloat(); // Convert to float

        // Assign the value to the corresponding parameter
        switch (code) {
            case 'X': *x = value; break;
            case 'Y': *y = value; break;
            case 'F': if (f != nullptr) *f = value; break;
            default: break;
        }
        index++; // Move to the next parameter
    }
}

// Parse parameters specifically for arc movements (G3 command)
void parseParametersForArc(String params, float *x, float *y, float *i, float *j, float *f) {
    int index = 0;
    params += " "; // Add a trailing space to simplify parsing
    while (index < params.length()) {
        char code = params.charAt(index); // Read the parameter letter (e.g., X, Y, I, J, F)
        index++;
        int start = index;

        // Find the end of the parameter value
        while (index < params.length() && params.charAt(index) != ' ') {
            index++;
        }
        String valueStr = params.substring(start, index); // Extract value as string
        float value = valueStr.toFloat(); // Convert to float

        // Assign the value to the corresponding parameter
        switch (code) {
            case 'X': *x = value; break;
            case 'Y': *y = value; break;
            case 'I': *i = value; break;
            case 'J': *j = value; break;
            case 'F': if (f != nullptr) *f = value; break;
            default: break;
        }
        index++; // Move to the next parameter
    }
}

// Handle arc movements (G3 command)
void handleArcMove(String params, bool counterClockwise) {
    float x = currentX;
    float y = currentY;
    float i = 0;
    float j = 0;
    float f = -1;

    // Parse the parameters for arc movement
    parseParametersForArc(params, &x, &y, &i, &j, &f);

    // Adjust for relative mode if necessary
    if (!isAbsoluteMode) {
        x += currentX;
        y += currentY;
    }

    // Calculate the center of the arc
    float centerX = currentX + i;
    float centerY = currentY + j;

    float radius = sqrt(i * i + j * j); // Calculate the radius

    // Calculate the start and end angles
    float startAngle = atan2(currentY - centerY, currentX - centerX);
    float endAngle = atan2(y - centerY, x - centerX);

    // Adjust end angle for counterclockwise movement
    if (counterClockwise && endAngle <= startAngle) {
        endAngle += TWO_PI;
    } else if (!counterClockwise && endAngle >= startAngle) {
        endAngle -= TWO_PI;
    }

    // Split the arc into segments for smoother movement
    int segments = 20; // Number of segments (adjustable)
    for (int s = 1; s <= segments; s++) {
        float t = (float)s / segments; // Calculate interpolation factor
        float angle = startAngle + t * (endAngle - startAngle);
        float newX = centerX + radius * cos(angle); // Calculate interpolated X position
        float newY = centerY + radius * sin(angle); // Calculate interpolated Y position

        moveToCoordinates(newX, newY); // Move to the interpolated position
    }

    // Update current positions
    currentX = x;
    currentY = y;

    DEBUG_PRINTLN("Kreisbogenbewegung abgeschlossen. Neue Position: X=" + String(currentX) + ", Y=" + String(currentY)); // Debug
}

// Enable motor outputs
void enableMotors() {
    if (!motorsEnabled) {
        stepperX.enableOutputs();
        stepperY.enableOutputs();
        motorsEnabled = true;
        DEBUG_PRINTLN("Motoren aktiviert."); // Debug: Motors enabled
    }
}

// Disable motor outputs to save power
void disableMotors() {
    if (motorsEnabled) {
        stepperX.disableOutputs();
        stepperY.disableOutputs();
        motorsEnabled = false;
        DEBUG_PRINTLN("Motoren deaktiviert (Leerlaufstrom reduziert)."); // Debug: Motors disabled
    }
}
