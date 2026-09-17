# Monitoramento Industrial e Predição de Desgaste com Machine Learning em Tempo Real

Projeto prático desenvolvido para a disciplina de **Big Data e Tratamento de Dados** do curso técnico em **Cibersistemas para Automação**. 

O sistema simula a coleta de dados telemétricos de sensores industriais (horas de operação, temperatura, umidade e vibração) em um microcontrolador ESP32 via **Wokwi**, envia essas leituras através do protocolo **MQTT (HiveMQ Cloud)** e executa uma pipeline de streaming de dados com **PySpark (MLlib)**. O modelo preditivo de Regressão Linear calcula a estimativa de desgaste do equipamento em tempo real e classifica seu estado operacional.

---

## 🏗️ Arquitetura do Sistema

```text
[ Simulador ESP32 (Wokwi) ] 
            │ (MQTT / JSON)
            ▼
   [ Broker HiveMQ Cloud ]
            │ (Ponte de Transmissão / Stream)
            ▼
    [ Pasta stream_data/ ]
            │ (Processamento em Tempo Real)
            ▼
[ Pipeline PySpark + MLlib ] ──► Modelo de Regressão Linear
            │
            ▼
[ Arquivo predictions/predictions.csv ] ──► Regressão + Status (Normal / Atenção / Crítico)

```

---

## 🛠️ Tecnologias Utilizadas

| Camada | Tecnologia | Função |
| --- | --- | --- |
| **Simulação & Hardware** | ESP32 / PlatformIO / Wokwi | Geração e leitura dos dados dos sensores |
| **Comunicação** | MQTT / HiveMQ Cloud | Protocolo de mensagens leve para IoT |
| **Processamento & ML** | PySpark (Spark Context / MLlib) | Ingestão em lote/stream e Machine Learning |
| **Linguagens** | C++ (Firmware) / Python 3.x | Desenvolvimento do microcontrolador e pipeline PySpark |

---

## 📁 Estrutura do Repositório

```text
.
├── include/
│   └── credentials.h.example  # Modelo para dados do Wi-Fi e MQTT
├── py/
│   └── main.py                # Pipeline PySpark (Treinamento + Predição Stream)
├── src/
│   └── main.cpp               # Firmware do ESP32 para o Wokwi
├── .env.example               # Modelo de variáveis de ambiente do Python
├── .gitignore                 # Filtro para ignorar senhas, cache e arquivos temporários
├── diagram.json               # Esquemático de conexões do Wokwi
├── platformio.ini             # Configuração de dependências do PlatformIO
└── wokwi.toml                 # Configuração do simulador Wokwi

```

---

## ⚙️ Configuração e Instalação

### 1. Pré-requisitos

* **Python 3.8+**
* **Java JDK 8 ou 11** (necessário para execução do Apache Spark)
* **VS Code** com as extensões **PlatformIO** e **Wokwi Simulator**

### 2. Configuração de Credenciais

Antes de executar o projeto, configure suas credenciais de rede e broker MQTT.

* **Firmware (C++):** Crie uma cópia do arquivo `include/credentials.h.example`, renomeie para `include/credentials.h` e preencha suas chaves:
```cpp
#define WIFI_SSID     "Wokwi-GUEST"
#define WIFI_PASS     ""
#define MQTT_USER     "seu_usuario"
#define MQTT_PASS     "sua_senha"

```


* **Python/PySpark:** Crie uma cópia do arquivo `.env.example`, renomeie para `.env` e insira os mesmos dados:
```env
MQTT_USER=seu_usuario
MQTT_PASS=sua_senha

```



---

## 🚀 Como Executar

**1. Instalar as dependências do Python**

```bash
pip install pyspark python-dotenv

```

**2. Iniciar a Pipeline do PySpark**
Execute o script em Python para treinar o modelo preditivo com os dados históricos e aguardar a chegada das leituras em tempo real:

```bash
python py/main.py

```

**3. Iniciar a Simulação IoT (Wokwi)**

* Abra o projeto no VS Code.
* Inicie a simulação pelo arquivo `diagram.json` ou execute a extensão do Wokwi.
* O ESP32 começará a transmitir os dados simulados via MQTT.

---

## 📊 Regras de Classificação Preditiva

O modelo avalia a combinação ponderada dos fatores (dando maior impacto à vibração e horas de uso) e classifica a predição de desgaste nas seguintes faixas:

* **Normal:** Predição de desgaste < 40%
* **Atenção:** Predição de desgaste entre 40% e 69%
* **Crítico:** Predição de desgaste >= 70%

Os resultados processados são exibidos no console e gravados de forma incremental em `predictions/predictions.csv`.

```

```
