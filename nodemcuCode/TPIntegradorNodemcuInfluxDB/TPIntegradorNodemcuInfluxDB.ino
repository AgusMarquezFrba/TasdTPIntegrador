#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include <DHT.h> // libreria para el sensor de humedad y temperatura

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

#define DHTTYPE DHT22 // DHT 22 (tambien se utilizo el 11)
#define DHTPIN 5


// InfluxDB v2 Url del server, e.g. https://eu-central-1-1.aws.cloud2.influxdata.com (Use: InfluxDB UI -> Load Data -> Client Libraries)
#define INFLUXDB_URL "http://192.168.1.9:8086"
// InfluxDB v2 API token (Use: InfluxDB UI -> Data -> API Tokens -> <select token>)
#define INFLUXDB_TOKEN "lP3iaHfTo-gYWDCVtVY_AiByDgwV2L00OG2Luoxvqk4cOIAUUs1H2ax1fJZWq09D_dxPwSyOWIzZl_CHPW675w=="
// InfluxDB v2 Id del organization (Use: InfluxDB UI -> User -> About -> Common Ids )
#define INFLUXDB_ORG "TASD"
// InfluxDB v2 Nombre del bucket (Use: InfluxDB UI ->  Data -> Buckets)
#define INFLUXDB_BUCKET "DEMO_SENSORES"

const char* ssid = "AGUSCASA";
const char* password = "ardp3456.";

DHT dht(DHTPIN, DHTTYPE); 


// Cliente de influx
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN);
Point sensor("sensores_dht");

long contador = 0;
unsigned long lastTime = 0;


void setUpWifi() 
{
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void setup()
{
  dht.begin();
  Serial.begin(115200);
          while (!Serial) {
  ; // espera a que levante el puerto serial del node
  }
  setUpWifi();
  delay(5000);

  sensor.addTag("ubicacion", "CABA");
}

void loop()
{
      sensor.clearFields();
      Serial.print("Writing: ");
      Serial.println(sensor.toLineProtocol());
  
     // Se leen los valores de humedad y temperatura y se envian los datos a la api de influx

     float h = dht.readHumidity();
     float t = dht.readTemperature();
     Serial.print("Current humidity = ");
     Serial.print(h);
     Serial.print("% ");
     Serial.print("temperature = ");
     Serial.print(t);
     Serial.println("C "); 

    Serial.println(client.getServerUrl());

    sensor.addField("temperature",t);
    sensor.addField("humidity",h);
    
    //Envia la informacion a InfluxDB
    if(!client.writePoint(sensor))
    {
      Serial.print("Error al escribir en influxDB: ");
      Serial.println(client.getLastErrorMessage());
    }else{
            Serial.println("Envio exitoso");
    }

    
    delay(1500);

    /* Pendiente envio por post custom
    while(Serial.available()) {
  
      String a = Serial.readString();
      
      if(!(a.isEmpty()))
        sendPostRequests(a);
      else
        Serial.println("No se recibe informacion");
  
        a = "";
      }
  
      delay(700);
      */

     
}
