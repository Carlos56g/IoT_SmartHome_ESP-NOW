#include <WiFi.h>
#include <esp_now.h>
#include "FunctionsEspNow.h" //Archivo de cabecera que contiene la funciones personalizadas que se usaran con ESPNOW

// Credenciales de la Red
const char* ssid = "Xiaomi_F85B";
const char* password = "88889999";

WiFiServer Server(80);  // Numero de Puerto a Usar
String header;          // Variable para Almacenar el HTTP Request

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA); //Modo Access Point y Cliente WiFi 
  // Conexion a la red de Internet
  Serial.printf("\n\nConectando a la RED: %s\n",ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  //Imprime la Direccion IP del Server para acceder a el
  Serial.println("\n\nWiFi Conectado Corecctamente!\nDireccion IP: ");
  Serial.print(WiFi.localIP());
  Serial.printf("\nCanal Wifi: %d\n\n",WiFi.channel());
  Server.begin();
  InitEspNow();
}

void loop(){
  WiFiClient client = Server.available();   // Se encarga de Escuchar las peticiones en el Server

  if (client) {                             // Si se conecta un Cliente,
    Serial.println("\nNuevo Cliente Conectado!\n");          // Imprime un Mensaje Informativo
    String currentLine = "";                // String para almacenar los datos provinientes del cliente
    while (client.connected()) {  // Mientras el cliente esté conectado
      if (client.available()) {             // Regresa un entero con la cantidad de Bytes a leer del cliente
        char c = client.read();             // Lee un caracter a 
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // Si el Byte es de tipo \n
          if (currentLine=="") { //Si CurrentLine está vacia, entonces hemos llegado al final de la HTTP Request (ya que hemos recibido \n\n)
            //Empezamos a construir la Respuesta
            // La cabecera de una HTTP Response siempre debe de incluir un codigo de respuesta (e.g. HTTP/1.1 200 OK)
            // y el tipo de contenido asi el cliente sabe que contenido está recibiendo, por ultimo un salto de linea:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            //Envia un mensaje al ESP-NOW Seleccionado dependiendo de la clase de request
            if (header.indexOf("GET /Rst/Host") >= 0) {
              ESP.restart();
            }
            if (header.indexOf("GET /Send/Temp") >= 0) {
              SendMessage(1);
            }
            if (header.indexOf("GET /Send/Lght") >= 0) {
              SendMessage(2);
            }
            if (header.indexOf("GET /Send/Accs") >= 0) {
              SendMessage(3);
            }
            
            // Contenido html
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">"); //Evita que se carguen iconos
            
            
            // Styles
            client.println("<style>");
            client.println("html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".buttonStyle1 { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;}");
            client.println("</style>");

            client.println("</head>");

            // Body de la Pagina Web
            client.println("<body>");
            client.println("<h1>ESP32 Web Server ESP-NOW</h1>");
            
            // Boton del Host
            client.println("<p>" + (String)"Reiniciar Host" + "</p>");
            client.println("<p><a href=\"/Rst/Host\"><button class=\"buttonStyle1\">Reiniciar</button></a></p>");
            // Boton de Temperatura
            client.println("<p>" + (String)"Mensaje Temp" + "</p>");
            client.println("<p><a href=\"/Send/Temp\"><button class=\"buttonStyle1\">Encender Led</button></a></p>");
            //Boton de Luz
            client.println("<p>" + (String)"Mensaje Luz" + "</p>");
            client.println("<p><a href=\"/Send/Lght\"><button class=\"buttonStyle1\">Encender Led</button></a></p>");
            //Boton de Acceso
            client.println("<p>" + (String)"Mensaje Accs" + "</p>");
            client.println("<p><a href=\"/Send/Accs\"><button class=\"buttonStyle1\">Encender Led</button></a></p>");

            client.println("</body>");

            client.println("</html>");
            
            client.println();   // La respuesta HTTP termina con un Salto de linea
            
            break;              //Salimos del Loop
          } else {              // Si se recibe un \n, al finalizar, limpiamos CurrentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // Si se tiene cualquier cosa diferente de \r lo agrego a la línea actual. ya que los finales de linea suelen venir como los finales de línea suelen venir como \r\n.
          currentLine += c;      // El byte leido se agrega al final de la Linea Actual
        }
      }
    }
    // Reasignamos la variable header
    header = "";
    //Cierra la Conexion
    client.stop();
    Serial.println("\nCliente Desconectado!\n");
  }
}