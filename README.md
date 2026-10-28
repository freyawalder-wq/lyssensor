# Lyssensor

Arduino-sketch that reads two light sensors and sends their values over the
serial port as newline-terminated text.

## Sensors

- **VEML7700** on I2C address `0x10`, reported as lux at `/lys/sensor1`.
- **Grove Light Sensor v1.0** connected to analog input `A0`, reported as the
  raw analog-to-digital converter (ADC) reading at `/lys/sensor2`.

The Grove value is a raw board-dependent reading, not a calibrated lux value.
On boards with a 10-bit ADC it is normally in the range `0` to `1023`.

## Serial output

The sketch uses **115200 baud** and sends one line approximately every
250 milliseconds. Example:

```text
/lys/sensor1 12.34
/lys/sensor2 518
```

These are text lines with OSC-style addresses, not binary OSC packets. Serial
error messages start with `# ERROR:`.

## Wiring

- Connect the VEML7700 to the board's I2C SDA/SCL pins, power, and ground.
- Connect the Grove Light Sensor v1.0 signal output to `A0`; connect power and
  ground as required by the board and sensor.
- Ensure that both sensors and the Arduino board use compatible logic voltage.

## Upload and use

1. Open `Lyssensor_sketch_r3.ino` in the Arduino IDE.
2. Select the correct board and its serial port.
3. Upload the sketch.
4. Open Serial Monitor at **115200 baud** to view readings.

The VEML7700 is retried if initialization or a reading fails. The analog
sensor continues to report while the VEML7700 is unavailable.
