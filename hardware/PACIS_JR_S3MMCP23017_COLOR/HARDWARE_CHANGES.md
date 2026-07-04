# PACIS Jr s3Mmcp23017 Hardware Changes

This note compares the original Junior hardware idea to the current `Pacis_jr_s3Mmcp23017` sketch.

## Keep The Same

| Circuit | Keep |
| --- | --- |
| Zone buttons | ESP32 GPIO12, GPIO14, GPIO27, GPIO26, GPIO25 |
| PTT button | ESP32 GPIO33 |
| W.N. button | ESP32 GPIO32 |
| RS485 | GPIO16 TX, GPIO34 RX, GPIO13 EN to MAX3485 |
| Heartbeat/status LED | ESP32 GPIO2 |
| MCP23017 I2C | ESP32 GPIO21 SDA, GPIO22 SCL |
| MCP23017 power | 3.3 V and GND |

## Change The Panel LEDs

All panel LED control moves to the MCP23017. The ESP32 direct LED pins from the old Junior sketch are no longer used for panel lamps.

| Old use | New use |
| --- | --- |
| ESP32 GPIO23 Zone 1 LED | Disconnect from lamp driver input; Zone 1 green moves to MCP23017 GPA0 |
| ESP32 GPIO22 Zone 2 LED | Disconnect from lamp driver input; Zone 2 green moves to MCP23017 GPA1 |
| ESP32 GPIO21 Zone 3 LED | Disconnect from lamp driver input; Zone 3 green moves to MCP23017 GPA2 |
| ESP32 GPIO19 Zone 4 LED | Disconnect from lamp driver input; Zone 4 green moves to MCP23017 GPA3 |
| ESP32 GPIO18 Zone 5 LED | Disconnect from lamp driver input; Zone 5 green moves to MCP23017 GPA4 |
| ESP32 GPIO5 PTT LED | Remove; PTT has no LED |
| ESP32 GPIO17 W.N. LED | Disconnect from lamp driver input; W.N. green moves to MCP23017 GPA5 |

## MCP23017 Lamp Driver Wiring

Use the MCP23017 outputs to drive ULN2803A inputs or equivalent transistor driver inputs. Do not drive panel lamps directly from the MCP23017 pins.

| MCP23017 pin | Connect to |
| --- | --- |
| GPA0 | Zone 1 green driver input |
| GPA1 | Zone 2 green driver input |
| GPA2 | Zone 3 green driver input |
| GPA3 | Zone 4 green driver input |
| GPA4 | Zone 5 green driver input |
| GPA5 | W.N. green driver input |
| GPA6 | Spare |
| GPA7 | Spare |
| GPB0 | Zone 1 red driver input |
| GPB1 | Zone 2 red driver input |
| GPB2 | Zone 3 red driver input |
| GPB3 | Zone 4 red driver input |
| GPB4 | Zone 5 red driver input |
| GPB5 | Spare; W.N. is green only |
| GPB6 | Spare |
| GPB7 | Spare |

## Behavior Change

| PACIS/server state | Lamp behavior |
| --- | --- |
| Zone selected/approved | Zone green ON |
| Zone busy/denied/old blink state | Zone red ON solid |
| Zone off | Green OFF, red OFF |
| PTT active | No lamp |
| W.N. active | W.N. green ON |

## Bench Checklist

1. Confirm MCP23017 address jumpers are set to `0x20`.
2. Power MCP23017 from 3.3 V.
3. Confirm SDA/SCL pullups are to 3.3 V, not 5 V.
4. Move all panel LED driver inputs from direct ESP32 LED pins to the MCP23017 outputs above.
5. Leave button wiring on the original ESP32 pins.
6. Leave RS485 wiring on the original MAX3485 pins.
7. Leave heartbeat/status LED on ESP32 GPIO2.
8. Do not wire a PTT lamp.
