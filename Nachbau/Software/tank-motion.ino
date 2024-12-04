#define DEBUG  // Kommentieren Sie diese Zeile aus, um den DEBUG-Modus zu deaktivieren

#ifdef DEBUG
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

#include <AccelStepper.h>
#include <math.h>

// Define pin connections
#define STEP_PIN_X 3
#define DIR_PIN_X 4
#define BUTTON_PIN_X 13 // Schalter für Referenzfahrt X-Achse

#define STEP_PIN_Y 8
#define DIR_PIN_Y 9
#define BUTTON_PIN_Y 2 // Schalter für Referenzfahrt Y-Achse

AccelStepper stepperX(AccelStepper::DRIVER, STEP_PIN_X, DIR_PIN_X);
AccelStepper stepperY(AccelStepper::DRIVER, STEP_PIN_Y, DIR_PIN_Y);

// Schritte pro mm
const float stepsPerMmX = 38.7;
const float stepsPerMmY = 87.79;

// Globale Variablen
bool isAbsoluteMode = true; // Standard: Absoluter Modus
float currentX = 0; // Aktuelle X-Position relativ zur Homing-Position
float currentY = 0; // Aktuelle Y-Position relativ zur Homing-Position

// Inaktivitäts-Timer
unsigned long lastCommandTime = 0; // Zeitstempel des letzten empfangenen Befehls
const unsigned long idleTimeout = 10000; // 10 Sekunden Inaktivitätszeit
bool motorsEnabled = true; // Status der Motoren

// G-Code-Skript
const char* gCodeScript[] = {
    "G28",                // Referenzfahrt
    "G1 X1 Y20 F800",     // Fahre zur gewünschten Startposition
    "G92 X0 Y0",          // Setze aktuelle Position auf (0,0)
    "G91",                // Relativer Modus

    // 1. Gerade nach rechts
    "G1 X20 Y0 F800",     // Fahre 20 mm nach rechts

    // 2. Halbkreis (aufgeteilt für bessere Auflösung)
    "G3 X10 Y10 I-10 J10 F600", // Erste Viertelkreisbewegung
    "G3 X0 Y20 I-10 J0 F600",   // Zweite Viertelkreisbewegung

    // 3. Gerade zurück
    "G1 X-20 Y0 F800",    // Fahre 20 mm zurück

    // 4. Optional: Rückkehr zum Startpunkt
    "G1 X0 Y-20 F800",    // Fahre zurück zur Startposition

    "M2"                  // Programmende, Motoren deaktivieren
};

const int gCodeScriptSize = sizeof(gCodeScript) / sizeof(gCodeScript[0]);
int currentScriptLine = 0; // Aktuelle Zeile im G-Code-Skript
bool scriptRunning = true; // Skript läuft jetzt automatisch

const unsigned long commandDelay = 100; // 100 Millisekunden zwischen den Befehlen

void setup() {
    Serial.begin(115200);

    pinMode(BUTTON_PIN_X, INPUT_PULLUP);
    pinMode(BUTTON_PIN_Y, INPUT_PULLUP);

    stepperX.setMaxSpeed(300);
    stepperX.setAcceleration(100);
    stepperY.setMaxSpeed(300);
    stepperY.setAcceleration(100);

    stepperX.enableOutputs();
    stepperY.enableOutputs();

    Serial.println("G-Code-Skript wird ausgeführt...");
    lastCommandTime = millis(); // Initialisieren Sie den Zeitstempel

    scriptRunning = true; // Skript starten
}

void loop() {
    // Skript verarbeiten, wenn es läuft
    static unsigned long lastCommandMillis = 0;
    if (scriptRunning) {
        if (currentScriptLine < gCodeScriptSize) {
            if (millis() - lastCommandMillis >= commandDelay) {
                String command = String(gCodeScript[currentScriptLine]);
                lastCommandTime = millis(); // Aktualisieren Sie den Zeitstempel
                enableMotors(); // Motoren aktivieren, falls sie deaktiviert waren
                processGCode(command);
                currentScriptLine++;
                lastCommandMillis = millis();
            }
        } else {
            scriptRunning = false; // Skript beendet
        }
    }

    // Überprüfen Sie die Inaktivitätszeit
    if (motorsEnabled && (millis() - lastCommandTime > idleTimeout)) {
        disableMotors();
    }

    // Optional: Befehle von der seriellen Schnittstelle akzeptieren
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        lastCommandTime = millis(); // Aktualisieren Sie den Zeitstempel
        enableMotors(); // Motoren aktivieren, falls sie deaktiviert waren
        processGCode(input);
    }
}

void processGCode(String command) {
    command.trim();
    if (command.length() == 0) return;

    DEBUG_PRINTLN("Empfangener Befehl: " + command);

    // Befehl und Parameter trennen
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
    code.toUpperCase();

    if (code == "G28") {
        startHoming();
    } else if (code == "G90") {
        isAbsoluteMode = true;
        DEBUG_PRINTLN("Absoluter Modus aktiviert.");
    } else if (code == "G91") {
        isAbsoluteMode = false;
        DEBUG_PRINTLN("Relativer Modus aktiviert.");
    } else if (code == "G92") {
        setCurrentPosition(params);
    } else if (code == "G1") {
        handleLinearMove(params);
    } else if (code == "G3") {
        handleArcMove(params, true);
    } else if (code == "M2") {
        disableMotors();
        scriptRunning = false; // Skript stoppen
        DEBUG_PRINTLN("Programm beendet. Motoren deaktiviert.");
    } else {
        DEBUG_PRINTLN("Unbekannter Befehl: " + code);
    }
}

void startHoming() {
    DEBUG_PRINTLN("Referenzfahrt gestartet...");
    enableMotors(); // Motoren aktivieren

    // Homing X-Achse
    DEBUG_PRINTLN("Homing X-Achse beginnt...");
    stepperX.setSpeed(-200);
    while (digitalRead(BUTTON_PIN_X) == HIGH) {
        stepperX.runSpeed();
        // Optional: DEBUG_PRINTLN("Warte auf Endschalter X...");
    }
    stepperX.stop();
    stepperX.setCurrentPosition(0);
    DEBUG_PRINTLN("Homing X-Achse abgeschlossen.");

    // Homing Y-Achse
    DEBUG_PRINTLN("Homing Y-Achse beginnt...");
    stepperY.setSpeed(-200);
    while (digitalRead(BUTTON_PIN_Y) != HIGH) {
        stepperY.runSpeed();
        // Optional: DEBUG_PRINTLN("Warte auf Endschalter Y...");
    }
    stepperY.stop();
    stepperY.setCurrentPosition(0);
    DEBUG_PRINTLN("Homing Y-Achse abgeschlossen.");

    currentX = 0;
    currentY = 0;

    DEBUG_PRINTLN("Referenzpunkt gesetzt auf (0,0).");
}

void setCurrentPosition(String params) {
    float x = currentX;
    float y = currentY;

    parseParameters(params, &x, &y, nullptr);

    currentX = x;
    currentY = y;

    stepperX.setCurrentPosition(currentX * stepsPerMmX);
    stepperY.setCurrentPosition(currentY * stepsPerMmY);

    DEBUG_PRINTLN("Aktuelle Position gesetzt auf X=" + String(currentX) + ", Y=" + String(currentY));
}

void handleLinearMove(String params) {
    float x = isAbsoluteMode ? currentX : 0;
    float y = isAbsoluteMode ? currentY : 0;
    float f = -1; // Standardmäßig keine Änderung der Geschwindigkeit

    parseParameters(params, &x, &y, &f);

    if (!isAbsoluteMode) {
        x += currentX;
        y += currentY;
    }

    if (f > 0) {
        float speedX = f / 60.0 * stepsPerMmX;
        float speedY = f / 60.0 * stepsPerMmY;
        stepperX.setMaxSpeed(speedX);
        stepperY.setMaxSpeed(speedY);
    }

    moveToCoordinates(x, y);

    currentX = x;
    currentY = y;
}

void moveToCoordinates(float xTarget, float yTarget) {
    enableMotors(); // Sicherstellen, dass Motoren aktiviert sind

    long xSteps = xTarget * stepsPerMmX;
    long ySteps = yTarget * stepsPerMmY;

    stepperX.moveTo(xSteps);
    stepperY.moveTo(ySteps);

    while (stepperX.distanceToGo() != 0 || stepperY.distanceToGo() != 0) {
        stepperX.run();
        stepperY.run();
    }

    DEBUG_PRINTLN("Bewegung abgeschlossen.");
    DEBUG_PRINT("Neue Position: X = ");
    DEBUG_PRINT(currentX);
    DEBUG_PRINT(", Y = ");
    DEBUG_PRINTLN(currentY);
}

void parseParameters(String params, float *x, float *y, float *f) {
    int index = 0;
    params += " ";
    while (index < params.length()) {
        char code = params.charAt(index);
        index++;
        int start = index;
        while (index < params.length() && params.charAt(index) != ' ') {
            index++;
        }
        String valueStr = params.substring(start, index);
        float value = valueStr.toFloat();

        switch (code) {
            case 'X':
                *x = value;
                break;
            case 'Y':
                *y = value;
                break;
            case 'F':
                if (f != nullptr) *f = value;
                break;
            default:
                break;
        }
        index++;
    }
}

void parseParametersForArc(String params, float *x, float *y, float *i, float *j, float *f) {
    int index = 0;
    params += " ";
    while (index < params.length()) {
        char code = params.charAt(index);
        index++;
        int start = index;
        while (index < params.length() && params.charAt(index) != ' ') {
            index++;
        }
        String valueStr = params.substring(start, index);
        float value = valueStr.toFloat();

        switch (code) {
            case 'X':
                *x = value;
                break;
            case 'Y':
                *y = value;
                break;
            case 'I':
                *i = value;
                break;
            case 'J':
                *j = value;
                break;
            case 'F':
                if (f != nullptr) *f = value;
                break;
            default:
                break;
        }
        index++;
    }
}

void handleArcMove(String params, bool counterClockwise) {
    // Einfachheitshalber implementieren wir den Bogen als eine Serie von Geraden
    float x = currentX;
    float y = currentY;
    float i = 0;
    float j = 0;
    float f = -1;

    parseParametersForArc(params, &x, &y, &i, &j, &f);

    if (!isAbsoluteMode) {
        x += currentX;
        y += currentY;
    }

    float centerX = currentX + i;
    float centerY = currentY + j;

    float radius = sqrt(i * i + j * j);

    // Berechnung des Start- und Endwinkels
    float startAngle = atan2(currentY - centerY, currentX - centerX);
    float endAngle = atan2(y - centerY, x - centerX);

    // Anpassung des Winkels für gegen den Uhrzeigersinn
    if (counterClockwise && endAngle <= startAngle) {
        endAngle += TWO_PI;
    } else if (!counterClockwise && endAngle >= startAngle) {
        endAngle -= TWO_PI;
    }

    // Aufteilen des Bogens in Segmente
    int segments = 20; // Anzahl der Segmente (anpassbar)
    for (int s = 1; s <= segments; s++) {
        float t = (float)s / segments;
        float angle = startAngle + t * (endAngle - startAngle);
        float newX = centerX + radius * cos(angle);
        float newY = centerY + radius * sin(angle);

        moveToCoordinates(newX, newY);
    }

    currentX = x;
    currentY = y;

    DEBUG_PRINTLN("Kreisbogenbewegung abgeschlossen. Neue Position: X=" + String(currentX) + ", Y=" + String(currentY));
}

void enableMotors() {
    if (!motorsEnabled) {
        stepperX.enableOutputs();
        stepperY.enableOutputs();
        motorsEnabled = true;
        DEBUG_PRINTLN("Motoren aktiviert.");
    }
}

void disableMotors() {
    if (motorsEnabled) {
        stepperX.disableOutputs();
        stepperY.disableOutputs();
        motorsEnabled = false;
        DEBUG_PRINTLN("Motoren deaktiviert (Leerlaufstrom reduziert).");
    }
}
