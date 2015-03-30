# php-rdkafka

PHP-rdkafka builds on top of [librdkafka](https://github.com/edenhill/librdkafka) to provide a working PHP client for [Kafka](https://kafka.apache.org/) 0.8 (and potentially old versions supported by librdkafka).

## Installation

This is a standard PHP extension:

    phpize
    ./configure
    make
    sudo make install
    # Add extension=rdkafka.so to your php.ini:
    echo extension=rdkafka.so|sudo tee -a /path/to/php.ini
