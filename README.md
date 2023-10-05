# rnnoise-ffmpeg
FFMPEG Wrapper around the RNNoise audio noise suppression library packaged in a Docker

## Compilation
```
docker build . -t rnnoise-ffmpeg
```

### Build for multi-platform
```
docker buildx build --platform linux/amd64,linux/arm64 -t rnnoise-ffmpeg .
```

## Prebuilt docker

Available at https://hub.docker.com/repository/docker/simonblandford/rnnoise-ffmpeg

## Usage examples
Process all the MP3 files in a directory, in, and write the processed files to a directory, out. This invokes the default `rnnoise-batch-mp3.sh` script.

```
docker run --rm -v "$( pwd )/in:/in" -v "$( pwd )/out:/out" rnnoise-ffmpeg
```
___
List available commands
```
docker run --rm rnnoise-ffmpeg /bin/ls /usr/local/bin
```
---
Get usage information for the rnnoise-ffmpeg command.

```
docker run --rm rnnoise-ffmpeg rnnoise-ffmpeg
```
---
Process in.mp3 and write the output in M4A format to a directory, out.

```
docker run --rm -v "$( pwd )/in.mp3:/in.mp3" -v "$( pwd )/out:/out" rnnoise-ffmpeg rnnoise-ffmpeg /in.mp3 /out/out.m4a "-ac 1 -c:a aac -b:a 64k"
```
___
Process in.mp3 and output out.mp3. Input and output pipes are used to the docker to avoid bind mounting.

```
cat "in.mp3" | docker run --rm -i rnnoise-ffmpeg rnnoise-ffmpeg - - "-c:a libmp3lame -b:a 64k -f mp3" "" "-f mp3" >out.mp3
```
___
Process in.mp3 and output out.mp3 with input pre-processing to remove silence and add compression. Input and output pipes are used to the docker to avoid bind mounting.

```
cat "in.mp3" | docker run --rm -i rnnoise-ffmpeg rnnoise-ffmpeg - - "-c:a libmp3lame -b:a 64k -f mp3" "-filter:a silenceremove=start_periods=1:start_duration=1:start_threshold=-75dB:detection=peak,alimiter=level_out=0.9:limit=0.1:release=200:asc=1:asc_level=0.2" "-f mp3" >out.mp3
```
___
Process in.mp3 and output out.mp3 with output post-processing to remove silence and add compression. Input and output pipes are used to the docker to avoid bind mounting.

```
cat "in.mp3" | docker run --rm -i rnnoise-ffmpeg rnnoise-ffmpeg - - "-filter:a silenceremove=start_periods=1:start_duration=1:start_threshold=-75dB:detection=peak,alimiter=level_out=0.9:limit=0.1:release=200:asc=1:asc_level=0.2 -c:a libmp3lame -b:a 64k -f mp3" "" "-f mp3" >out.mp3
```
