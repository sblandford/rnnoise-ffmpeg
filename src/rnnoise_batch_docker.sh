#!/bin/bash

REQUIRED_APPS="mp3gain rnnoise_ffmpeg mediainfo"
OUTDIR="/out"
INDIR="/in"

for i in $( echo "$REQUIRED_APPS" ); do
    if ! which "$i" 2>&1 >/dev/null; then
      echo "Required application missing : $i"
      flunked="yes"
    fi
done

[[ $flunked ]] && exit 1

mkdir -p "$OUTDIR"
cd "$INDIR"
IFS=$'/n'
for i in *.mp3; do
    [[ "$i" == "*.mp3" ]] && continue
    echo "$i"
    j="$INDIR/$i"
    [[ -f "$j" ]] || continue
    info=$( mediainfo "$j" )
    # Get integer kbps, ignore anything after decimal point
    kbps=$( echo "$info" | grep -P "Overall bit rate\s+:\s+[0-9]+" | grep -Po "[0-9]+" | head -n 1)
    # Determine if variable bit rate
    # echo "$info" | grep -F "Overall bit rate mode" | grep -Fq "Variable" && variable="yes"
    # Work out likely variable bitrate setting
    codec="-b:a $kbps""k"
    v=9
    if [[ $variable ]]; then
        if [[ $kbps -gt 230 ]]; then
            v=0
        elif [[ $kbps -gt 200 ]]; then
            v=1
        elif [[ $kbps -gt 185 ]]; then
            v=2
        elif [[ $kbps -gt 170 ]]; then
            v=3
        elif [[ $kbps -gt 155 ]]; then
            v=4
        elif [[ $kbps -gt 125 ]]; then
            v=5
        elif [[ $kbps -gt 110 ]]; then
            v=6
        elif [[ $kbps -gt 90 ]]; then
            v=7
        elif [[ $kbps -gt 75 ]]; then
            v=8
        fi
        codec="-q:a $v"
    fi
    mp3gain -r "$j"
    outfile="$OUTDIR/$( echo "$i" | tr -d "[:blank:]" )"
#    kbps=$( ffprobe "$j" 2>&1 | \
#        grep -P "Stream.*Audio" | grep -oP "[0-9]+\s*kb/s" | \
#        grep -oP "[0-9]+" | head -n 1 )
    rnnoise_ffmpeg "$j" "$outfile" \
        "-ac 1 -c:a libmp3lame $codec" \
        "silenceremove=start_periods=1:start_duration=1:start_threshold=-75dB:detection=peak,aformat=dblp,extrastereo=m=0.0,alimiter=level_out=0.8:limit=0.2:release=200:asc=1:asc_level=0.2"
done

