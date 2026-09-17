import json
import time
import os
import paho.mqtt.client as mqtt
from dotenv import load_dotenv

load_dotenv()

MQTT_USER = os.getenv("MQTT_USER")
MQTT_PASS = os.getenv("MQTT_PASS")

broker, port = "e0722a79b31148c9b57155e48211f476.s1.eu.hivemq.cloud", 8883
buffer = {"timestamp": 0.0, "horas_operacao": 0.0, "temperatura": 0.0, "umidade": 0.0, "vibracao": 0.0}

os.makedirs("stream_data", exist_ok=True)

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("\nConectado com sucesso ao HiveMQ Cloud!")
        client.subscribe("ctw/joao/#")
        print("📡 Escutando tópicos: ctw/joao/#\n")
    else:
        print(f"\nErro na conexão MQTT. Código: {rc} (Verifique usuário/senha no .env)\n")

def on_message(client, userdata, msg):
    try:
        val = float(msg.payload.decode())
        print(f"Chegou dado -> Tópico: {msg.topic} | Valor: {val}")

        if msg.topic == "ctw/joao/temperature": 
            buffer["temperatura"] = val
        elif msg.topic == "ctw/joao/humidity":
            buffer["umidade"] = val
        elif msg.topic == "ctw/joao/pot1": 
            buffer["horas_operacao"] = (val / 4095.0) * 100.0
        elif msg.topic == "ctw/joao/pot2": 
            buffer["vibracao"] = (val / 4095.0) * 5.0

        buffer["timestamp"] = time.time()

        file_path = f"stream_data/data_{int(buffer['timestamp'] * 1000)}.json"
        with open(file_path, "w") as f:
            json.dump(buffer, f)
        print(f"JSON gerado com sucesso: {file_path}")
        
    except Exception as e:
        print(f"Erro ao processar mensagem: {e}")

client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION1)
client.tls_set()
client.username_pw_set(MQTT_USER, MQTT_PASS)

client.on_connect = on_connect
client.on_message = on_message

print("Conectando ao broker MQTT...")
client.connect(broker, port)
client.loop_forever()