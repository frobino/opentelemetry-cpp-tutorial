FROM debian:stable

# add non-free repos
RUN sed -i "s#deb http://deb.debian.org/debian buster main#deb http://deb.debian.org/debian testing main contrib non-free#g" /etc/apt/sources.list
RUN sed -i "s#deb http://security.debian.org/debian-security buster/updates main##g" /etc/apt/sources.list
RUN sed -i "s#deb http://deb.debian.org/debian buster-updates main#deb http://deb.debian.org/debian testing-updates main contrib non-free#g" /etc/apt/sources.list

RUN apt-get update --allow-releaseinfo-change && apt-get install -y \
    git \
    gcc-10 \
    python3-pip \
    python3 \
    cmake

RUN pip install conan
RUN conan profile new default --detect
RUN conan profile update settings.compiler.libcxx=libstdc++11 default

# Working directory
WORKDIR /usr/src/
COPY . /usr/src/service-b
COPY ./common ./common

# Install dependencies
# RUN apt-get update && \
#     apt-get install -y cmake build-essential git rapidjson-dev && \
#     apt install -y software-properties-common    

# Build the service
WORKDIR /usr/src/service-b/
RUN chmod -R 777 /usr/src/service-b/
RUN mkdir build
RUN ls
RUN ls ..
RUN cd build && \
    pwd && \
    conan install .. && \
    cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_FLAGS="-finstrument-functions" -DCMAKE_CXX_FLAGS="-finstrument-functions" && \
    cmake --build .

# Start the service
CMD ["./build/bin/serviceB"]
