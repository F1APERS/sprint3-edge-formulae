# Sprint 3 - Edge

## Descrição
Este é um projeto que permite ao usuário simular os batimentos cardíacos e temperatura corporal de um piloto da Fórmula E durante uma corrida. O projeto foi desenvolvido para a disciplina de Edge, do curso de Engenharia de Software da FIAP.

## Link do projeto
https://wokwi.com/projects/409934446038996993

## Link do vídeo explicativo
https://youtu.be/wZQZZX42_4U

## Como usar
1. Inicie a simulaçao no Wokwi
2. Inicie o broker MQTT
3. Aguarde o wokwi conectar-se ao broker
4. No broker, inscreva-se ao tópico "fiapers/dados"
5. Ajuste o pulso (potenciômetro) e a temperatura corporal (DHT)
6. Inicie o node red, adicione o flow do projeto e abra a dashboard
7. Verá que as informações atualizadas serão apresentadas na tela

## Requisitos
- Wokwi
- HiveMQ WebsocketClient
- Node Red
- 1 potenciômetro
- 1 servo motor
- 1 DHT22
- 2 Buzzers
- 1 ESP 32

## Dependências
- Biblioteca DHT sensor library for ESPx (Wokwi)
- Biblioteca ESP32Servo (Wokwi)
- Biblioteca ArduinoJson (Wokwi)
- Biblioteca PubSubClient (Wokwi)
- Biblioteca node-red-dashboard (Node Red)
- Biblioteca node-red-node-serialport (Node Red)

## Autores
- João Pedro de Souza Lopes (RM 557993)
- Lorenzzo Vendrusculo Dias (RM 558305)
- pedro Henrique Angyalossy Corazza Ferronato (RM 554757)
- Miguel Marques Lourenço de Souza (RM 555426)
- Gabriel Martins Vannucci (RM 556883)
