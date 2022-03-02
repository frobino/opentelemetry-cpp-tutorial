FROM ubuntu:20.10

# Working directory
WORKDIR /usr/src/
COPY . /usr/src/

# Install dependencies
RUN apt-get update && \
    apt-get install -y cmake build-essential git rapidjson-dev && \
    apt install -y software-properties-common    

# Build the service
WORKDIR /usr/src/service-a/
RUN mkdir build && cd build && \
    cmake -DBUILD_TESTING=OFF -DWITH_EXAMPLES=OFF -DWITH_JAEGER=ON .. && \
    make

# Start the service
CMD ["./build/serviceA"]