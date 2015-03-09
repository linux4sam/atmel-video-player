# atmel-video-player

## Features

Modern feature rich video player (widget) with the following requirements:  

1.  Qt5 based

2. Utilizes multiple framebuffer feature of the TARGET

3. Implements alpha blending, d. Provides information on FPS and CPU utilization when decoding a video

## Usage

In the current video player, the video will be rendered to FB0, so the GUI should be displayed 
in other device as FB1. Users can interact with the application by using a mouse connected to the board.

To enable mouse and rendering to FB1 launch the application using the following command.

```bash
 ./player -platform linuxfb:fb=/dev/fb1 -plugin EvdevMouse
```

## Known Issues and Limitations

1. No seeking support is available yet.

2. No audio support is available yet.

3. Only H264 is supported at the moment.