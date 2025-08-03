FROM ubuntu:latest
ARG DEBIAN_FRONTEND=noninteractive

ARG DB_HOST
ARG DB_USER
ARG DB_NAME
ARG DB_PASSWD
ARG ISSUER
ARG AUTH_KEY
ARG DB_PORT
ARG PORT

RUN apt-get update && apt-get install -y \
    build-essential cmake git wget curl \
    libjsoncpp-dev uuid-dev openssl libssl-dev zlib1g-dev \
    libpq-dev libsqlite3-dev gdb gcc g++ zip unzip tar pkg-config \
    ninja-build bison flex autoconf make ca-certificates python3

RUN git clone --depth 1 https://github.com/microsoft/vcpkg

# NastavenÌ promÏnn˝ch
ENV VCPKG_ROOT=/opt/vcpkg
ENV PATH="$VCPKG_ROOT:$PATH"    

RUN git clone https://github.com/microsoft/vcpkg.git $VCPKG_ROOT && \
    $VCPKG_ROOT/bootstrap-vcpkg.sh

RUN git -C $VCPKG_ROOT pull && vcpkg update
RUN rm -rf $VCPKG_ROOT/downloads/*



WORKDIR /app
COPY . .


# Build projektu
RUN cmake -Bbuild -S. \
    -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake -GNinja && \
    cmake --build build --config Release --parallel

# Exponuj port
EXPOSE $PORT

WORKDIR /app/build

# Spusù server
CMD ["./teplomerAPI"]