#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>
#include <WiFiClientSecure.h>

// Certificados necesarios para AWS IoT
const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

const char AWS_CERT_CRT[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUPeeZHR9O3ECpH/8chLUCEbEDTHgwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI0MTEwNzAxMDgw
OFoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMobr9bozJF4KnsgoVfB
e16uYmevmoSLVhBAEpDDJkJqHzRmyt+Tmqw3IBPCnEy/0Ih5tJXAzK9cFFhTjhVy
77Bh+QLkJzrQ0nkeEqBC0Z84BDlxdFewG3AKAQUxy0dcIiz+ATc8gT6AH+7UzV/B
/EBQq3iQUOmq+idc1hXOVYirRIBsInY5KELomwVEXD0HFG9lewBn3Baxe9bDi8is
YeKqGj4UZnqk1YIgzBg7T6HoysA5BtXAfIz5GSTdzF/+AdGucL82ScduY1B/AM+D
4ZWvT3867Y51WCdB8z3j8+AqX4Cr2OvgI8vuwFrc/Q36+Jj0ZtPBeuMYpXuJ+86n
lAkCAwEAAaNgMF4wHwYDVR0jBBgwFoAUu8MADhIQeKjOR7OY10ZRD1dRwfQwHQYD
VR0OBBYEFJ4Ol1UhTFMKvp5Uax45aC2BBFkrMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQBGWA5EmVAYR8XgTwbkhTFpKDrj
tPKBEG5wDcWQd6J/gHpLYw35AjZPqk4ysOqq6xSLtYOGUmFP555uoJ9DumimuVHO
7lg4CT+4mTBk7VJ2NlSO1QmmE9ZJX/jmwhqkPiKreiZ0bvjf4/Zust6IQXp8kzvg
RW4ZlKf6WLwUn1b1RQ3WuST+vFlbnCWvhlOmlEWmR8dohQGHDFebZPLPYZYnICOt
9NONQZdxZRIOdZkvyxn1tHKducxTwN1XNqxCW0qT1DXvjUx38wpmSV9gFXdrMm0R
iDQjo5mMq0NFcD09QOLY/VY1FhFdQxOtivo05AVAlUgCrCpDfLFAuPLRrW0p
-----END CERTIFICATE-----
)EOF";

const char AWS_CERT_PRIVATE[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEAyhuv1ujMkXgqeyChV8F7Xq5iZ6+ahItWEEASkMMmQmofNGbK
35OarDcgE8KcTL/QiHm0lcDMr1wUWFOOFXLvsGH5AuQnOtDSeR4SoELRnzgEOXF0
V7AbcAoBBTHLR1wiLP4BNzyBPoAf7tTNX8H8QFCreJBQ6ar6J1zWFc5ViKtEgGwi
djkoQuibBURcPQcUb2V7AGfcFrF71sOLyKxh4qoaPhRmeqTVgiDMGDtPoejKwDkG
1cB8jPkZJN3MX/4B0a5wvzZJx25jUH8Az4Phla9PfzrtjnVYJ0HzPePz4CpfgKvY
6+Ajy+7AWtz9Dfr4mPRm08F64xile4n7zqeUCQIDAQABAoIBAH+wIFbZu6zIS6aA
xg6IaeY2rGsvkaLd2SDpKhGViY16OH7F1rSML7NQHI7/a/o9UYV3xvS521eLCDuJ
hoHcnLxGGamO9Y3+MotcyY3AyWkbXefg2w0vcRZG68AZZoGOHvVoAOKh8lHHZuVg
QuSVzUXg8YR5QPVwFR1bVNJwCrANy0VpaOuJwu8HuxWyMALoYMyAUgdOdd1a30A5
IETilrTIMFsIx8nrPMjxCKlKZ7nKYGYOsMbRRMReOz/kRrtthZ8iikXKzmluKuEb
zFTHIKeoCCxF7903qovXB7ciJkgc7svQt+taTAeT6a6u0Hv+Kk7Hf14phEoCShz1
Ru7VF0UCgYEA/jx4CDwmuAkQ/WCuR32JMjuOq/ASsoWf86B6hsS9B65RkWfOySe3
VO1rccBvsxBjBh7CdEmoBMvojwC9eEQnChrX7KBTG+yjS7VMcuKj/6scvsDr7kfv
HzJDbnYc9qM1wrJr988KGyR4854hBVOEk3RJI+vpjD2sI4Ujw6GZfscCgYEAy4Kj
Ew4aClP6qMtKAM7MSjRgUvO3K+epHnEbOGgvKWOFfsUdHjzeBCApyMD+6qZx0J9q
hezJ3jlzUZ+FGT5uWpeVL5GEPZN9R4cTCa1eLSs43Bw03iDeG06l2Oi7jGTIskrk
ZMgeHGDNm07bK+Ai+XnOx+G/eFv34VZXje/0xq8CgYAC/6mWx0bZotm9c1d65mMo
4YT0SDegPW0+jWUEk+t63LZ/lY7d1ocWDFrZE6D9prLUTqaSgo/lxpXuod6aPYuM
Vfa4BsgUKz1d1jyZmky+5kDHizT9MYIDhy8+bUw4KIKo0jvtx1ZiLZLzpKjVDEAj
ERsPvmeZzNxr6tUaKJzL3wKBgQDF0LLgt8laFq1EW24QgPMUgfrLRvjaZXD/FSRH
XQXSrlk5kzItjS6lRfo3mFlxDt780ahCi5rhNmjAg1Vl50hvKH6r4T9wdyfEWiiI
NIuhTi0TzxsYe0xJsNYGlJqQ6fonTCLbT6XtIa9ZcnoaGOEGLH9xzR5HSOYWk83B
OuhsIwKBgQDasShSKQe20Zn6q77dJYRipDx6TDRdOdY4ledAGKMt8k9g9XeC67Qw
vGVyVks3EAok++uVaVn0FZqOy3VbXtvRamL3Y8pjEy3tA+gGILHuiMYMyGZnkFgv
85SchkJ3YvXXH8ntt9Z8H2ifWoet6s4lrMxECBfLvj/UqQp686cntw==
-----END RSA PRIVATE KEY-----
)EOF";

// Clase para controlar el servo
class ServoMotor {
private:
    Servo servo;
    int servoPin;
    int currentAngle;
    int servoSpeed;
    bool increasing;
    unsigned long lastMove;
    bool isMoving;
    int lastServoState;

public:
    ServoMotor(int pin) : servoPin(pin), currentAngle(0), servoSpeed(50), increasing(true), lastMove(0), isMoving(false) {}

    void setup() {
        servo.attach(servoPin);
        servo.write(currentAngle);
    }

    void setSpeed(int speed) {
        if (speed == 0) {
            isMoving = false;
            Serial.println("Servo stopped");
        } else if (speed >= 1 && speed <= 100) {
            servoSpeed = speed;
            isMoving = true;
            Serial.print("New servo speed set to: ");
            Serial.println(servoSpeed);
        } else {
            Serial.println("Invalid speed value. Use 0 to stop, or 1-100 for speed.");
        }
    }

    void updateFromState(int servoState) {
        if (servoState != lastServoState) {
            lastServoState = servoState;
            switch (servoState) {
                case 0:
                    setSpeed(0);  // Detiene el servo
                    break;
                case 1:
                    setSpeed(20); // Velocidad baja
                    break;
                case 2:
                    setSpeed(60); // Velocidad media
                    break;
                case 3:
                    setSpeed(95); // Velocidad alta
                    break;
                default:
                    Serial.println("Unknown servoState");
                    break;
            }
        }
    }

    void move() {
        if (!isMoving) return;

        unsigned long currentTime = millis();
        if (currentTime - lastMove >= (101 - servoSpeed)) {
            lastMove = currentTime;

            if (increasing) {
                currentAngle++;
                if (currentAngle >= 180) increasing = false;
            } else {
                currentAngle--;
                if (currentAngle <= 0) increasing = true;
            }

            servo.write(currentAngle);
            Serial.print("Current angle: ");
            Serial.println(currentAngle);
        }
    }
};

// Clase para manejar la conexión a AWS IoT y los mensajes MQTT
class Communicator {
private:
    const char* ssid;
    const char* password;
    const char* mqtt_server;
    const char* mqtt_topic;
    WiFiClientSecure espClient;
    PubSubClient client;
    ServoMotor& servoMotor;

public:
    Communicator(const char* ssid, const char* password, const char* mqtt_server, const char* mqtt_topic, ServoMotor& servo)
        : ssid(ssid), password(password), mqtt_server(mqtt_server), mqtt_topic(mqtt_topic), client(espClient), servoMotor(servo) {}

    void setup() {
        setupWifi();
        setupMQTT();
    }

    void loop() {
        if (!client.connected()) {
            reconnect();
        }
        client.loop();
    }

private:
    void setupWifi() {
        delay(10);
        Serial.println();
        Serial.print("Connecting to ");
        Serial.println(ssid);

        WiFi.begin(ssid, password);

        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }

        Serial.println("\nWiFi connected");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    }

    void setupMQTT() {
        espClient.setCACert(AWS_CERT_CA);
        espClient.setCertificate(AWS_CERT_CRT);
        espClient.setPrivateKey(AWS_CERT_PRIVATE);
        client.setServer(mqtt_server, 8883);
        client.setCallback([this](char* topic, byte* payload, unsigned int length) { this->callback(topic, payload, length); });
    }

    void callback(char* topic, byte* payload, unsigned int length) {
        // Creamos un buffer temporal para asegurarnos que el mensaje termine en null
        char jsonBuffer[512];
        if (length >= sizeof(jsonBuffer)) {
            Serial.println("Payload too large");
            return;
        }
        memcpy(jsonBuffer, payload, length);
        jsonBuffer[length] = '\0';

        StaticJsonDocument<512> doc;
        DeserializationError error = deserializeJson(doc, jsonBuffer);

        if (error) {
            Serial.print("deserializeJson() failed: ");
            Serial.println(error.f_str());
            return;
        }

        // Verificamos específicamente la existencia del campo servoState
        JsonObject state = doc["state"];
        if (!state.isNull()) {
            JsonObject desired = state["desired"];
            if (!desired.isNull() && desired.containsKey("servoState")) {
                int servoState = desired["servoState"];
                Serial.print("Received servoState: ");
                Serial.println(servoState);
                servoMotor.updateFromState(servoState);
            }
        }
    }

    void reconnect() {
        while (!client.connected()) {
            Serial.print("Attempting MQTT connection...");
            String clientId = "ESP32Client-";
            clientId += String(random(0xffff), HEX);

            if (client.connect(clientId.c_str())) {
                Serial.println("connected");
                client.subscribe(mqtt_topic);
            } else {
                Serial.print("failed, rc=");
                Serial.print(client.state());
                Serial.println(" try again in 5 seconds");
                delay(5000);
            }
        }
    }
};

ServoMotor servo(16);
Communicator communicator("FERRUVEGA", "Ignacio73*-", "a1o8cg6i3hlsiy-ats.iot.us-east-2.amazonaws.com", "$aws/things/tempEsp32/shadow/update/accepted", servo);

void setup() {
    Serial.begin(115200);
    servo.setup();
    communicator.setup();
}

void loop() {
    communicator.loop();
    servo.move();
}
