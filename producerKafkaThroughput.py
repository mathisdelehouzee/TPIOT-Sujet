from confluent_kafka import Producer
from random import randint
from time import time, sleep
import os
import socket
import sys

LastMessage = 0

topic = "test"
# Adresse du broker Kafka : argument de la ligne de commande, sinon variable
# d'environnement KAFKA_BOOTSTRAP_SERVERS, sinon le cloud lui-même.
# Depuis une autre machine (ex : le Raspberry Pi) : python producerKafkaThroughput.py <ip du cloud>:9093
bootstrap = sys.argv[1] if len(sys.argv) > 1 else os.environ.get("KAFKA_BOOTSTRAP_SERVERS", "localhost:9092")
conf = {
    "bootstrap.servers": bootstrap,
    "client.id": socket.gethostname(),
}
producer = Producer(conf)


def waitUntilNextMessage(interval):
    while time() - LastMessage < interval:
        sleep(0.001)


def produceMessage(topic, key, value):
    global LastMessage
    LastMessage = time()
    producer.produce(topic, key=key, value=value, callback=deliveryCallback)
    producer.poll(1)


def deliveryCallback(err, msg):
    if err:
        print(f"ERROR: Message failed delivery: {err}")
    else:
        topic = msg.topic()
        key = msg.key().decode("utf-8")
        value = msg.value().decode("utf-8")
        print(f"Produced event to topic {topic}: key = {key:12} value = {value:12}")


def produceRandomMessages(min=0, max=100, interval=10):
    while True:
        produceMessage(topic, "key", str(randint(min, max)))
        waitUntilNextMessage(interval)


def produceSlopeMesssages(max=25, interval=10):
    i = 0
    while True:
        produceMessage(topic, "key", str(i))
        waitUntilNextMessage(interval)
        i += 1
        i %= max


def testProducer(interval=0.001, random=""):
    while True:
        while type(interval) != float:
            try:
                interval_input = input("Enter the interval between messages: ")
                if "/" in interval_input:
                    numerator, denominator = map(float, interval_input.split("/"))
                    interval = numerator / denominator
                else:
                    interval = float(interval_input)
            except ValueError:
                print("Please enter a valid number or fraction.")
        while random not in ["y", "", "n"]:
            random = input("Would you like to produce random messages? (y): ").lower()
        if random == "y":
            produceRandomMessages(interval=interval)
        else:
            produceSlopeMesssages(interval=interval)


if __name__ == "__main__":
    testProducer()
