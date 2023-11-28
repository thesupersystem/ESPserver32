
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WakeOnLan.h>
#include <HTTPClient.h>

WiFiUDP UDP;
WakeOnLan WOL(UDP);


// Replace with your network credentials
const char* ssid = "TheB3st$$1";
const char* password = "l0ck0N3$$1";

const char* MACAddress = "B4:2E:99:F9:58:06";
// Set web server port number to 80
WiFiServer server(80);

//Watchout Server

const char * woIP = "192.168.1.7";
const int woPort = 3039;


 char shutdownWo[] = "authenticate 1  \npowerDown";
 char timeline1[] = "authenticate 1  \nreset \ndelay \"10\"  \nrun \"A\"";
 char timeline2[] = "authenticate 1  \nreset \ndelay \"10\"  \nrun \"B\"";
 char killall[] = "authenticate 1 \nreset";

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";

// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(output26, OUTPUT);

  // Set outputs to LOW
  digitalWrite(output26, LOW);


  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("GPIO 26 on");
              output26State = "on";
              wakeMyPC();
              digitalWrite(output26, HIGH);
            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("GPIO 26 off");
              output26State = "off";
              shutDownWO();
              digitalWrite(output26, LOW);
            } 


            else if(header.indexOf("GET /27/on") >= 0) {
              Serial.println("GPIO 27 on");
              output27State = "on";
              woTimeline1();
              digitalWrite(output27, HIGH);
            } else if (header.indexOf("GET /27/off") >= 0) {
              Serial.println("GPIO 27 off");
              output27State = "off";
              killAll();
              digitalWrite(output27, LOW);
            } 

 //////////////////////////////////////////////// HTML ////////////////////////////////////////////////           
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");

                        
            // Web Page Heading
            client.println("<body><h1>UOB Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>WO Media Server "  "</p>");
            
            // If the output26State is off, it displays the ON button       
            if (output26State=="off") {
              client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
              
            } else {
              client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 


             // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>Timeline 1 "  "</p>");
                       
               
            // If the output26State is off, it displays the ON button       
            if (output27State=="off") {
              client.println("<p><a href=\"/27/on\"><button class=\"button\">ON</button></a></p>");
              
            } else {
              client.println("<p><a href=\"/27/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 


            
            client.println("</body></html>");



            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
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

/////////////////////////////////// functions to control things in network ////////////////////////////////////


void wakeMyPC() {
    

      WOL.setRepeat(3, 100);
      WOL.calculateBroadcastAddress(WiFi.localIP(), WiFi.subnetMask());

  
    WOL.sendMagicPacket(MACAddress); // Send Wake On Lan packet with the above MAC address. Default to port 9.
    // WOL.sendMagicPacket(MACAddress, 7); // Change the port number
}


void sleepMyPC(){
      
      //Your Domain name with URL path or IP address with path
      String serverName = "http://192.168.1.25:3000/shutsys";
  
      String serverPath = serverName;
      
      HTTPClient http;
      // Your Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());
      
    // Send HTTP GET request
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();


}

void shutDownWO(){

       UDP.beginPacket(woIP,woPort);
      int i = 0;
    while (shutdownWo[i] != 0) 
    UDP.write((uint8_t)shutdownWo[i++]);
     UDP.endPacket();


  
}


void woTimeline1(){



     UDP.beginPacket(woIP,woPort);
      int i = 0;
    while (timeline1[i] != 0) 
    UDP.write((uint8_t)timeline1[i++]);
     UDP.endPacket();
  
}


void woTimeline2(){



     UDP.beginPacket(woIP,woPort);
      int i = 0;
    while (timeline2[i] != 0) 
    UDP.write((uint8_t)timeline2[i++]);
     UDP.endPacket();
  
}


void killAll(){



     UDP.beginPacket(woIP,woPort);
      int i = 0;
    while (killall[i] != 0) 
    UDP.write((uint8_t)killall[i++]);
     UDP.endPacket();
  
}
