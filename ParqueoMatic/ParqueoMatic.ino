//Librerias utilizadas
#include <ArduinoJson.h>
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

uint8_t carros1 = 0;
uint8_t carros2 = 0;

void setup() {
  Serial.begin(115200);

  if(!SPIFFS.begin()){ //ver si se puede cargar las cosas con el SPIFF
      Serial.println("No se ha podido acceder a SPIFFS");
      return;
    }
    
  File archivoJSON = SPIFFS.open("/datos.json","r");
  if(!archivoJSON){
    Serial.println("error leyendo el JSON");
    return;
    }  

  size_t datos = archivoJSON.size();
  if(datos > 200){ //abrirlo y ver que no tenga un tamaño mayor al requerido
    Serial.println("El archivo es demasiado grande");
    return;
    }

  //creando un buffer para leer el archivo
  std::unique_ptr<char[]>buf(new char[datos]);
  archivoJSON.readBytes(buf.get(), datos);
  Serial.println(buf.get()); //ver si se cargo de forma correcta el archivo

  StaticJsonDocument<200> datosJson; //crear un documento para guardar los valores
  deserializeJson(datosJson, buf.get());
  carros1 = datosJson["Parqueo1"]; //se crean las variables individuales que se almacenaran
  carros2 = datosJson["Parqueo2"];

  archivoJSON.close(); //se cierra luego de abrir el archivo y modificarlo
  
  Serial.print("Configurando el access point ....."); //configuracion del accesspoint para enlazar la app
  WiFi.softAP(ssid,pass);
  WiFi.softAPConfig(ipLocal,gateway,subnet);
  delay(100);

  server.on("/", paginaInicio2);
  server.begin(); //iniciar el servidor
  Serial.println("se inicio el servidor");
  delay(100);
  
}

void loop() {
  server.handleClient();
  
}

void guardarJson(){
  //Usado para modificar el valor del JSON en el SPIFF
  StaticJsonDocument<100> datosParqueo; //JSON local colocado en ESP32 para las variables que se guardaran
  datosParqueo["Parqueo1"] = carros1;
  datosParqueo["Parqueo2"] = carros2;

  File archivoGuardar = SPIFFS.open("/datos.json","w"); //se abre el documento
  serializeJson(datosParqueo,archivoGuardar); //se serializa el JSON del ESP32 al archivo abierto y se modifica este JSON
  archivoGuardar.close(); //se cierra el archivo
  
  }

void paginaInicio(void){
  server.send(200,"text/html", enviarHTMLUpdate()); //para enviar la pagina de inicio
  }

void paginaInicio2(void){ //enviar una pagina de inicio utilizando el SPIFF
  File archivo = SPIFFS.open("/index.html","r");
  size_t enviar = server.streamFile(archivo,"text/html");
  archivo.close();
  }

String enviarHTMLUpdate(void){ //HTML variable que se envia a el cliente
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
