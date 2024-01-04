FROM debian:bullseye as build

RUN apt-get update --allow-releaseinfo-change && apt-get install -y \
    git \
    gcc-10 \
    python3-pip \
    python3 \
    cmake

RUN pip install conan==1.62.0
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
RUN rm -rf build && mkdir build
RUN ls
RUN ls ..
RUN cd build && \
    pwd && \
    conan install .. && \
    cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_FLAGS="-finstrument-functions" -DCMAKE_CXX_FLAGS="-finstrument-functions" && \
    cmake --build .

FROM debian:bullseye

COPY --from=build /usr/src/service-b/build/bin/serviceB /app/serviceB

# Start the service
CMD ["/app/serviceB"]