FROM ubuntu:jammy AS build

RUN apt update -y && apt install -y \
    build-essential \
    autoconf \
    libtool \
    git \
    && rm -rf /var/lib/apt/lists/*

RUN cd /usr/local/src && \
    git clone "https://github.com/xiph/rnnoise.git" && \
    cd rnnoise && \
    ./autogen.sh && \
    ./configure && \
    make -j 4 && \
    make install

RUN mkdir -p /usr/local/src/rnnoise_ffmpeg
COPY rnnoise_demo.c /usr/local/src/rnnoise_ffmpeg/rnnoise_demo.c
RUN cd /usr/local/src/rnnoise_ffmpeg && \
    gcc rnnoise_demo.c -static -lrnnoise -lm -o rnnoise_ffmpeg && \
    install rnnoise_ffmpeg /usr/local/bin/rnnoise_ffmpeg

FROM ubuntu:jammy

RUN apt update -y && apt install -y \
    mp3gain \
    mediainfo \
    ffmpeg \
    && rm -rf /var/lib/apt/lists/*

RUN mkdir -p /in /out

COPY --from=build /usr/local/bin/rnnoise_ffmpeg /usr/local/bin/rnnoise_ffmpeg
COPY rnnoise_batch_docker.sh /usr/local/bin/rnnoise_batch.sh
RUN chmod 0755 /usr/local/bin/rnnoise_batch.sh

CMD /usr/local/bin/rnnoise_batch.sh
