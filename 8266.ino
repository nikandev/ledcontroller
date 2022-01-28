#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

const char* ssid = "";
const char* password = "";

WiFiServer server(80);

int redValue = 0;
int greenValue = 0;
int blueValue = 0;

int pos1 = 0;
int pos2 = 0;
int pos3 = 0;
int pos4 = 0;

// Variable to store the HTTP request
String header;

const int bluePin = D8;
const int redPin = D7;
const int greenPin = D6;

// Setting PWM bit resolution
const int resolution = 256;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() 
{
    Serial.begin(115200);
    Serial.println("Booting...");
    
    // Connect to Wi-Fi network with SSID and password
    WiFi.softAPdisconnect(true);
    
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }
    
    // Start OTA server
    ArduinoOTA.onStart([]()
    {
        String type;
        
        if (ArduinoOTA.getCommand() == U_FLASH) 
        {
            type = "sketch";
        } 
        else 
        { // U_FS
            type = "filesystem";
        }
        
        // NOTE: if updating FS this would be the place to unmount FS using FS.end()
        Serial.println("Start updating " + type);
    });
    
    ArduinoOTA.onEnd([]() 
    {
        Serial.println("\nEnd");
    });
    
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) 
    {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    
    ArduinoOTA.onError([](ota_error_t error) 
    {
        Serial.printf("Error[%u]: ", error);
        
        if (error == OTA_AUTH_ERROR) 
        {
            Serial.println("Auth Failed");
        } 
        else if (error == OTA_BEGIN_ERROR) 
        {
            Serial.println("Begin Failed");
        } 
        else if (error == OTA_CONNECT_ERROR) 
        {
            Serial.println("Connect Failed");
        } 
        else if (error == OTA_RECEIVE_ERROR) 
        {
            Serial.println("Receive Failed");
        }
        else if (error == OTA_END_ERROR) 
        {
            Serial.println("End Failed");
        }
    });
    
    ArduinoOTA.begin();
    
    // Print local IP address and start web server
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    Serial.println("Starting server...");
    server.begin();

    analogWriteRange(resolution);
    analogWrite(redPin, LOW);
    analogWrite(greenPin, LOW);
    analogWrite(bluePin, LOW);
}

void loop()
{
    ArduinoOTA.handle(); // Handle OTA updates
    WiFiClient client = server.available();   // Listen for incoming clients
    
    if (client)
    {                             // If a new client connects,
        currentTime = millis();
        previousTime = currentTime;
        Serial.println("New Client.");          // print a message out in the serial port
        String currentLine = "";                // make a String to hold incoming data from the client
        
        while (client.connected() && currentTime - previousTime <= timeoutTime) 
        {            // loop while the client's connected
            currentTime = millis();
            
            if (client.available()) 
            {             // if there's bytes to read from the client,
                char c = client.read();             // read a byte, then
                //Serial.write(c);                    // print it out the serial monitor
                header += c;
                
                if (c == '\n') 
                {                    // if the byte is a newline character
                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    if (currentLine.length() == 0) 
                    {
                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's coming, then a blank line:
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println("Connection: close");
                        client.println();
                        
                        // Request sample: /?r201g32b255&
                        // Red = 201 | Green = 32 | Blue = 255
                        if(header.indexOf("GET /?r") >= 0) 
                        {
                            pos1 = header.indexOf('r');
                            pos2 = header.indexOf('g');
                            pos3 = header.indexOf('b');
                            pos4 = header.indexOf('&');

                            redValue = header.substring(pos1+1, pos2).toInt();
                            client.println(" redvalue ");
                            client.println(redValue);
                            redValue = constrain(redValue, 0, 256);
                            client.println(" redvalue constrained ");
                            client.println(redValue);
                            blueValue = header.substring(pos2+1, pos3).toInt();
                            client.println(" blueValue");
                            client.println(blueValue);
                            blueValue = constrain(blueValue, 0, 256);
                            client.println(" blueValue constrained ");
                            client.println(blueValue);
                            blueValue = map(blueValue, 0, 256, 0, 64);
                            client.println(" blueValue mapped ");
                            client.println(blueValue);
                            greenValue = header.substring(pos3+1, pos4).toInt();
                            client.println(" greenValue ");
                            client.println(greenValue);
                            greenValue = constrain(greenValue, 0, 256);
                            client.println(" greenValue constrained ");
                            client.println(greenValue);
                            greenValue = map(greenValue, 0, 256, 0, 64);
                            client.println(" greenValue mapped ");
                            client.println(greenValue);

                            analogWrite(redPin, redValue);
                            analogWrite(bluePin, blueValue);
                            analogWrite(greenPin, greenValue);
                        }
                        
                        break;
                    }
                    else 
                    { // if you got a newline, then clear currentLine
                        currentLine = "";
                    }
                    
                } 
                else if (c != '\r') 
                {  // if you got anything else but a carriage return character,
                    currentLine += c;      // add it to the end of the currentLine
                }
            }
        }
        // Clear the header variable
        header = "";
        // Close the connection
        client.stop();
        Serial.println("Client disconnected.");
        Serial.println("");
    }
}
