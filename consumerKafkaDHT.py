from confluent_kafka import Consumer, TopicPartition
import matplotlib.pyplot as plt
from matplotlib import style
import socket
import matplotlib
import os
import time
import json
from datetime import datetime


topics = [
    TopicPartition("test", 0, 0),
    TopicPartition("temperature", 0, 0),
    TopicPartition("humidity", 0, 0),
]
conf = {
    # Adresse du broker Kafka (par défaut le cloud lui-même)
    "bootstrap.servers": os.environ.get("KAFKA_BOOTSTRAP_SERVERS", "localhost:9092"),
    "client.id": socket.gethostname(),
    "group.id": "test-consumer-group",
    # "schema.registry.url": "http://localhost:8081",
}

graphing = "s"
while graphing not in ["y", "", "n"]:
    graphing = input("Would you like to graph the data? (y/n): ").lower()
if graphing != "y":
    graphing = False
else:
    graphing = True


def freq(T):
    S = 0
    for i in range(1, len(T)):
        S += T[i] - T[i - 1]
    if S == 0:
        return 0
    return (len(T) - 1) / S


if graphing:
    matplotlib.use("TkAgg")

    style.use("fivethirtyeight")

    fig = plt.figure()
    ax1 = fig.add_subplot(1, 1, 1)

xTemp = []
Temp = []
xHumi = []
Humi = []


consumer = Consumer(conf)
# consumer.assign(topics)
consumer.subscribe(["test", "temperature", "humidity"])

if graphing:
    plt.ion()
    plt.show()

n = 100  # Number of last received values to display
nm = 250  # Number of last received values to calculate frequency
na = 2  # Number of last recived values to display at once
NA = 0


def update_plot():
    global NA
    if NA == 0:
        ax1.clear()
        ax1.plot(xTemp, Temp, "r", label="Temperature")
        ax1.plot(xHumi, Humi, "b", label="Humidity")
        plt.draw()
        plt.legend()
        plt.pause(0.000000000001)
        NA += 1
        NA %= na
    else:
        NA += 1
        NA %= na


def extract_value(topic, value):
    # Les messages de l'ESP32 sont au format {"schema": ..., "payload": {...}}
    try:
        data = json.loads(value)
    except ValueError:
        return float(value)
    if isinstance(data, dict):
        return float(data.get("payload", data)[topic])
    return float(data)


def process_message(topic, value, timestamp_ms):
    timestamp = timestamp_ms / 1000.0 - T0
    try:
        number = extract_value(topic, value)
    except (KeyError, TypeError, ValueError):
        print(f"Cannot plot message from topic {topic}: {value}")
        return
    if topic == "temperature":
        xTemp.append(timestamp)
        Temp.append(number)
    elif topic == "humidity":
        xHumi.append(timestamp)
        Humi.append(number)
    # Keep only the last n elements
    if len(xTemp) > n:
        xTemp.pop(0)
    if len(Temp) > n:
        Temp.pop(0)
    if len(xHumi) > n:
        xHumi.pop(0)
    if len(Humi) > n:
        Humi.pop(0)
    update_plot()


T = [time.time()]
T0 = time.time()

try:
    while True:
        msg = consumer.poll(1.0)
        if msg is None:
            print("Waiting...")
        elif msg.error():
            print(f"ERROR: {msg.error()}")
        else:
            T.append(time.time())
            if len(T) > nm:
                T.pop(0)
            value = msg.value()
            topic = msg.topic()
            timestamp = datetime.fromtimestamp(msg.timestamp()[1] / 1000.0).strftime(
                "%Y-%m-%d %H:%M:%S.%f"
            )[:-3]
            print(f"Received message: {value} at {timestamp:22}.", end="")
            print(
                f" Time since last message: {T[-1] - T[-2]:23}. Average frequency ({nm} messages): {freq(T):20}",
                end="",
            )
            print("")
            if graphing:
                process_message(topic, value, msg.timestamp()[1])
except KeyboardInterrupt:
    pass
finally:
    consumer.close()
