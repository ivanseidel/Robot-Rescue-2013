// System Includes
#include <inttypes.h>
#include <math.h>

// Custom Includes
#include <EEPROM.h>
#include <I2Cmaster.h>
#include <MLX90614.h>

// Custom board Definitions
const int LED = A1;

/*
    Data communication definitions
*/
#define USART_SPEED             9600

#define CMD_READ_ALL            'r'
#define CMD_SETUP_ALL_SENSORS   's'
#define CMD_LIST_CONNECTIONS    'l'
#define CMD_SET_DEFAULT_VALUES  'd'
#define CMD_QUIT                '0'
#define CMD_CHECK_ALIVE         'a'
#define CMD_PERFORM_READ        'p'

// Raw Commands
#define CMD_SET_MIN_DEVIATION   'm' // Should be followed by a single byte
                                    // containing the DEVIATION*50
                                    // unsigned uint8_t type

#define CMD_START_BYTE          0xFF
#define CMD_SIZE                14  // Size of packet sent from ATMEGA328

#define DEFAULT_MIN_DEVIATION   1.0
#define DEVIATION_PRESCALE      20
#define VECTOR_PRESCALE         2
#define VECTOR_ZERO             64
#define TEMPERATURE_PRESCALE    2


/*
   Useful functions, just for easy coding...
*/
void workStart(){
    digitalWrite(LED, HIGH);
}

void workDone(){
    digitalWrite(LED, LOW);
}

void clearArray(int* array, int size, int val = 0){
    for(int i = 0; i < size; i++)
        array[i] = val;
}

float standard_deviation(float data[], int n)
{
    float mean=0.0, sum_deviation=0.0;
    int i;
    for(i=0; i<n;++i)
    {
        mean+=data[i];
    }
    mean=mean/n;
    for(i=0; i<n;++i)
    sum_deviation+=(data[i]-mean)*(data[i]-mean);
    return sqrt(sum_deviation/n);           
}

// Configurations on EEPROM
#define MAX_SENSORS         10
#define ADR_SENSORS_START   0x32
#define ADR_MIN_DEVIATION   0x60

// Get all sensors written on EEPROM.
// Sets the addresses to the pointer array
void readSensors(int* addresses){
    for(int i = 0; i < MAX_SENSORS; i++){
        addresses[i] = EEPROM.read(ADR_SENSORS_START+i);
    }
}

// Saves a sensors.
// positon = -1: saves on the last positon possible. if full, override last one.
void setSensor(int address, int position = 0){
    position = (position < 0? 0: position);
    position = (position > MAX_SENSORS? MAX_SENSORS: position);

    EEPROM.write(ADR_SENSORS_START+position, address);
}

double readMinDeviation(){
    return EEPROM.read(ADR_MIN_DEVIATION)/(DEVIATION_PRESCALE*1.0);
}

void setMinDeviation(uint8_t val){
    EEPROM.write(ADR_MIN_DEVIATION, val);
}

void setMinDeviation(double val){
    setMinDeviation((uint8_t)(val*DEVIATION_PRESCALE));
}

void setDefaultValues(){
    // Set default values for Temperatures Addresses
    for(int i = 0; i < 10; i++)
        setSensor(i+1, i);

    // Set Default Deviation
    setMinDeviation(DEFAULT_MIN_DEVIATION);
}

/*
    Local data storage for sensors and configs
*/

// MLX90644 communication objects, used to read and check sensors
MLX90614 sensors[MAX_SENSORS];

// Buffer to store last sensors measurements (easy and fast access)
float _temp[MAX_SENSORS] = {NULL};

float _minDeviation = 0.0;

// Keeps the sensor angles for current configuration. Angles are in radians
float _tempAngles[MAX_SENSORS] = {
    0.0,                // 0
    PI/4.0,             // 45
    1.309,              // 90  // 45+30  = 75
    1.83259,            // 90  // 135-30 = 105
    PI - HALF_PI,       // 135
    PI,                 // 180
    PI + PI/4.0,        // 225
    4.4506,             // 270 // 225+30 = 255
    4.97418,            // 270 // 315-30 = 285
    TWO_PI - HALF_PI,   // 315
};

float _tempWeights[MAX_SENSORS] = {
    1,                  // 0
    1,                  // 45
    0.8,                // 90
    0.8,                // 90
    1,                  // 135
    1,                  // 180
    1,                  // 225
    0.8,                // 270
    0.8,                // 270
    1,                  // 315
};

void loadSensors(){
    int sensorsAdr[MAX_SENSORS] = {};
    readSensors(sensorsAdr);

    for(int i = 0; i < MAX_SENSORS; i++){
        sensors[i] = MLX90614(sensorsAdr[i]);
    }

    _minDeviation = readMinDeviation();
}

/*
    Command Functions
*/

void listAllSensors(){
    int found[10];
    int total = MLX90614::scanAll(1, 127, 10, found);

    if(total>0){
        Serial.print("\t");
        for(int i = 0; i < total; i++){
            Serial.print("[");
            Serial.print(found[i]);
            Serial.print("] ");
        }
        Serial.println("");
    }
}

void listConnectionStatus(){

    int sensorsAdr[MAX_SENSORS] = {};
    bool sensorsStatus[MAX_SENSORS] = {};
    readSensors(sensorsAdr);

    for(int i = 0; i < MAX_SENSORS; i++){
        sensorsStatus[i] = MLX90614::scan(sensorsAdr[i], sensorsAdr[i]) > 0;
    }

    Serial.print("\t");
    bool con;
    int adr;
    for(int i = 0; i < MAX_SENSORS; i++){
        adr = sensorsAdr[i];
        con = sensorsStatus[i];

        if(adr == 0x00 || adr == 0xFF){
            Serial.print(" [----]  ");
        }else{
            if(con)Serial.print("<[");
            else Serial.print(" [");

            if(adr < 16)Serial.print("0x0");
            else Serial.print("0x");

            Serial.print(adr, HEX);
            
            if(con)Serial.print("]> ");
            else Serial.print("]  ");
        }
    }
    Serial.println("");
}

void setupAllSensors(){
    Serial.println("\t#Please, to change one sensor address, leave ONLY him connected");

    int found[10] = {0,0,0,0,0,0,0,0,0,0};
    int total_found = 0, last_total = 0;
   
    int filter;
    last_total = 0;
    while(true){
        total_found = MLX90614::scanAll(1, 127, 10, found);

        if(Serial.available()){
            char c = Serial.read();
            int pos = c - '0'+1;
            if(pos > 10 || pos <= 0){
                Serial.println("\t#Setup Ended: Wrong input");
                return;
            }else{

                Serial.println("\t#Changing address...");
                MLX90614 _mlx = MLX90614(found[0]);
                if(_mlx.changeAddress(pos)){
                    setSensor(pos, pos-1);
                    Serial.println("\n#Change complete! Unplug and plug to have effect.");
                    listConnectionStatus();
                }else{
                    Serial.println("\n#ERROR! Could not change address.");
                }

                Serial.print("\t#New Address: ");
                Serial.println(pos, HEX);

                Serial.println("\t#Unplug and plug to have affect!");
            }
        }

        if(total_found != last_total){
            if(filter <= 1){
                filter ++;
            }
            Serial.print("\t#Devices connected: ");
            Serial.println(total_found);
            last_total = total_found;
            listAllSensors();
        }

        last_total = total_found;
    }
}

// Prints a bar graph
void printBarGraph(float min, float max, int ysize, float vals[], int n){
    /*for(int k = 0; k < 30; k++){
        Serial.print("\n");
    }*/
    // Serial.println("-----------------------------------------------");

    float rel = (max-min)/ysize;

    for(int i = 0; i < n; i++){
        for(int x = 0; x < ysize; x++){
            if((vals[i]-min) >= x*rel){
                Serial.print("=");
            }else{
                break;
            }
        }
        Serial.print("| ");
        Serial.print(vals[i]);
        Serial.print("\n");
    }
    // Serial.println("-----------------------------------------------");
}

/*
    MAIN CODE
*/

#define UPDATE_INTERVAL     50

char c;
bool _sendData = false;
long _err = 0;
long lastRun = 0;

float deviation;
float vX, vY, angle;


void setup(){
    pinMode(LED, OUTPUT);

    Serial.begin(USART_SPEED);
    Serial.println("$Initializing...");
    
    MLX90614::init();
    loadSensors();

    Serial.println("$Ready!");
}

#define MSK         0b01111111
uint8_t buff[64];

void loop(){
    // Check for messasges on Serial, if some, enter terminal mode
    if(Serial.available()){
        workStart();
    
        int c = Serial.read();
        if(c == CMD_PERFORM_READ){
            _sendData = true;
        }if(c == CMD_READ_ALL){
            Serial.println("$ ListAll");
            listAllSensors();
        }else if(c == CMD_SETUP_ALL_SENSORS){
            Serial.println("$ Setup");
            setupAllSensors();
            loadSensors();
        }else if(c == CMD_LIST_CONNECTIONS){
            Serial.println("$ ActCon");
            listConnectionStatus();
        }else if(c == CMD_SET_DEFAULT_VALUES){
            Serial.println("$ ResetDef");
            setDefaultValues();
            loadSensors();
        }else if(c == CMD_QUIT){
        }else if(c == CMD_CHECK_ALIVE){
            Serial.write(0xFF);
        }else if(c == CMD_SET_MIN_DEVIATION){
            delay(1);
            if(Serial.available() > 0)
                setMinDeviation((uint8_t)Serial.read());
            Serial.write((uint8_t)(readMinDeviation()*DEVIATION_PRESCALE));
            loadSensors();
        }else{
            Serial.println("$ CMDNOTFOUND");
        }
        Serial.flush();
        workDone();
    }

    // Check if should execute sensor data processing
    workStart();
    
    // Start of real work
    // Reads all sensors
    for(int i = 0; i<MAX_SENSORS; i++){
        _temp[i] = sensors[i].read();

        if(_temp[i] < 0){ // Correct communication errors
            _err++;
            _temp[i] = 0;
        }
    }

    // Fix zeroes (mistakes on Communication)
    // TODO

    // Find average, minimum and maximum
    float min = 999, max = 0, avg;

    for(int i = 0; i<MAX_SENSORS; i++){
        min = (_temp[i] < min? _temp[i]: min);
        max = (_temp[i] > max? _temp[i]: max);
        avg += _temp[i];
    }
    avg = avg/MAX_SENSORS;

    // Calculate Mean and Standard Deviation
    deviation = standard_deviation(_temp, MAX_SENSORS);

    // Calculate vector angle for measurements
    vX = 0.0;
    vY = 0.0;

    for(int i = 0; i < MAX_SENSORS; i++){
        vX += (_temp[i]*_tempWeights[i]-min)*sin(_tempAngles[i]);
        vY += (_temp[i]*_tempWeights[i]-min)*cos(_tempAngles[i]);
    }

    angle = atan2(vX, vY)*180.0/PI;



    if(millis() - lastRun >= UPDATE_INTERVAL){
        lastRun = millis();

        /*
            Potocol definitions
            
            MSK = 0b01111111

            Byte 0: 0b1000000 // Start Byte
            Byte 1: Deviation | MSK
            Byte 2: VecX      | MSK
            Byte 3: VecY      | MSK

            Byte 4..13: SensorValX | MSK

            // Interruptions should be done as follows:

            Is Reset Flag enabled?
                Disable ALL outputs

            Is there victim?
                Set Direction flags (RIGHT, LEFT, FRONT, BACK)
                Output high on Interrupt pin


        */
        buff[0] = CMD_START_BYTE;
        buff[1] = (uint8_t)(deviation*DEVIATION_PRESCALE) & MSK;
        buff[2] = ((uint8_t)(vX*VECTOR_PRESCALE) + VECTOR_ZERO) & MSK;
        buff[3] = ((uint8_t)(vY*VECTOR_PRESCALE) + VECTOR_ZERO) & MSK;

        for(int i = 0; i < 10; i++)
            buff[i+4] = (uint8_t)(_temp[i]*TEMPERATURE_PRESCALE) & MSK;

        Serial.write(buff, 14);

        _sendData = false;
    }

    TWCR = 0;
    workDone();
    /*Serial.print("-------------\n");
    for(int i = 0; i < 14; i++){
        Serial.println(buff[i]);
    }*/
    /*Serial.print(deviation);
    Serial.print("\t");
    Serial.print(vX);
    Serial.print("\t");
    Serial.print(vY);
    Serial.print("\t");
    Serial.print(sqrt(vX*vX + vY*vY));
    Serial.print("\t");
    Serial.println(angle);*/

    // Output data for debbug
    /*printBarGraph(min, max, 50, _temp, MAX_SENSORS);
    Serial.print("DEVIATION: ");
    Serial.println(deviation);

    Serial.print("VECTOR:\tx=");
    Serial.print(vX);
    Serial.print("\ty=");
    Serial.println(vY);

    Serial.print("ANGLE: ");
    Serial.println(angle);*/

    /*Serial.print(angle);
    Serial.print("\t");
    Serial.println(deviation);*/

    // Reset I2C bus state
}
