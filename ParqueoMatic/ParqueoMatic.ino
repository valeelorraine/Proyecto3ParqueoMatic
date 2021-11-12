
//Librerias utilizadas
#include <WiFi.h>
#include <SPIFFS.h>
#include <WebServer.h>

//Variables para el WebServer
const char* ssid = "ParqueoMatic";
const char* pass = "12345678";

IPAddress ipLocal(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

WebServer server(80);

void setup() {
  Serial.begin(115200);
  Serial.print("Configurando el access point .....");
  WiFi.softAP(ssid,pass);
  WiFi.softAPConfig(ipLocal,gateway,subnet);
  delay(100);

  server.on("/", paginaInicio); //pagina de inicio

  server.begin(); //iniciar el servidor
  Serial.println("se inicio el servidor");
  delay(100);
}

void loop() {
  server.handleClient();
}

void paginaInicio(void){
  server.send(200,"text/html", enviarHTMLUpdate());
  }

String enviarHTMLUpdate(void){
  String val = "<!doctype html>\n";
  val += "<html>\n";
  val += "<head>\n";
  val += "<title>Prueba </title>\n";
  val += "</head>\n";
  val += "<body>\n";
  val += "<h1> Primera pagina </h1>\n";
  val += "Primera pagina\n";
  val += "</body>\n";
  val += "</html>\n";
  return val;
  }
