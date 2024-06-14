# Sistema de Control de Puertas Inteligente con IoT
## 1. Descripción del Proyecto
Este proyecto tiene como objetivo desarrollar un sistema de control de puertas inteligente utilizando tecnología IoT. El sistema permitirá a los usuarios controlar el acceso a las habitaciones mediante un dispositivo ESP32, empleando el protocolo MQTT y la plataforma ThingSpeak. Los usuarios podrán abrir o cerrar las puertas utilizando una aplicación móvil, ya sea presionando un botón en la pantalla del celular o ingresando un PIN. Este proyecto está diseñado para ser implementado por un solo desarrollador, por lo que se ha considerado un balance adecuado entre complejidad y funcionalidad.

## 2. Planteamiento del Problema
En muchas instalaciones, como oficinas, hogares o edificios de apartamentos, la gestión del acceso a las habitaciones puede ser un desafío. Los métodos tradicionales de llaves físicas presentan problemas como la pérdida, la duplicación no autorizada y la dificultad de gestión a distancia. Se requiere un sistema más eficiente, seguro y fácil de administrar que permita el control remoto y la gestión del acceso.

## 3. Objetivos
### Objetivo General
Desarrollar un sistema de control de puertas inteligente que permita la apertura y cierre de puertas mediante una aplicación móvil, utilizando tecnología IoT, ESP32, MQTT y ThingSpeak.

### Objetivos Específicos
Implementar un dispositivo basado en ESP32 para controlar la cerradura de la puerta.
Configurar el protocolo MQTT para la comunicación entre el dispositivo y la aplicación móvil.
Utilizar la plataforma ThingSpeak para el almacenamiento y monitoreo de datos.
Garantizar la seguridad del sistema mediante la implementación de mecanismos de cifrado y autenticación.
## 4. Sensores 
#### Sensores
Sensor de estado de puerta (reed switch o sensor magnético): Para detectar si la puerta está abierta o cerrada.

## 5. IoT y Comunicación
### Plataforma IoT:
ThingSpeak: Para el almacenamiento y visualización de datos en la nube.
### Protocolo de Comunicación:

MQTT: Un protocolo ligero de mensajería que permite la comunicación entre dispositivos IoT y la aplicación móvil.
## 6. Base de Datos
La base de datos de ThingSpeak se utilizará para almacenar y visualizar datos relacionados con el estado de la puerta y los registros de acceso. 

## 7. Componentes del Sistema
### Hardware:

##### ESP32: El microcontrolador principal del sistema.
##### Relé de 5V.
##### Sensor de estado de puerta: Para detectar el estado de la puerta.
##### Cables y conectores: Para realizar las conexiones necesarias.
##### Batería: Para alimentar el sistema.
### Software:
##### Firmware ESP32: Para controlar los sensores, actuadores y gestionar la comunicación MQTT.
##### Broker MQTT: Para manejar los mensajes entre la aplicación móvil y el ESP32.
##### ThingSpeak: Para el almacenamiento y visualización de datos
