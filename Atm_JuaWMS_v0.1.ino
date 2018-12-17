#include <Automaton.h>
#include <jled.h>
#include <FS.h>

class Atm_juaWMS : public Machine {
  public:
    Atm_juaWMS(void) : Machine() {};

    // short pin;
    SPIFFS.begin();
    FSInfo fs_info;
    atm_timer_millis sample_timer;
    JLed led;
    string data_collected;
    float fileUsedKB;
    float fileTotalKB;
    float memoryTreshold;
    // bool memory_full
    // bool memory_clean

    enum { IDLE, COLLECT_ON, SAVE_ON, CLEAN_ON}; // STATES
    enum { SAMPLE_TIMER, DATA_COLLECTED, MEMORY_FULL, MEMORY_CLEAN, EVT_ON, EVT_OFF, ELSE}; // EVENTS
    enum { COLLECT_DATA, SAVE_DATA, CLEAN_MEMORY}; // ACTIONS

    Atm_juaWMS & begin(){
      const static state_t state_table[] PROGMEM = {
     /*                ON_ENTER     ON_LOOP    ON_EXIT   SAMPLE_TIMER  DATA_COLLECTED   MEMORY_FULL   MEMORY_CLEAN    EVT_ON       EVT_OFF        ELSE */
     /* IDLE  */       COLLECT_DATA,        -1,       -1,           -1,            -1,           -1,             -1,  COLLECT_ON,           -1,      -1,
     /* COLLECT_ON */  COLLECT_DATA,        -1,       -1,           -1,       SAVE_ON,           -1,             -1,          -1,         IDLE,      -1,
     /* SAVE_ON */     SAVE_DATA   ,        -1,       -1,   COLLECT_ON,            -1,     CLEAN_ON,             -1,          -1,         IDLE,      -1,
     /* CLEAN_ON */    CLEAN_MEMORY,        -1,       -1,           -1,            -1,           -1,        SAVE_ON,          -1,         IDLE,      -1,
      };

      SPIFFS.info(fs_info);
      led = JLed(D6); // use contructor for get time PIN led = JLed(pin);
      sample_timer.set(5000);// use contructor for get time
      data_collected = "" ;
      fileTotalKB = (float)fs_info.totalBytes ;
      memoryTreshold = 0.8;// use contructor for get memoryTreshold
      return *this;
    }

    int event( int id ) {
      switch ( id ) {

        case SAMPLE_TIMER :
          return sample_timer.expired( this );

        case DATA_COLLECTED :
          return (data_collected != "")

        case MEMORY_FULL:
          SPIFFS.info(fs_info);
          fileUsedKB = (float)fs_info.usedBytes ;
          return (fileUsedKB => fileTotalKB * memoryTreshold)//check if memory is full

        case MEMORY_CLEAN:
          SPIFFS.info(fs_info);
          fileUsedKB = (float)fs_info.usedBytes ;
          return (fileUsedKB <= fileTotalKB * memoryTreshold)//check if memory not is full
      }
      return 0;
    }

    void action( int id ) {
      switch ( id ) {
        case COLLECT_DATA :
          // collect data her
          led.Blink(100, 100).Forever();
          led.Update();
          return;
        case SAVE_DATA :
          //if data saved data_saved == structure
          //if memory is full
          led.off();
          led.Update();
          return;
        case CLEAN_MEMORY :
          led.off();
          led.Update();
          return;
       }
     }

}

Atm_juaWMS juaWMS;

void setup() {
  Atm_juaWMS juaWMS;
  juaWMS.begin();  // Turn it on
}

void loop() {
  juaWMS.cycle();
}
