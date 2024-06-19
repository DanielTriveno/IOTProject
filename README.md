# Sistema de Control de Puertas Inteligente con IoT
## 1. Descripción del Proyecto

Este proyecto tiene como objetivo desarrollar un sistema de control de puertas inteligente utilizando tecnología IoT. El sistema permitirá a los usuarios controlar el acceso a las habitaciones mediante un dispositivo ESP32, empleando el protocolo MQTT y la plataforma ThingSpeak. Los usuarios podrán abrir o cerrar las puertas utilizando una aplicación móvil. También, se utiliza un sensor ultrasónico para detectar y confirmar la llegada la proximidad de personas a la puerta. 

## 2. Planteamiento del Problema
La gestión del acceso a las habitaciones en instalaciones como oficinas, hogares o edificios de apartamentos puede ser complicada con métodos tradicionales. Este sistema busca ofrecer una solución más eficiente y segura, que permita el control remoto y la gestión del acceso sin las complicaciones de las llaves físicas.

## 3. Objetivos
### Objetivo General
Desarrollar un sistema de control de puertas inteligente que permita detectar la proximidad de personas y controlar la apertura y cierre de puertas mediante una aplicación móvil, utilizando tecnología IoT.

### Objetivos Específicos
- Implementar un dispositivo basado en ESP32 para controlar la cerradura de la puerta mediante un motor servo.
- Utilizar un sensor ultrasónico para detectar la presencia de personas frente a la puerta.
- Emplear la plataforma ThingSpeak para el almacenamiento y monitoreo de datos.
- Asegurar la comunicación entre el dispositivo y la aplicación móvil mediante WiFi.

## 4. Sensores y Actuadores
### Sensores
- Sensor ultrasónico: Para detectar la presencia de invitados en la puerta.
### Actuadores
- Motor servo: Para controlar el estado de la cerradura de la puerta.
- Buzzer: Para emitir un sonido cuando se detecta un invitado.

## 5. IoT y Comunicación
### Plataforma IoT:
- ThingSpeak: Para el almacenamiento y visualización de datos en la nube.
### Protocolo de Comunicación:
- MQTT: Utilizado para la comunicación entre el dispositivo ESP32 y la aplicación móvil, proporcionando un método eficiente y ligero para el envío de mensajes.

## 6. Base de Datos
La base de datos de ThingSpeak se utilizará para almacenar y visualizar datos relacionados con el estado de la puerta y los registros de acceso.

## 7. Componentes del Sistema
### Hardware:
- ESP32: El microcontrolador principal del sistema.
- Motor servo: Para controlar la cerradura de la puerta.
- Sensor ultrasónico: Para detectar la presencia de invitados.
- Buzzer: Para notificar la detección de invitados.
- Pantalla OLED: Para mostrar información del sistema.
- Cables y conectores: Para realizar las conexiones necesarias.
- Batería: Para alimentar el sistema.
### Software:
- Firmware ESP32: Para controlar los sensores, actuadores y gestionar la comunicación WiFi.
- Broker MQTT: Para manejar los mensajes entre la aplicación móvil y el ESP32.
- ThingSpeak: Para el almacenamiento y visualización de datos.