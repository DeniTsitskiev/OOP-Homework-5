FROM alpine:3.19

RUN apk add --no-cache build-base cmake git

WORKDIR /app

COPY . .

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build \
    && ctest --test-dir build --output-on-failure

ENTRYPOINT ["sh", "-c", "if [ \"$1\" = tests ]; then shift; exec ctest --test-dir build \"$@\"; elif [ $# -gt 0 ]; then exec \"$@\"; else exec ./build/hw5_app; fi", "entrypoint"]

