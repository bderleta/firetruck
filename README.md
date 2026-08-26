# Firetruck Controller

Controller software for a Wi-Fi-enabled firetruck. The device exposes an HTTP
API for driving, steering, ladder and turntable control, the water pump, and
emergency lights.

## Connecting

The controller starts a Wi-Fi access point with the following settings:

| Setting | Value |
| --- | --- |
| Address | <http://192.168.4.1> |
| SSID | `ToyControl` |
| Password | `12345678` |

## HTTP API

Send `GET` requests to the controller's address. For example:

```text
http://192.168.4.1/drive?cmd=forward&speed=300
```

### Drive

| Endpoint | Description |
| --- | --- |
| `/drive?cmd=enable` | Enables the drive motor controller, headlights, and rear lights. |
| `/drive?cmd=disable` | Disables the drive motor controller and lights. |
| `/drive?cmd=forward&speed=<speed>` | Moves forward. `<speed>` must be between `200` and `400`. |
| `/drive?cmd=reverse&speed=<speed>` | Moves backward. `<speed>` must be between `200` and `400`. |
| `/drive?cmd=brake` | Stops the drive motor and enables braking. |
| `/drive?cmd=coast` | Stops the drive motor and disables braking. |

### Steering

| Endpoint | Description |
| --- | --- |
| `/steer?cmd=left` | Keeps turning the front wheels left. |
| `/steer?cmd=right` | Keeps turning the front wheels right. |
| `/steer?cmd=off` | Stops the steering motor and returns the wheels to center. |

### Turntable

| Endpoint | Description |
| --- | --- |
| `/turntable?cmd=left` | Keeps turning the turntable counterclockwise. |
| `/turntable?cmd=right` | Keeps turning the turntable clockwise. |
| `/turntable?cmd=off` | Stops the turntable motor. |

### Ladder

| Endpoint | Description |
| --- | --- |
| `/ladder?cmd=up` | Keeps increasing the ladder's length and reach. |
| `/ladder?cmd=down` | Keeps reducing the ladder's length and reach. |
| `/ladder?cmd=off` | Stops ladder adjustment. |

### Pump

| Endpoint | Description |
| --- | --- |
| `/pump?cmd=on` | Turns on the water pump. |
| `/pump?cmd=off` | Turns off the water pump. |

### Emergency lights

| Endpoint | Description |
| --- | --- |
| `/emergency_lights?cmd=on` | Turns on the emergency lights. |
| `/emergency_lights?cmd=off` | Turns off the emergency lights. |
