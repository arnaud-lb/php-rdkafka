#!/bin/sh
mkdir -p ${HOME}/kafka
if [ ! -f "/kafka/kafka_2.12-2.3.0.tgz" ]; then
    wget -O ${HOME}/kafka/kafka_2.12-2.3.0.tgz http://ftp.man.poznan.pl/apache/kafka/2.3.0/kafka_2.12-2.3.0.tgz;
fi
tar -xzf ${HOME}/kafka/kafka_2.12-2.3.0.tgz
${HOME}/kafka/kafka_2.12-2.3.0/bin/zookeeper-server-start.sh -daemon kafka_2.12-2.3.0/config/zookeeper.properties
${HOME}/kafka/kafka_2.12-2.3.0/bin/kafka-server-start.sh -daemon kafka_2.12-2.3.0/config/server.properties
