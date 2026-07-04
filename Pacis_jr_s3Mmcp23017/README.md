# Pacis_jr-s3Mmcp23017

Version: `01.04`
Build: `PACIS_JR_S3MMCP23017_COLOR_2026_07_03`
Build time: `2026-07-03 22:35:00 -04:00`

This sketch keeps the Junior PACIS logic and adapts only the panel hardware behavior.

## Facts Locked In

| Function | Hardware |
| --- | --- |
| Zone buttons | Original Junior GPIO pins |
| PTT button | Original Junior GPIO pin, button only |
| W.N. button | Original Junior GPIO pin, same logic as Junior |
| Zone green LEDs | MCP23017 outputs |
| Zone red/busy LEDs | MCP23017 outputs |
| W.N. green LED | MCP23017 output |
| Heartbeat/status | Original Junior heartbeat GPIO pin |
| RS485 | Original Junior MAX3485 wiring |

## Behavior

| State | LED result |
| --- | --- |
| Zone selected/approved | Green |
| Zone busy/denied/old blink state | Solid red |
| Zone off | Off |
| PTT | No LED |
| W.N. active | Original W.N. LED behavior |

## MCP23017 LED Outputs

| MCP23017 pin | Output |
| --- | --- |
| GPA0 | Zone 1 green |
| GPA1 | Zone 2 green |
| GPA2 | Zone 3 green |
| GPA3 | Zone 4 green |
| GPA4 | Zone 5 green |
| GPA5 | W.N. green |
| GPB0 | Zone 1 red |
| GPB1 | Zone 2 red |
| GPB2 | Zone 3 red |
| GPB3 | Zone 4 red |
| GPB4 | Zone 5 red |
| GPB5 | Spare |

The sketch currently uses:

```cpp
I2C_SDA_PIN = 21;
I2C_SCL_PIN = 22;
MCP23017_ADDRESS = 0x20;
```

Power the MCP23017 from 3.3 V so I2C stays safe for the ESP32.
