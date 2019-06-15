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
        // we serialize track by track in order to save memory.
        file.print("{\"tracks\":[");
        for (int t = 0; t < NUMBER_OF_INSTRUMENTTRACKS; t++){
            StaticJsonDocument<40000> trackDoc;
            JsonObject track = trackDoc.to<JsonObject>();
            JsonArray patterns = track.createNestedArray("patterns");
            for (int p = 0; p < NUMBER_OF_PATTERNS; p++){
                JsonObject pattern = patterns.createNestedObject();
                SequencerPattern & sequencerPattern = sequencer->tracks[t].patterns[p];
                pattern["triggerState"] = sequencerPattern.triggerState;
                pattern["pLockArmState"] = sequencerPattern.pLockArmState;
                pattern["offset"] = sequencerPattern.offset;
                pattern["trackLength"] = sequencerPattern.trackLength;
                JsonArray steps = pattern.createNestedArray("steps");
                for (int s = 0; s < NUMBER_OF_STEPS_PER_PATTERN; s++){
                    JsonObject step = steps.createNestedObject();
                    JsonArray stepParams = step.createNestedArray("params");
                    SequencerStep & sequencerStep = sequencerPattern.steps[s];
                    stepParams.add(sequencerStep.parameter1);
                    stepParams.add(sequencerStep.parameter2);
                    stepParams.add(sequencerStep.parameter3);
                    stepParams.add(sequencerStep.parameter4);
                    stepParams.add(sequencerStep.parameter5);
                    stepParams.add(sequencerStep.parameter6);
                }
            }
            if (serializeJson(trackDoc, file) == 0) {
                Serial.println(F("Failed to write to file"));
            }
            if (t < NUMBER_OF_INSTRUMENTTRACKS - 1){
                file.print(",");
            }
        }
        file.print("]}");
        // Close the file
        file.close();
        Serial.println(F("Finished save"));
    };

void ProjectPersistence::load(const char *filename, Sequencer * sequencer){
        // Open file for writing
        File file = SD.open(filename, FILE_READ);
        if (!file) {
            Serial.println(F("Failed to read file"));
            return;
        }
        file.find("\"tracks\":[");
        int t = 0;
        int p = 0;
        int s = 0;
        do {
            StaticJsonDocument<40000> trackDoc;
            DeserializationError err = deserializeJson(trackDoc, file);
            // Parse succeeded?
            if (err) {
                Serial.print(F("deserializeJson() returned "));
                Serial.println(err.c_str());
                return;
            }
            JsonArray patterns = trackDoc["patterns"];
            //Serial.print("track");
            //Serial.println(t);
            p = 0;
            for (JsonObject pattern : patterns){
                //Serial.print("pattern");
                //Serial.println(p);
                SequencerPattern & sequencerPattern = sequencer->tracks[t].patterns[p];
                sequencerPattern.triggerState = pattern["triggerState"];
                sequencerPattern.pLockArmState = pattern["pLockArmState"];
                sequencerPattern.offset = pattern["offset"];
                sequencerPattern.trackLength = pattern["trackLength"];
                JsonArray steps = pattern["steps"];
                s = 0;
                for (JsonObject step : steps){
                    //Serial.print("step ");
                    //Serial.print(s);
                    //Serial.print(":");
                    JsonArray stepParams = step["params"];
                    SequencerStep & sequencerStep = sequencerPattern.steps[s];
                    sequencerStep.parameter1 = stepParams[0];
                    sequencerStep.parameter2 = stepParams[1];
                    sequencerStep.parameter3 = stepParams[2];
                    sequencerStep.parameter4 = stepParams[3];
                    sequencerStep.parameter5 = stepParams[4];
                    sequencerStep.parameter6 = stepParams[5];
                    s++;
                }
                p++;
            }
            t++;
        } while (file.findUntil(",","]"));

        // Close the file
        file.close();
        Serial.println(F("Finished load"));
    };
