#include <Arduino.h>
#include <EEPROM.h>


#include <WiFi.h>

String ssid = "";
String password = ""; 
String ip = "";

WiFiServer server(7777); 

// Estados posibles
enum Estado {
    SUCIO = 'D',
    LIMPIO = 'F',
    LIMPIANDO = 'C',
    EN_USO = 'U',
    ERROR = 'X'
};

Estado estadoActual = SUCIO; // Estado inicial

// Guardar datos de red en la memoria
void guardarCredenciales(String ssid, String password) {
  EEPROM.begin(512);
  EEPROM.put(0, ssid);
  EEPROM.put(64, password);
  EEPROM.commit();
  EEPROM.end();
}


void cargarCredenciales() {
  EEPROM.begin(512);
  EEPROM.get(0, ssid);
  EEPROM.get(64, password);
  EEPROM.end();
}

// Función para cambiar el estado (ejemplo, se puede modificar según lógica)
void cambiarEstado(Estado nuevoEstado) {
    estadoActual = nuevoEstado;
    Serial.print("Estado cambiado a: ");
    Serial.println((char)estadoActual);
}

// Función que devuelve el estado actual como un carácter
char obtenerEstado() {
    return (char)estadoActual;
}

void obtenerEstadoActual() {
  // Comprobar los estados posibles con sensores etc
}

bool detener() {
  // Detener el proceso de limpieza

  //Prueba
  Serial.println("Aspiradora detenida");
  return true;
}

bool reiniciar() {
  // Reiniciar el proceso

  //Prueba
  Serial.println("Aspiradora reiniciada");
  return true;
}

bool iniciar() {
  // Iniciar el proceso

  //Prueba
  Serial.println("Aspiradora iniciada");
  return true;
}

void actualizarWiFi(String nuevoSSID, String nuevaPassword) {
  ssid = nuevoSSID;
  password = nuevaPassword;

  // Desconectar y reconectar con las nuevas credenciales
  WiFi.disconnect();
  WiFi.begin(ssid.c_str(), password.c_str());

  Serial.println("Reconectando a la nueva red Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Conectando a Wi-Fi...");
  }

  Serial.println("Conectado a la nueva red Wi-Fi");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    Serial.println("Conectando a la red Wi-Fi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando a Wi-Fi...");
    }

    Serial.println("Conectado a la red Wi-Fi");
    Serial.print("Dirección IP: ");
    ip = WiFi.localIP().toString();
    Serial.println(ip);
    server.begin();
    Serial.println("Esperando conexión...");
}

void loop() {
    obtenerEstadoActual();

    WiFiClient client = server.available();
    if (client) {
        Serial.println("Cliente conectado");
        String message = "";

        while (client.available()) {
            char c = client.read();
            message += c;
        }
        message.trim();
        message.toUpperCase();

        Serial.println("Mensaje recibido: " + message);

        // Comando para actualizar estado de la aspiradora
        if (message == "U") {
            char estado = obtenerEstado();
            client.println(estado);
            Serial.print("Estado enviado: ");
            Serial.println(estado);
        }

        // Comando para detener la aspiradora
        if (message == "S") {
          if (detener())
          {
            client.println("T");
            Serial.println("T");
          }
        }

        // Comando para reiniciar la aspiradora
        if (message == "R") {
          if (reiniciar())
            {
              client.println("T");
              Serial.println("T");
            }

        }

        // Comando para iniciar la aspiradora
        if (message == "I") {
          if (iniciar())
              {
                client.println("T");
                Serial.println("T");
              }
        }

        // Comando para obtener ip
        if (message == "G") {
          client.println(ip);
          Serial.println(ip);
        }
        

        // Comando para actualizar el SSID y la contraseña (formato: "W:SSID:CONTRASEÑA")
        if (message.startsWith("W:")) {
          int separatorIndex1 = message.indexOf(':', 2); // Buscar el primer separador
          int separatorIndex2 = message.indexOf(':', separatorIndex1 + 1); // Buscar el segundo separador

          if (separatorIndex1 != -1 && separatorIndex2 != -1) {
              String nuevoSSID = message.substring(2, separatorIndex1);
              String nuevaPassword = message.substring(separatorIndex1 + 1);

              actualizarWiFi(nuevoSSID, nuevaPassword);
              client.println("T"); // Confirmar que se actualizó
              Serial.println("SSID y contraseña actualizados");
          } else {
              client.println("F"); // Formato incorrecto
              Serial.println("Formato incorrecto para actualizar WiFi");
          }
      }
        
        client.stop();
        Serial.println("Cliente desconectado");
    }
}
