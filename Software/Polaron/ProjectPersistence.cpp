#include <SD.h>
#include "ProjectPersistence.h"
#include "ArduinoJson-v6.11.0.h"
#include "Sequencer.h"

void ProjectPersistence::init(){
    int attempts = 4;
    while (!sdCardInitialized && attempts-- > 0) {
        sdCardInitialized = SD.begin(BUILTIN_SDCARD);
        Serial.println(F("Failed to initialize SD library"));
        delay(1000);
    }
    if (sdCardInitialized){
        Serial.println(F("SD lib initialized"));
    } else {
        Serial.println(F("Failed to initialize SD library, giving up"));
    }
}

void ProjectPersistence::save(const char *filename, Sequencer * sequencer){
        // Delete existing file, otherwise the configuration is appended to the file
        SD.remove(filename);

        // Open file for writing
        File file = SD.open(filename, FILE_WRITE);
        if (!file) {
            Serial.println(F("Failed to create file"));
            return;
        }
        // Allocate a temporary JsonDocument
        // Don't forget to change the capacity to match your requirements.
        // Use arduinojson.org/assistant to compute the capacity.
        StaticJsonDocument<100000> doc;
        JsonArray tracks = doc.createNestedArray(F("tracks"));
        for (int t = 0; t < NUMBER_OF_INSTRUMENTTRACKS; t++){
            JsonObject track = tracks.createNestedObject();
            JsonArray patterns = track.createNestedArray(F("patterns"));
            for (int p = 0; p < 1; p++){
                JsonObject pattern = patterns.createNestedObject();
                SequencerPattern & sequencerPattern = sequencer->tracks[t].patterns[p];
                pattern[F("triggerState")] = sequencerPattern.triggerState;
                pattern[F("pLockArmState")] = sequencerPattern.pLockArmState;
                pattern[F("offset")] = sequencerPattern.offset;
                pattern[F("trackLength")] = sequencerPattern.trackLength;
                JsonArray steps = pattern.createNestedArray(F("steps"));
                for (int s = 0; s < NUMBER_OF_STEPS_PER_PATTERN; s++){
                    JsonObject step = steps.createNestedObject();
                    JsonArray stepParams = step.createNestedArray(F("params"));
                    SequencerStep & sequencerStep = sequencerPattern.steps[s];
                    stepParams.add(sequencerStep.parameter1);
                    stepParams.add(sequencerStep.parameter2);
                    stepParams.add(sequencerStep.parameter3);
                    stepParams.add(sequencerStep.parameter4);
                    stepParams.add(sequencerStep.parameter5);
                    stepParams.add(sequencerStep.parameter6);
                }
            }
        }

        //serializeJsonPretty(doc, Serial);

        // Serialize JSON to file
        if (serializeJson(doc, file) == 0) {
            Serial.println(F("Failed to write to file"));
        }

        // Close the file
        file.close();
    };

void ProjectPersistence::load(const char *filename, Sequencer * sequencer){
        // Open file for writing
        File file = SD.open(filename, FILE_READ);
        if (!file) {
            Serial.println(F("Failed to read file"));
            return;
        }
        // Allocate a temporary JsonDocument
        // Don't forget to change the capacity to match your requirements.
        // Use arduinojson.org/assistant to compute the capacity.
        StaticJsonDocument<100000> doc;
        
        DeserializationError err = deserializeJson(doc, file);
        // Parse succeeded?
        if (err) {
            Serial.print(F("deserializeJson() returned "));
            Serial.println(err.c_str());
            return;
        }

        JsonArray tracks = doc[F("tracks")];
        for (int t = 0; t < NUMBER_OF_INSTRUMENTTRACKS; t++){
            JsonObject track = tracks[t];
            JsonArray patterns = track[F("patterns")];
            for (int p = 0; p < 1; p++){
                JsonObject pattern = patterns[p];
                SequencerPattern & sequencerPattern = sequencer->tracks[t].patterns[p];
                sequencerPattern.triggerState = pattern[F("triggerState")];
                sequencerPattern.pLockArmState = pattern[F("pLockArmState")];
                sequencerPattern.offset = pattern[F("offset")];
                sequencerPattern.trackLength = pattern[F("trackLength")];
                JsonArray steps = pattern[F("steps")];
                for (int s = 0; s < NUMBER_OF_STEPS_PER_PATTERN; s++){
                    JsonObject step = steps[s];
                    JsonArray stepParams = step[F("params")];
                    SequencerStep & sequencerStep = sequencerPattern.steps[s];
                    sequencerStep.parameter1 = stepParams[0];
                    sequencerStep.parameter2 = stepParams[1];
                    sequencerStep.parameter3 = stepParams[2];
                    sequencerStep.parameter4 = stepParams[3];
                    sequencerStep.parameter5 = stepParams[4];
                    sequencerStep.parameter6 = stepParams[5];
                }
            }
        }

        // Close the file
        file.close();
    };