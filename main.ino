#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"         //Heart rate calculating algorithm, packaged with MAX3010x library
#include <SD.h>                // for SD card
#include <TMRpcm.h>            // Lib to play wav file
#include <Adafruit_MLX90614.h> // for infrared thermometer
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"

#define I2C_ADDRESS 0x3C // 0X3C+SA0 - 0x3C or 0x3D
#define RST_PIN -1       // Define proper RST_PIN if required.
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define SD_ChipSelectPin 10 // for SD card
//--------------------------------------- oled

MAX30105 particleSensor;
Adafruit_MLX90614 mlx = Adafruit_MLX90614(); // for infrared thermometer
SSD1306AsciiAvrI2c oled;                     // create an object for LCD
TMRpcm tmrpcm;                               // create an object for music player

const int RATE_SIZE = 4;
const int echoPin = 16; // ultrasonic
const int trigPin = 17; // ultrasonic
const int RELAY_PIN = 3;
const int ECHO_PIN = 4;
const int TRIG_PIN = 5;

byte rates[RATE_SIZE];
byte rateSpot = 0;

float beatsPerMinute;

long lastBeat = 0;
long avgSpO2 = 0;
long count = 0;
long duration;
long duration1;

int beatAvg;
int distance;
int step1_judge = 0;
int distance1;
int count = 0;

#if defined(_AVR_ATmega328P) || defined(AVR_ATmega168_)

uint16_t irBuffer[100];  // infrared LED sensor data
uint16_t redBuffer[100]; // red LED sensor data
#else
uint32_t irBuffer[100];  // infrared LED sensor data
uint32_t redBuffer[100]; // red LED sensor data
#endif

int32_t bufferLength;  // data length
int32_t spo2;          // SPO2 value
int8_t validSPO2;      // indicator to show if the SPO2 calculation is valid
int32_t heartRate;     // heart rate value
int8_t validHeartRate; // indicator to show if the heart rate calculation is valid

byte pulseLED = 11; // Must be on PWM pin
byte readLED = 13;  // Blinks with each data read

double temp; // to save temperature value

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // Declaring the display name (display)

// Logo2 and Logo3 are two bmp pictures that display on the OLED if called
static const unsigned char PROGMEM logo_bmp[] = {
    0x03,
    0xC0,
    0xF0,
    0x06,
    0x71,
    0x8C,
    0x0C,
    0x1B,
    0x06,
    0x18,
    0x0E,
    0x02,
    0x10,
    0x0C,
    0x03,
    0x10,
    0x04,
    0x01,
    0x10,
    0x04,
    0x01,
    0x10,
    0x40,
    0x01,
    0x10,
    0x40,
    0x01,
    0x10,
    0xC0,
    0x03,
    0x08,
    0x88,
    0x02,
    0x08,
    0xB8,
    0x04,
    0xFF,
    0x37,
    0x08,
    0x01,
    0x30,
    0x18,
    0x01,
    0x90,
    0x30,
    0x00,
    0xC0,
    0x60,
    0x00,
    0x60,
    0xC0,
    0x00,
    0x31,
    0x80,
    0x00,
    0x1B,
    0x00,
    0x00,
    0x0E,
    0x00,
    0x00,
    0x04,
    0x00,
};

static const unsigned char PROGMEM logo1_bmp[] = {0x01, 0xF0, 0x0F, 0x80, 0x06, 0x1C, 0x38, 0x60, 0x18, 0x06, 0x60, 0x18, 0x10, 0x01, 0x80, 0x08,
                                                  0x20, 0x01, 0x80, 0x04, 0x40, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x02, 0xC0, 0x00, 0x08, 0x03,
                                                  0x80, 0x00, 0x08, 0x01, 0x80, 0x00, 0x18, 0x01, 0x80, 0x00, 0x1C, 0x01, 0x80, 0x00, 0x14, 0x00,
                                                  0x80, 0x00, 0x14, 0x00, 0x80, 0x00, 0x14, 0x00, 0x40, 0x10, 0x12, 0x00, 0x40, 0x10, 0x12, 0x00,
                                                  0x7E, 0x1F, 0x23, 0xFE, 0x03, 0x31, 0xA0, 0x04, 0x01, 0xA0, 0xA0, 0x0C, 0x00, 0xA0, 0xA0, 0x08,
                                                  0x00, 0x60, 0xE0, 0x10, 0x00, 0x20, 0x60, 0x20, 0x06, 0x00, 0x40, 0x60, 0x03, 0x00, 0x40, 0xC0,
                                                  0x01, 0x80, 0x01, 0x80, 0x00, 0xC0, 0x03, 0x00, 0x00, 0x60, 0x06, 0x00, 0x00, 0x30, 0x0C, 0x00,
                                                  0x00, 0x08, 0x10, 0x00, 0x00, 0x06, 0x60, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x01, 0x80, 0x00};

/**
 * @brief Set the up sd card object
 *
 */
void setup_sd_card()
{
    if (!SD.begin(SD_ChipSelectPin))
    { // see if the card is present and can be initialized:
        Serial.println("SD fail");
        Serial.println("SD ok");
        oled.clear();
        oled.set1X();
        oled.println("SD card fail");
        return; // don't do anything more if not
    }
    else
    {
        Serial.println("SD ok");
        oled.clear();
        oled.println("SD card ok");
    }
}

/**
 * @brief Set the up MAX30105 sensor object
 *
 */
void setup_particle_sensor()
{
    // Initialize sensor
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) // Use default I2C port, 400kHz speed
    {
        Serial.println(F("MAX30105 was not found. Please check wiring/power."));
        while (1)
            ;
    }
}

/**
 * @brief initialize MAX30105 sensor
 *
 */
void init_particle_sensor()
{
    // Initialize sensor
    particleSensor.begin(Wire, I2C_SPEED_FAST); // Use default I2C port, 400kHz speed
    particleSensor.setup();                     // Configure sensor with default settings
    particleSensor.setPulseAmplitudeRed(0x0A);  // Turn Red LED to low to indicate sensor is running
}

/**
 * @brief Arduino default setup function
 *
 */
void setup()
{
    // dispensing ultrasonic setup
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(TRIG_PIN, OUTPUT);

#if RST_PIN >= 0
    oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else  // RST_PIN >= 0
    oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif // RST_PIN >= 0 \
       // Call oled.setI2cClock(frequency) to change from the default frequency.
    oled.setFont(Adafruit5x7);

    tmrpcm.speakerPin = 9; // pin 9 for output audio
    Serial.begin(9600);

    pinMode(pulseLED, OUTPUT);
    pinMode(readLED, OUTPUT);

    setup_particle_sensor();
    setup_sd_card();
    delay(1000);

    tmrpcm.play("m_wel.wav");
    tmrpcm.volume(1);

    oled.clear();
    oled.set2X();
    oled.println("");
    oled.println("  Welcome");

    display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Start the OLED display
    display.display();

    delay(3000);

    init_particle_sensor();

    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);

    mlx.begin(); // start infrared thermometer

    delay(10000); // wait for welcome audio
}

/**
 * @brief controls the sanitizer dispensing pump
 *
 */
void handSanitizerDispenser()
{
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration1 = pulseIn(ECHO_PIN, HIGH);
    // Calculating the distance
    distance1 =
        duration1 * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
    // Displays the distance

    if (distance1 < 5)
    {
        if (count == 0)
        {
            digitalWrite(RELAY_PIN, LOW);
            delay(200);
            digitalWrite(RELAY_PIN, HIGH);
            delay(200);
            count++;
        }
    }
    else
    {
        digitalWrite(RELAY_PIN, HIGH);
        delay(800);
        count = 0;
    }

    delay(50);
}

/**
 * @brief Get the Pulse And Sp O2 from MAX30105
 *
 */
void getPulseAndSpO2()
{
    long irValue = particleSensor.getIR(); // Reading the IR value it will permit us to know if there's a finger on the sensor or not
                                           // Also detecting a heartbeat
    if (irValue > 7000)
    {                                                      // If a finger is detected
        display.clearDisplay();                            // Clear the display
        display.drawBitmap(5, 5, logo_bmp, 24, 21, WHITE); // Draw the first bmp picture (little heart)
        display.setTextSize(2);                            // Near it display the average BPM you can display the BPM if you want
        display.setTextColor(WHITE);
        display.setCursor(50, 0);
        display.println("BPM");
        display.setCursor(50, 18);
        display.println(beatAvg);
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(90, 20);
        display.println("SpO2");
        display.setCursor(90, 10);
        display.print(avgSpO2);
        display.println("%");
        display.display();

        if (checkForBeat(irValue) == true) // If a heart beat is detected
        {

            maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &avgSpO2, &heartRate, &validHeartRate);

            display.clearDisplay();                             // Clear the display
            display.drawBitmap(0, 0, logo1_bmp, 32, 32, WHITE); // Draw the second picture (bigger heart)
            display.setTextSize(2);                             // And still displays the average BPM
            display.setTextColor(WHITE);
            display.setCursor(50, 0);
            display.println("BPM");
            display.setCursor(50, 18);
            display.println(beatAvg);
            display.setTextSize(1);
            display.setTextColor(WHITE);
            display.setCursor(90, 20);
            display.println("SpO2");
            display.setCursor(90, 10);
            display.print(avgSpO2);
            display.println("%");
            display.display(); // And tone the buzzer for a 100ms you can reduce it it will be better
            delay(100);        // Deactivate the buzzer to have the effect of a "bip"
            // We sensed a beat!
            long delta = millis() - lastBeat; // Measure duration between two beats
            lastBeat = millis();

            beatsPerMinute = 60 / (delta / 1000.0); // Calculating the BPM

            if (beatsPerMinute < 255 && beatsPerMinute > 20) // To calculate the average we strore some values (4) then do some math to calculate the average
            {
                rates[rateSpot++] = (byte)beatsPerMinute; // Store this reading in the array
                rateSpot %= RATE_SIZE;                    // Wrap variable

                // Take average of readings
                beatAvg = 0;
                for (byte x = 0; x < RATE_SIZE; x++)
                    beatAvg += rates[x];
                beatAvg /= RATE_SIZE;
            }
        }
    }
    if (irValue < 7000)
    { // If no finger is detected it inform the user and put the average BPM to 0 or it will be stored for the next measure
        beatAvg = 0;
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(30, 5);
        display.println("Please Place ");
        display.setCursor(30, 15);
        display.println("your finger ");
        display.display();
    }
}

/**
 * @brief default  Arduino loop function
 *
 */
void loop()
{
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH, 23529); // 23529us for timeout 4.0m max

    // Calculating the distance
    distance = duration * 0.034 / 2;

    Serial.print("distance is ");
    Serial.println(distance);

    if ((distance < 10) && (distance > 0))
        step1_judge++;
    else
        step1_judge = 0;

    if (step1_judge > 2)
    {
        step1_judge = 0;
        tmrpcm.play("m_wel.wav");
        delay(10000); // wait for welcome voice complete

        temp = mlx.readObjectTempC() + 1.2;

        oled.clear();
        oled.set2X();
        oled.println("");
        oled.print("  ");
        oled.print(temp, 1);
        oled.println("dgC");

        tmrpcm.play("m_now.wav");
        delay(1380);

        if (temp < 20)
        {
            tmrpcm.play("m_b20.wav");  // speak out below 20 dgC
            delay(1700);               // wait for audio finish
            tmrpcm.play("m_nman.wav"); // speak out "you're not human"
            delay(2270);               // wait for audio finish
        }
        else
        {
            if (temp > 50)
            {
                tmrpcm.play("m_over50.wav");
                delay(1740);
                tmrpcm.play("m_nman.wav");
                delay(2270);
            }
            else
            {
                speak_out(temp); // speak out temperature (if it is from 20 to 50dgC)
                delay(1500);
                if ((temp > 36) && (temp < 37))
                {
                    tmrpcm.play("m_normal.wav"); // speak out "normal temperature, keep healthy" if it is 36~37dgC
                    delay(3268);
                }
                if (temp > 37)
                {
                    tmrpcm.play("m_fever.wav"); // speak out "you got fever"
                    delay(2728);
                }
            }
        }
    }
    handSanitizerDispenser();
    getPulseAndSpO2();
    delay(300);
}

/**
 * @brief helper function to play audio for  auditory feedback
 *
 * @param temperature_result double
 */
void speak_out(double temperature_result)
{
    temperature_result = temperature_result * 10;
    temperature_result = round(temperature_result);
    int temp0 = temperature_result;
    int temp1 = temp0 / 10; // temperature value, xy digit (in xy.z dgC)
    int temp2 = temp1 % 10; // temperature value, y digit (in xy.z dgC)
    int temp3 = temp0 % 10; // temperature value, z digit (in xy.z dgC)

    if (temp1 < 20)
    {
        tmrpcm.play("m_below20.wav");
        delay(1631);
    }
    if (temp1 > 50)
    {
        tmrpcm.play("m_over50.wav");
        delay(1747);
    }
    if ((temp1 >= 20) && (temp1 <= 29))
    {
        tmrpcm.play("m_twenty.wav");
        delay(600);
    }
    if ((temp1 >= 30) && (temp1 <= 39))
    {
        tmrpcm.play("m_thirty.wav");
        delay(500);
    }
    if ((temp1 >= 40) && (temp1 <= 49))
    {
        tmrpcm.play("m_fourty.wav");
        delay(691);
    }
    if (temp2 != 0)
        speak_num(temp2); // temperature value, y digit (in xy.z dgC)
    if ((temp1 >= 20) && (temp1 <= 50))
    {
        tmrpcm.play("m_point.wav");
        delay(319);
        speak_num(temp3); // temperature value, z digit (in xy.z dgC)
    }
    tmrpcm.play("m_dgc.wav");
    delay(853);
    Serial.println(temp0);
    Serial.println(temp1);
    Serial.println(temp2);
    Serial.println(temp3);
}

/**
 * @brief helper fucntion to speak out each digit of a temperature reading
 *
 * @param number
 */
void speak_num(int number)
{
    if (number == 1)
    {
        tmrpcm.play("m_one.wav");
        delay(453);
    }
    if (number == 2)
    {
        tmrpcm.play("m_two.wav");
        delay(499);
    }
    if (number == 3)
    {
        tmrpcm.play("m_three.wav");
        delay(406);
    }
    if (number == 4)
    {
        tmrpcm.play("m_four.wav");
        delay(401);
    }
    if (number == 5)
    {
        tmrpcm.play("m_five.wav");
        delay(354);
    }
    if (number == 6)
    {
        tmrpcm.play("m_six.wav");
        delay(401);
    }
    if (number == 7)
    {
        tmrpcm.play("m_seven.wav");
        delay(633);
    }
    if (number == 8)
    {
        tmrpcm.play("m_eight.wav");
        delay(360);
    }
    if (number == 9)
    {
        tmrpcm.play("m_nine.wav");
        delay(580);
    }
    if (number == 0)
    {
        tmrpcm.play("m_zero.wav");
        delay(610);
    }
}