//Librerias utilizadas
#include <ArduinoJson.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <WebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

//Variables para el WebServer
const char* ssid = "ParqueoMatic";
const char* pass = "12345678";
char dato;
uint8_t posicion = 0;
uint8_t posicion2 = 0;

bool upDisp = false;

const bool cero[] =   {0,1,1,1,1,1,1};
const bool uno[] =    {0,0,0,0,1,1,0};
const bool dos[] =    {1,0,1,1,0,1,1};
const bool tres[] =   {1,0,0,1,1,1,1};
const bool cuatro[] = {1,1,0,0,1,1,0};
const bool cinco[] =  {1,1,0,1,1,0,1};
const bool seis[] =   {1,1,1,1,1,0,1};
const bool siete[] =  {0,0,0,0,1,1,1};
const bool ocho[] =   {1,1,1,1,1,1,1};

const uint8_t pines[] = {15,2,0,4,5,18,19};

String recibido,dummy,recibido2;

int datos[4];

//variables para el timestamp
const char* ssid2 = "CBV383Z2-1616-G";
const char* password2 = "8da2b3806736a";

//cleinte NTP para obtener el tiempo
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

//para darle formato a las fechas
String formattedDate;
String dayStamp;
String timeStamp;

IPAddress ipLocal(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

WebServer server(80);

StaticJsonDocument<40000> datosJson; //crear un documento para guardar los valores

JsonArray fechas = datosJson.createNestedArray("fechas");
JsonArray parqueo = datosJson.createNestedArray("parqueo");

StaticJsonDocument<40000> datosJson2; //crear un documento para guardar los valores

JsonArray fechas2 = datosJson2.createNestedArray("fechas");
JsonArray parqueo2 = datosJson2.createNestedArray("parqueo");

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);

  for (int i = 0; i<7; i ++){
    pinMode(pines[i],OUTPUT);
    digitalWrite(pines[i],HIGH);
    }
  
  WiFi.mode(WIFI_MODE_APSTA);
  
  if(!SPIFFS.begin()){ //ver si se puede cargar las cosas con el SPIFF
      Serial.println("No se ha podido acceder a SPIFFS");
      return;
    }

  Serial.print("Configurando el access point ....."); //configuracion del accesspoint para enlazar la app
  WiFi.softAP(ssid,pass);
  WiFi.softAPConfig(ipLocal,gateway,subnet);
  delay(100);

  server.on("/", leerHTML);
  server.on("/rfsh", leerHTML);
  server.begin(); //iniciar el servidor
  Serial.println("se inicio el servidor");
  
  WiFi.begin(ssid2,password2);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  timeClient.begin();
  timeClient.setTimeOffset(-21600);
  
  delay(100);
  
}

void loop() {
  server.handleClient();

  if(upDisp){
    displayUpdate();
    upDisp = false;
    }
  
  if(Serial.available()){
    upDisp = true;
    upTime();
    recibido = Serial.readStringUntil('\n');
    Serial.println("1 " + recibido);
    
    if(fechas.size() > 5){
        fechas.remove(0);
        parqueo.remove(0);
        }
        
    fechas.add(dayStamp + "   " +timeStamp);
    parqueo.add(recibido);
        
    }
    
    if(Serial2.available()){
    upDisp = true;
    upTime();
    recibido2 = Serial2.readStringUntil('\n');
    Serial.println("2 " + recibido2);
    
    if(fechas2.size() > 5){
        fechas2.remove(0);
        parqueo2.remove(0);
        }
        
    fechas2.add(dayStamp + "   " +timeStamp);
    parqueo2.add(recibido2);
        
    }
    
}

void displayUpdate(void){
  String dummy2;
  uint8_t numDisp[4],numDisp2[4];
  
  for(int i=0; i<fechas.size();i++){
    const char* datoDisp = datosJson["parqueo"][i];
    dummy2 = datoDisp;

    for(int i=0; i<4;i++){
      int indexDisp = dummy2.indexOf('X'); //mira el index del dato
      numDisp[i]= dummy2.substring(0,indexDisp).toInt(); //lo separa en una substring hasta ese index
      dummy2 = dummy2.substring(indexDisp + 1); //crea la nueva string en base a la anterior quitando el dato
      }
    }

  for(int i=0; i<fechas2.size();i++){
    const char* datoDisp2 = datosJson2["parqueo"][i];
    dummy2 = datoDisp2;

    for(int i=0; i<4;i++){
      int indexDisp = dummy2.indexOf('X'); //mira el index del dato
      numDisp2[i]= dummy2.substring(0,indexDisp).toInt(); //lo separa en una substring hasta ese index
      dummy2 = dummy2.substring(indexDisp + 1); //crea la nueva string en base a la anterior quitando el dato
      }
    }
   uint8_t DatoFinal = 8 - numDisp[0] - numDisp[1] - numDisp[2] - numDisp[3] - numDisp2[0] - numDisp2[1] - numDisp2[2] - numDisp2[3];
   if(DatoFinal>8)DatoFinal = 0;
   if(DatoFinal<0)DatoFinal = 0;
   Serial.println(DatoFinal);

   switch(DatoFinal){
    case 0:
    for(int i=0;i<7;i++){
      digitalWrite(pines[i],cero[i]);
      }
    break;
    case 1:
    for(int i=0;i<7;i++){
      digitalWrite(pines[i],uno[i]);
      }
    break;
    case 2:
    for(int i=0;i<7;i++){
      digitalWrite(pines[i],dos[i]);
      }
    break;
    case 3:
    for(int i=0;i<7;i++){
      digitalWrite(pines[i],tres[i]);
      }
    break;
    case 4:
    for(int i=0;i<7;i++){
      digitalWrite(pines[i],cuatro[i]);
      }
    break;
    case 5:
    for(int i=0;i<7;i++){
      digitalWrite(pines[i],cinco[i]);
      }
    break;
    case 6:
    for(int i=0;i<7;i++){
      digitalWrite(pines[i],seis[i]);
      }
    break;
    case 7:
    for(int i=0;i<7;i++){
      digitalWrite(pines[i],siete[i]);
      }
    break;
    case 8:
    for(int i=0;i<7;i++){
      digitalWrite(pines[i],ocho[i]);
      }
    break;
    }
  }

void upTime(void){
  
  while(!timeClient.update()) {//Actualizar el tiempo actual
    timeClient.forceUpdate();
  }
  //obtener la fecha
  formattedDate = timeClient.getFormattedDate();

  // Separar en fecha y hora
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);

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
  HTMLdin.close();
  
  for(int i=0; i<fechas.size();i++){
    datosHTML += "<tr align =\"center\" >\n"; //inicio de la fila
    
    datosHTML += "<td>"; //celda de numero
    datosHTML += (i);
    datosHTML += "</td>\n";

    datosHTML += "<td>"; //celda de fecha
    const char* dummyfecha = datosJson["fechas"][i];
    dummy = dummyfecha;
    dummy = dummy.substring(0);
    datosHTML += dummy;
    datosHTML += "</td>\n";
    
    const char* dato10 = datosJson["parqueo"][i];
    dummy = dato10;
    
    for(int i=0; i<4;i++){
      int index = dummy.indexOf('X'); //mira el index del dato
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
  
  File HTMLdin2 = SPIFFS.open("/index2.html","r");
  datosHTML += HTMLdin2.readString();
  HTMLdin2.close();

  for(int i=0; i<fechas2.size();i++){
    datosHTML += "<tr align =\"center\" >\n"; //inicio de la fila
    
    datosHTML += "<td>"; //celda de numero
    datosHTML += (i);
    datosHTML += "</td>\n";

    datosHTML += "<td>"; //celda de fecha
    const char* dummyfecha = datosJson2["fechas"][i];
    dummy = dummyfecha;
    dummy = dummy.substring(0);
    datosHTML += dummy;
    datosHTML += "</td>\n";
    
    const char* dato10 = datosJson2["parqueo"][i];
    dummy = dato10;
    
    for(int i=0; i<4;i++){
      int index = dummy.indexOf('X'); //mira el index del dato
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
  server.send(200,"text/html",datosHTML);
  datosHTML = " ";
  
  }
