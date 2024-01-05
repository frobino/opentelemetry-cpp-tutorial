# opentelemetry-cpp-tutorial

Inspired from this [blog](https://logz.io/blog/cplusplus-opentelemetry-tracing/#start),
and this [otel.logz.io](https://github.com/dawidborycki/otel.logz.io) project.

## How to (local):

For each service (i.e. service-a and service-b), build:

```
cd service-a
mkdir build && cd build
conan install ..
# Note: CMAKE_C and CXX flags are optional
cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_FLAGS="-finstrument-functions" -DCMAKE_CXX_FLAGS="-finstrument-functions"
bear -- cmake --build .
```

For each service (i.e. service-a and service-b), start the process in a separate shell:

```
cd service-a/build
./bin/serviceA
```

Now use a third shell to trigger the functionality:

```
# Trigger serviceA, which will trigger serviceB
curl localhost:8081/ping
# Trigger serviceB
curl localhost:8082/ping
```

How to create user-space traces for a process:

```
cd service-b/build/bin
lttng create serviceB-session
lttng enable-channel u -u --subbuf-size 1024K --num-subbuf 8
lttng enable-event -c u -u lttng_ust_cyg_profile*,lttng_ust_statedump*
lttng add-context -c u -u -t vpid -t vtid
lttng start
LD_PRELOAD="liblttng-ust-cyg-profile.so liblttng-ust-libc-wrapper.so" ./service-b
lttng destroy
```

## How to (Docker):

From the root folder of this repo:

```bash
# 1. build the images, newtworks etc
docker-compose build
# 2. create the containers
docker compose create
# 3. start the containers
docker compose start
# 4. open jaeger at http://localhost:16686/
# 5. curl the service A, which will trigger service B
curl 0.0.0.0:8081/ping
# 6. look for the trace in jaeger
# 7. stop the containers
docker compose stop
```
