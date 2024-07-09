The protocol seems proprietary to LG controllers and close to the X-sat mitsubishi protocol. The idea is that every signal starts with a header composed of a 3.3 ms pulse and 9.9 ms space, followed by the signal, which is composed of 28 pulse/space pairs, where every pair starts with a 500 us pulse and the space defines the binary bit. If space is 500 us, it is a `logical 0`. If the space is composed of 1500 us, it is a `logical 1`.

![[protocol_summary.png]]

# Binary codes
Codes are always composed of 7 columns of 4 bytes, separated as:

`FIXED` `FIXED`      `cmd` `cmd_2` `temp` `fan_speed_1` `fan_speed_2`
XXXX       XXXX     XXXX XXXX     XXXX    XXXX                 XXXX

## Fixed
`FIXED` is always fixed as `1000`
## cmd/fixed commands

cmd varies based on command. Some commands are fixed and `cmd` is one way to tell what command may follow. Note that multiple commands have the same `cmd`, so we still need to use it to decode the entire command.

| command                | cmd  | full code                              |
| ---------------------- | ---- | -------------------------------------- |
| turn off               | 1100 | 1000 1000 1100 0000 0000 0101 0001     |
| turn on/common command | 0000 | 1000 1000 0000 xxxx xxxx xxxx xxxx     |
| comfort sleep on       | 1100 | 1000 1000 1100 0000 1000 1111 0011     |
| comfort sleep off      | 1010 | 1000 1000 1010 0000 0000 0000 1010     |
| energy saving on       | 0001 | 1000 1000 0001 0000 0000 0100 0101     |
| energy saving off      | 0001 | 1000 1000 0001 0000 0000 0101 0110<br> |
| jet mode               | 0001 | 1000 1000 0001 0000 0000 1000 1001     |
| timer                  | 1010 | [[timers]]                             |
| swing vertical         | 0001 | [[swing codes]]                        |
| swing horizontal       | 0001 | [[swing codes]]                        |

## cmd2

Based on the current mode:

| **mode**   | **cmd2** |
| ---------- | -------- |
| cold       | 1000     |
| hot        | 1100     |
| fan        | 1010     |
| auto       | 1011     |
| dehumidify | 1001     |

**MSB is 0 if turning on, 1 if changing mode/temp while still on.**
## temp

temp is the binary of `t - 15`, `t` being the desired temperature.
As such:

| temperature | temp |
| ----------- | ---- |
| 16          | 0001 |
| 17          | 0010 |
| 18          | 0011 |
| 19          | 0100 |
| 20          | 0101 |
| 21          | 0110 |
| 22          | 0111 |
| 23          | 1000 |
| 24          | 1001 |
| 25          | 1010 |
| 26          | 1011 |
| 27          | 1100 |
| 28          | 1101 |
| 29          | 1110 |
| 30          | 1111 |

Note that modes *dehumidify* and *fan* have no temperature, so the values are fixed:

| **mode**     | **temp** |
| ------------ | -------- |
| *dehumidify* | 1001     |
| *fan*        | 0011     |

## fan_speed_1

there are 5 fan speeds, from 0 to 4 and auto. Codes are fixed as follows:

| **fan speed** | **code** |
| ------------- | -------- |
| 0             | 0000     |
| 1             | 1001     |
| 2             | 0010     |
| 3             | 1010     |
| 4             | 0100     |
| auto          | 0101     |

## **fan_speed_2**

This one is calculated from a formula with the following pattern:

`base + type + T + IS_TURNING_ON`

### `base`
base is a fixed value following the same pattern as the 5 fan speeds, but with diffeSrent values:

| **fan speed** | **code** |
| ------------- | -------- |
| 0             | 1011     |
| 1             | 0100     |
| 2             | 1101     |
| 3             | 0101     |
| 4             | 1111     |
| auto          | 0000     |
### `type`

Type is  a fixed value for each type in the air-conditioner.

| **type**<br> | **value** |
| ------------ | --------- |
| cold         | 0000      |
| hot          | 0010      |
| fan          | 0010      |
| auto         | 0011      |
| dehumidify   | 0111      |
### `T`

`T` is calculated from the temperature:

`T = temperature - b_t`

`bt` is the base temperature for the current  mode:

| **type**   | **b_t** |
| ---------- | ------- |
| hot        | 16      |
| cold       | 18      |
| auto       | 18<br>  |
| dehumidify | *N/A*   |
| fan        | *N/A*   |
for *dehumidify*  and *fan*, `T` is **always** 0.

### `is_turning_on`

this value is `1000` if we are turning on the A/C, `0` otherwise.
