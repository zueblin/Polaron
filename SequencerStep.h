#ifndef SequencerStep_h
#define SequencerStep_h

#include <inttypes.h>

class SequencerStep
{
public:
    SequencerStep();
    
    void toggleTriggerState();
    bool isTriggerOn();
    
    
    void toggleParameterLockRecord();
    void setParameterLockRecordOn();
    void setParameterLockRecordOff();
    bool isParameterLockOn();
    void copyValuesFrom(SequencerStep sourceStep);
    
    
    uint8_t state;
    
    uint16_t parameter1;
    
    uint16_t parameter2;
    
    uint8_t triggerPattern;
    
};


#endif
