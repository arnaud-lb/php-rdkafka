#!/bin/sh

docker network create kafka_network
docker pull wurstmeister/zookeeper:latest
docker run -d --network kafka_network --name zookeeper wurstmeister/zookeeper:latest
docker pull wurstmeister/kafka:latest
docker run -d -p 9092:9092 --network kafka_network -e "KAFKA_AUTO_CREATE_TOPICS_ENABLE=true" -e "KAFKA_CREATE_TOPICS=test-topic:1:1:compact" -e "KAFKA_ADVERTISED_HOST_NAME=kafka" -e "KAFKA_ZOOKEEPER_CONNECT=zookeeper:2181" -e "KAFKA_ADVERTISED_PORT=9092" --name kafka wurstmeister/kafka:latest
printf "\n127.0.0.1  kafka\n"|sudo tee /etc/hosts >/dev/null

echo "Waiting for Kafka to be ready"

for i in $(seq 1 20); do
    if kafkacat -b 127.0.0.1 -L; then
        echo "Kafka is ready"
        exit 0
    fi
done

echo "Timedout waiting for Kafka to be ready"
exit 1
