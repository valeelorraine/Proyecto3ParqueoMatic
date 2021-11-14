//Librerias utilizadas
#include <ArduinoJson.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <WebServer.h>

//Variables para el WebServer
const char* ssid = "ParqueoMatic";
const char* pass = "12345678";
char dato;
uint8_t posicion = 0;

String recibido,dummy;
String dato1,dato2,dato3,dato4;

int datos[4];

IPAddress ipLocal(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

WebServer server(80);

StaticJsonDocument<1024> datosJson; //crear un documento para guardar los valores
JsonArray fechas = datosJson.createNestedArray("fechas");
JsonArray parqueo = datosJson.createNestedArray("parqueo");

void setup() {
  Serial.begin(115200);

  if(!SPIFFS.begin()){ //ver si se puede cargar las cosas con el SPIFF
      Serial.println("No se ha podido acceder a SPIFFS");
      return;
    }
    
  /*
  File archivoJSON = SPIFFS.open("/datos.json","r");
  if(!archivoJSON){
    Serial.println("error leyendo el JSON");
    return;
    }  

  size_t datos = archivoJSON.size();
  if(datos > 1024){ //abrirlo y ver que no tenga un tamaño mayor al requerido
    Serial.println("El archivo es demasiado grande");
    return;
    }

  //creando un buffer para leer el archivo
  std::unique_ptr<char[]>buf(new char[datos]);
  archivoJSON.readBytes(buf.get(), datos);
  Serial.println(buf.get()); //ver si se cargo de forma correcta el archivo
  
  StaticJsonDocument<1024> datosJson2;
  deserializeJson(datosJson2,buf.get());

  for(int i=0;i<5;i++){
    fechas.add(datosJson2["fechas"][i]);
    parqueo.add(datosJson2["parqueo"][i]);
    }
    
  serializeJsonPretty(datosJson, Serial);
  archivoJSON.close(); //se cierra luego de abrir el archivo y modificarlo
  */
  
  Serial.print("Configurando el access point ....."); //configuracion del accesspoint para enlazar la app
  WiFi.softAP(ssid,pass);
  WiFi.softAPConfig(ipLocal,gateway,subnet);
  delay(100);

  server.on("/", leerHTML);
  server.begin(); //iniciar el servidor
  Serial.println("se inicio el servidor");
  delay(100);
  
}

void loop() {
  server.handleClient();
  if(Serial.available()){
    posicion += 1;
    recibido = Serial.readStringUntil('\n');
    if(recibido != "a"){
    if(fechas.size() > 5){
        fechas.remove(0);
        parqueo.remove(0);
        }
    fechas.add(posicion);
    parqueo.add(recibido);
    leerHTML();
    /*
    serializeJsonPretty(datosJson, Serial);
    const char* dato10 = datosJson["parqueo"][0];
    dummy = dato10;
    
    for(int i=0; i<4;i++){
      int index = dummy.indexOf(','); //mira el index del dato
      datos[i]= dummy.substring(0,index).toInt(); //lo separa en una substring hasta ese index
      dummy = dummy.substring(index + 1); //crea la nueva string en base a la anterior quitando el dato
      Serial.println(datos[i]); //debugeo
      }
      */
    }
    else{
      guardarJson();
      }  
    }
    
}

void guardarJson(){
  //Usado para modificar el valor del JSON en el SPIFF
  File archivoGuardar = SPIFFS.open("/datos.json","w"); //se abre el documento
  serializeJson(datosJson,archivoGuardar); //se serializa el JSON del ESP32 al archivo abierto y se modifica este JSON
  archivoGuardar.close(); //se cierra el archivo
  Serial.println("Se guardo el JSON");
  }

void paginaInicio2(void){ //enviar una pagina de inicio utilizando el SPIFF
  File archivo = SPIFFS.open("/index.html","r");
  size_t enviar = server.streamFile(archivo,"text/html");
  archivo.close();
  }

void leerHTML(void){
  File HTMLdin = SPIFFS.open("/indexvariable.html","r");
  String datosHTML = HTMLdin.readString();
  /*datosHTML += "<tr>";
  datosHTML += "<td>2</td>";
  datosHTML += "<td>Abril 2020</td>";
  datosHTML += "<td>2</td>";
  datosHTML += "<td>2</td>";
  datosHTML += "<td>2</td>";
  datosHTML += "<td>2</td>";
  datosHTML += "</tr>";
  datosHTML += "</table>";
  datosHTML += "</html>";*/
  
  for(int i=0; i<fechas.size();i++){
    datosHTML += "<tr align =\"center\" >\n"; //inicio de la fila
    
    datosHTML += "<td>"; //celda de numero
    datosHTML += (i);
    datosHTML += "</td>\n";

    datosHTML += "<td>"; //celda de fecha
    const char* dummyfecha = datosJson["fechas"][i];
    dummy = dummyfecha;
    datosHTML += dummy;
    datosHTML += "</td>\n";
    
    const char* dato10 = datosJson["parqueo"][i];
    dummy = dato10;
    
    for(int i=0; i<4;i++){
      int index = dummy.indexOf(','); //mira el index del dato
      datos[i]= dummy.substring(0,index).toInt(); //lo separa en una substring hasta ese index
      dummy = dummy.substring(index + 1); //crea la nueva string en base a la anterior quitando el dato
      }
      
    for(int i=0; i<4;i++){
      datosHTML += "<td>"; //celda de numero
      datosHTML += datos[i];
      datosHTML += "</td>\n";
      }
    
    
    datosHTML += "</tr>\n"; //din de la fila para la siguiente
    }

  datosHTML += "</table>";
  datosHTML += "</html>";
  //Serial.println(datosHTML);
  server.send(200,"text/html",datosHTML);
  HTMLdin.close();
  }
