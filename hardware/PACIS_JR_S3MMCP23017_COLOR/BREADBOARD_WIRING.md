# PACIS Jr s3Mmcp23017 Breadboard Wiring

Use this with sketch `Pacis_jr_s3Mmcp23017`.

## Power Rails

| Rail | Connects to |
| --- | --- |
| 3.3 V | ESP32 3V3, MCP23017 VCC, button pullups |
| 12 V | Illuminated button LED positive terminals |
| GND | ESP32 GND, MCP23017 GND, both ULN2803A GND pins, 12 V supply GND |

All grounds must be common.

## ESP32 Pins

| ESP32 pin | Connects to | Notes |
| --- | --- | --- |
| GPIO12 | Zone 1 button input | 10k pullup to 3.3 V, NO contact to GND |
| GPIO14 | Zone 2 button input | 10k pullup to 3.3 V, NO contact to GND |
| GPIO27 | Zone 3 button input | 10k pullup to 3.3 V, NO contact to GND |
| GPIO26 | Zone 4 button input | 10k pullup to 3.3 V, NO contact to GND |
| GPIO25 | Zone 5 button input | 10k pullup to 3.3 V, NO contact to GND |
| GPIO33 | PTT button input | 10k pullup to 3.3 V, NO contact to GND |
| GPIO32 | W.N. button input | 10k pullup to 3.3 V, NO contact to GND |
| GPIO16 | MAX3485 DI | RS485 TX |
| GPIO34 | MAX3485 RO | RS485 RX |
| GPIO13 | MAX3485 DE and /RE | HIGH transmit, LOW receive |
| GPIO21 | MCP23017 SDA | I2C data |
| GPIO22 | MCP23017 SCL | I2C clock |
| GPIO2 | Heartbeat/status LED | Existing local status LED |
| 3V3 | MCP23017 VCC and button pullups | Logic power |
| GND | Common ground | Required |

## MCP23017 Control Pins

| MCP23017 module pin | Connects to |
| --- | --- |
| VCC | 3.3 V |
| GND | Common GND |
| SDA | ESP32 GPIO21 |
| SCL | ESP32 GPIO22 |
| RST | 3.3 V |
| ITA | Leave open |
| ITB | Leave open |

If the module has VCC/GND on both PA and PB sides, connect both VCC pins to 3.3 V and both GND pins to common GND.

## MCP23017 To ULN2803A #1: Green LEDs

| MCP23017 pin | ULN2803A #1 input | Button LED |
| --- | --- | --- |
| PA0 | IN1 | Zone 1 green |
| PA1 | IN2 | Zone 2 green |
| PA2 | IN3 | Zone 3 green |
| PA3 | IN4 | Zone 4 green |
| PA4 | IN5 | Zone 5 green |
| PA5 | IN6 | W.N. green |
| PA6 | IN7 | Spare |
| PA7 | IN8 | Spare |

## MCP23017 To ULN2803A #2: Red LEDs

| MCP23017 pin | ULN2803A #2 input | Button LED |
| --- | --- | --- |
| PB0 | IN1 | Zone 1 red/busy |
| PB1 | IN2 | Zone 2 red/busy |
| PB2 | IN3 | Zone 3 red/busy |
| PB3 | IN4 | Zone 4 red/busy |
| PB4 | IN5 | Zone 5 red/busy |
| PB5 | IN6 | Spare |
| PB6 | IN7 | Spare |
| PB7 | IN8 | Spare |

## ULN2803A Wiring

For both ULN2803A chips:

| ULN2803A pin/function | Connects to |
| --- | --- |
| GND | Common GND |
| COM | 12 V LED positive rail, optional for LED-only loads but OK to connect |
| IN1-IN8 | MCP23017 outputs |
| OUT1-OUT8 | LED negative terminals |

The ULN2803A only switches ground. It does not provide 12 V.

## Illuminated Push Button Switch Contacts

Use the NO contact, not NC.

For every button input:

```text
3.3 V -> 10k resistor -> ESP32 input pin
ESP32 input pin -> button NO contact
button COM contact -> GND
```

Expected logic:

| Button state | ESP32 reads |
| --- | --- |
| Not pressed | HIGH |
| Pressed | LOW |

## Illuminated Button LED Contacts

For each 12 V LED color:

```text
12 V positive -> button LED +
button LED -  -> ULN2803A output
ULN2803A input -> MCP23017 output
```

The 12-24 V, 15 mA button LEDs should already have their current limiting built in. Do not add a 10k resistor in series with the LED.

## Quick Test Order

1. Power only ESP32 and MCP23017 at 3.3 V. Confirm no heat.
2. Confirm buttons read HIGH when idle and LOW when pressed.
3. Add ULN2803A chips with common GND.
4. Add one green LED channel first: PA0 -> ULN #1 IN1 -> Zone 1 green.
5. Add one red LED channel first: PB0 -> ULN #2 IN1 -> Zone 1 red.
6. After Zone 1 works, wire the remaining channels.
