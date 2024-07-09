There are two types of swing, vertical and horizontal swings. Since codes are divided between them, and there's no way to send a combination of both swings (you have to send a code for each type of swing level individually), we will divide them here. 

**ALL** swings have the same prefix:
`FIXED` `FIXED` `cmd` `cmd2` `temp` `fan_speed_1` `fan_speed_2`
1000     1000    0001 0011 xxxx    xxxx                  xxxx

When specifying, we will only show code for the remaining fields.

# Vertical Swing

There are 8 possible values for these,  inciting 8 different codes. These codes are segmented in two types: single type swing and special swing

## Special Swing
These codes are reserved to two situations: when you want repeated swing (from top to bottom ad infinitum) and when you want to disable swing. For these, `temp` is fixed as `0001`, and`fan_speed_1` and  `fan_speed_2` change.


| **Type**        | code           |
| --------------- | -------------- |
| no swing        | 0001 0100 1001 |
| repeating swing | 0001 0101 1010 |

Node that `fan_speed_1` and `fan_speed_2` are incremented from the first to second row.

## Fixed swing

Fix the swing table in a single position, ranging from 0 to 5, with 0 being lowest and 6 being highest.  For these, `temp` is fixed at `0000`, `fan_speed 1/2` start with a pattern and are individually incremented in 1 as the position increases from 0 to 6:

For brevity, `temp` will be skipped in the table:

| **level** | code      |
| --------- | --------- |
| 0         | 0100 1000 |
| 1         | 0101 1001 |
| 2         | 0110 1010 |
| 3         | 0111 1011 |
| 4         | 1000 1100 |
| 5         | 1001 1101 |
