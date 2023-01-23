
## Stream to YouTube

```
ffmpeg -stream_loop -1 -i /usr/lib/silence.aac 
  -rtsp_transport tcp -i rtsp://127.0.0.1:554/stream=0 -c:v copy -c:a copy -f flv rtmp://a.rtmp.youtube.com/live2/$TOKEN
```
