# opentelemetry-cpp-tutorial

Inspired from this [blog](https://logz.io/blog/cplusplus-opentelemetry-tracing/#start),
and this [otel.logz.io](https://github.com/dawidborycki/otel.logz.io) project.

## How to:

```
conan install ..
cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug
cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_FLAGS="-finstrument-functions" -DCMAKE_CXX_FLAGS="-finstrument-functions"
bear -- cmake --build .

lttng create
lttng enable-channel u -u --subbuf-size 1024K --num-subbuf 8
lttng enable-event -c u -u lttng_ust_cyg_profile*,lttng_ust_statedump*
lttng add-context -c u -u -t vpid -t vtid
lttng start
LD_PRELOAD="liblttng-ust-cyg-profile.so liblttng-ust-libc-wrapper.so" ./service-b
lttng destroy
```
