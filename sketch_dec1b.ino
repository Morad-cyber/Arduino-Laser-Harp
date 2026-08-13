/*
 * PROJEKT: Laser Harp
 * PLATTFORM: Arduino Mega 2560
 * AUTOR: Team 2 / Berkel
 * DATUM: 27.01.2026
 *
 * BESCHREIBUNG:
 * Steuerung einer Laserharfe mit 8 Saiten.
 * Steuerung von Licht, Hardware-I/O und State-Management.
 */

#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

// ------------------------------------------------------------
// KONFIGURATION
// ------------------------------------------------------------

#define NUM_STRINGS 8
#define THRESHOLD 150  // Schwellenwert für Lichtunterbrechung

// Hardware Pin Zuweisung
const int pin_transistor = 2;  // Schaltet die Laser an/aus
const int pin_switch_main = 3; // Hauptschalter
const int pin_led_status = 10; // Status-LED (System an/aus)

const int pin_ldr[NUM_STRINGS] = {A0, A2, A3, A4, A5, A6, A7, A8};
const int pins_btn[3] = {4, 5, 6}; // Taster für Skalenauswahl
const int pins_led[3] = {7, 8, 9}; // LEDs fuer Skalenauswahl

// Musikalische Skalen (MIDI Notenwerte)
const int scales[3][NUM_STRINGS] = {
    {64, 65, 67, 69, 71, 72, 74, 76}, // Skala 1: E Phrygisch
    {60, 62, 64, 65, 67, 69, 71, 72}, // Skala 2: C Dur
    {57, 59, 60, 62, 64, 65, 68, 69}  // Skala 3: A Harmonisch Moll
};

// Datenstruktur für den Systemzustand
typedef struct {
    bool system_active;              // Ist die Harfe an?
    int current_scale;               // aktuell gewählte Skala (0-2)
    int base_line[NUM_STRINGS];      // Kalibrierungswerte (Licht-Nullpunkt)
    bool note_playing[NUM_STRINGS];  // Merker, ob Note gerade spielt
} harp_context_t;

static harp_context_t ctx; // Globaler Speicher für den Zustand

// ------------------------------------------------------------
// PROTOTYPEN
// ------------------------------------------------------------

void init_hardware_pins(void);
void check_power_switch(harp_context_t *c);
void toggle_system_state(harp_context_t *c);
void check_scale_buttons(harp_context_t *c);
void update_scale_leds(int scale_index);
void scan_all_strings(harp_context_t *c);
void process_string_logic(harp_context_t *c, int idx);
void stop_all_sound(harp_context_t *c);

// ------------------------------------------------------------
// HAUPTPROGRAMM
// ------------------------------------------------------------

void setup() {
    /* Use: Einmalige Initialisierung beim Start. */

    // 1. MIDI starten (startet standardmaessig mit 31250)
    MIDI.begin(MIDI_CHANNEL_OMNI);

    // 2. WICHTIG: Geschwindigkeit für Hairless ueberschreiben!
    Serial1.begin(115200);

    init_hardware_pins();

    // Startwerte setzen
    ctx.system_active = false;
    ctx.current_scale = 0;

    // LED der ersten Skala einschalten
    update_scale_leds(0);
}

void loop() {
    /* Use: Endlosschleife, steuert den Ablauf. */

    check_power_switch(&ctx);

    if (ctx.system_active) {
        // Nur wenn an: Skalen waehlen und spielen
        check_scale_buttons(&ctx);
        scan_all_strings(&ctx);
    } else {
        // Wenn aus: Nur Skalen waehlen erlaubt (Vorbereitung)
        check_scale_buttons(&ctx);
    }

    delay(5); // Stabilisierung fuer Analog-Wandler
}

// ------------------------------------------------------------
// HARDWARE & I/O
// ------------------------------------------------------------

void init_hardware_pins(void) {
    /* Use: Konfiguriert alle Ein- und Ausgaenge. */
    pinMode(pin_transistor, OUTPUT);
    pinMode(pin_led_status, OUTPUT);
    pinMode(pin_switch_main, INPUT_PULLUP);

    for (int i = 0; i < 3; i++) {
        pinMode(pins_btn[i], INPUT_PULLUP);
        pinMode(pins_led[i], OUTPUT);
    }

    // Sicherstellen, dass alles aus ist beim Start
    digitalWrite(pin_transistor, LOW);
    digitalWrite(pin_led_status, LOW);
}

void update_scale_leds(int active_scale) {
    /* Use: Schaltet nur die LED der aktiven Skala an. */
    for (int i = 0; i < 3; i++) {
        if (i == active_scale) {
            digitalWrite(pins_led[i], HIGH);
        } else {
            digitalWrite(pins_led[i], LOW);
        }
    }
}

// ------------------------------------------------------------
// SYSTEM LOGIK
// ------------------------------------------------------------

void check_power_switch(harp_context_t *c) {
    /* Use: Prueft Taster, entprellt und wechselt Zustand. */
    if (digitalRead(pin_switch_main) == LOW) {
        delay(50); // Einfaches Entprellen

        if (digitalRead(pin_switch_main) == LOW) {
            toggle_system_state(c);

            // Warten bis Taster losgelassen wird
            while (digitalRead(pin_switch_main) == LOW) {
                delay(10);
            }
        }
    }
}

void toggle_system_state(harp_context_t *c) {
    /* Use: Schaltet zwischen AN und AUS um. */
    if (c->system_active) {
        // Ausschalten
        c->system_active = false;
        digitalWrite(pin_transistor, LOW);
        digitalWrite(pin_led_status, LOW);
        stop_all_sound(c);
    } else {
        // Einschalten
        c->system_active = true;
        digitalWrite(pin_transistor, HIGH);
        digitalWrite(pin_led_status, HIGH);

        delay(400); // Warten bis Laser volle Helligkeit haben
        calibrate_sensors(c);
    }
}

void calibrate_sensors(harp_context_t *c) {
    /* Use: Speichert die Helligkeit bei offenen Laser als Nullpunkt. */
    for (int i = 0; i < NUM_STRINGS; i++) {
        // Wir lesen den Wert als Referenz
        c->base_line[i] = analogRead(pin_ldr[i]);
        c->note_playing[i] = false;
    }
}

void check_scale_buttons(harp_context_t *c) {
    /* Use: Prüft, ob ein Skalen-Taster gedrückt wurde. */
    for (int i = 0; i < 3; i++) {
        // Wenn Taster gedrückt UND Skala ist noch nicht aktiv
        if (digitalRead(pins_btn[i]) == LOW && c->current_scale != i) {
            stop_all_sound(c); // Alte Toene stoppen (Panic)

            c->current_scale = i;
            update_scale_leds(i);

            delay(200); // Entprellen des Tasters
        }
    }
}

void stop_all_sound(harp_context_t *c) {
    /* Use: Sendet NoteOff fuer alle Saiten (Panic-Funktion). */
    for (int s = 0; s < 3; s++) {
        for (int i = 0; i < NUM_STRINGS; i++) {
            MIDI.sendNoteOff(scales[c->current_scale][i], 0, 1);
        }
    }

    // Internen Status zuruecksetzen
    for (int i = 0; i < NUM_STRINGS; i++) {
        c->note_playing[i] = false;
    }
}

// ------------------------------------------------------------
// AUDIO VERARBEITUNG
// ------------------------------------------------------------

void scan_all_strings(harp_context_t *c) {
    /* Use: Ruft die Logik fuer jede einzelne Saite auf. */
    for (int i = 0; i < NUM_STRINGS; i++) {
        process_string_logic(c, i);
    }
}

void process_string_logic(harp_context_t *c, int idx) {
    /* Use: Entscheidet, ob Note AN oder AUS sein soll. */
    int current_val = analogRead(pin_ldr[idx]);
    int diff = c->base_line[idx] - current_val; // Differenz zum Nullpunkt
    int note = scales[c->current_scale][idx];

    // Logik: Ist der Laserstrahl unterbrochen?
    if (diff > THRESHOLD && !c->note_playing[idx]) {
        // Note starten (Velocity 127 = Maximum)
        MIDI.sendNoteOn(note, 127, 1);
        c->note_playing[idx] = true;
    }
    // Hysterese: Schwelle minus 50 verhindert Flackern am Rand
    else if (diff < (THRESHOLD - 50) && c->note_playing[idx]) {
        // Note stoppen
        MIDI.sendNoteOff(note, 0, 1);
        c->note_playing[idx] = false;
    }
}
