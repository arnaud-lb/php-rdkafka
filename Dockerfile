ARG PHP_IMAGE_TAG

FROM php:${PHP_IMAGE_TAG}

ARG LIBRDKAFKA_VERSION
ARG LIBSSL_VERSION

COPY . /php-rdkafka

# Install packages
RUN apk --no-cache upgrade && \
    apk --no-cache add bash openssh sudo git gcc g++ make autoconf \
    icu libssl${LIBSSL_VERSION} openssl-dev pcre-dev zlib-dev icu-dev wget valgrind

# Install librdkafka and ext-rdkafka
RUN git clone --depth 1 --branch ${LIBRDKAFKA_VERSION} https://github.com/edenhill/librdkafka.git \
    && cd librdkafka \
    && ./configure \
    && make \
    && make install \
    && cd /php-rdkafka \
    && phpize \
    && CFLAGS='-Werror=implicit-function-declaration' ./configure \
    && make \
    && cp modules/rdkafka.so "$(php -r 'echo ini_get("extension_dir");')" \
    && docker-php-ext-enable rdkafka

WORKDIR /php-rdkafka
