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

RUN mkdir -p /usr/local/src/rnnoise-ffmpeg
COPY src/rnnoise_demo.c /usr/local/src/rnnoise-ffmpeg/rnnoise_demo.c
RUN cd /usr/local/src/rnnoise-ffmpeg && \
    gcc rnnoise_demo.c -static -lrnnoise -lm -o rnnoise-ffmpeg && \
    install rnnoise-ffmpeg /usr/local/bin/rnnoise-ffmpeg

FROM ubuntu:jammy

RUN apt update -y && apt install -y \
    mp3gain \
    mediainfo \
    ffmpeg \
    && rm -rf /var/lib/apt/lists/*

RUN mkdir -p /in /out

COPY --from=build /usr/local/bin/rnnoise-ffmpeg /usr/local/bin/rnnoise-ffmpeg
COPY src/rnnoise-batch-mp3.sh /usr/local/bin/rnnoise-batch-mp3.sh
RUN chmod 0755 /usr/local/bin/rnnoise-batch-mp3.sh
RUN ln -s /usr/local/bin/rnnoise* /.

CMD /usr/local/bin/rnnoise-batch-mp3.sh
