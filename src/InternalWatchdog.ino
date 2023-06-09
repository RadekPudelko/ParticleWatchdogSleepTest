#include "Particle.h"

SYSTEM_MODE(AUTOMATIC); 
SYSTEM_THREAD(ENABLED);
SerialLogHandler logHandler;

uint64_t myTime;


SystemSleepConfiguration sleepConfig;
SystemSleepResult sleepResult;

// Watchdog is not stoppable or reconfigurabale once started on bsom
// aka stop() never works and init() can only be called once
void setup() {
    Serial.begin(115200);
    while(!Serial.isConnected())
        delay(100);
    Log.info("Start");
    
    bool rc = Watchdog.init(WatchdogConfiguration()
      .timeout(30s));
      // .capabilities(WatchdogCap::SLEEP_RUNNING)

    if(rc != SYSTEM_ERROR_NONE) {
        Log.info("Error initing watchdog");
    }

    if(Watchdog.start() != SYSTEM_ERROR_NONE) {
        Log.info("Error starting watchdog");
    }

    myTime = System.millis();

    sleepConfig.mode(SystemSleepMode::STOP)
               .duration(40s) // Needs to be lower than keep alive perioud (default 23 min)
               .network(NETWORK_INTERFACE_CELLULAR);
}

void loop() {
    uint64_t elapsed = System.millis() - myTime; 
    if(elapsed % 2000 == 0) {
        Log.info("Time %d", elapsed);
        delay(1);
    }

    Watchdog.refresh();
    // // Go to sleep when Particle cloud connected
    if(Particle.connected()) {
        delay(5000);
        Log.info("Going to sleep");
        // Watchdog should autostop when sleeping 
        uint64_t sleepTime = System.millis();
        sleepResult = System.sleep(sleepConfig);
        uint64_t wakeTime = System.millis();
        Log.info("Sleep Time %d", sleepTime);
        Log.info("Wake time %d", wakeTime);
        delay(2000);
    }
    Watchdog.refresh();
}

