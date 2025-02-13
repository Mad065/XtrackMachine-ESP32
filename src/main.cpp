#include <Arduino.h>

#include <WiFi.h>

const char* ssid = "Tu_SSID";
const char* password = "Tu_PASSWORD"; 

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
  return true;
}

bool reiniciar() {
  // Reiniciar el proceso
  return true;
}

bool iniciar() {
  // Iniciar el proceso
  return true;
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

        Serial.println("Mensaje recibido: " + message);

        if (message == "U") {
            char estado = obtenerEstado();
            client.println(estado);
            Serial.print("Estado enviado: ");
            Serial.println(estado);
        }

        if (message == "S") {
          if (detener())
          {
            client.println("T");
            Serial.println("T");
          }
        }

        if (message == "R") {
          if (reiniciar())
            {
              client.println("T");
              Serial.println("T");
            }

        }

        if (message == "I") {
          if (iniciar())
              {
                client.println("T");
                Serial.println("T");
              }
        }
        
        client.stop();
        Serial.println("Cliente desconectado");
    }
}
