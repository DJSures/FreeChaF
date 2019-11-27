# FreeChaF
This fork is of FreeChaF with changes to use the Fairchild Channel F original controllers and console buttons connected to a raspberry pi via Arduino Micro or Leonardo. Original Repo here: https://github.com/libretro/FreeChaF

## Authors

FreeChaF was created by David Richardson, this fork has changes by DJ Sures

## License
The FreeChaF core is licensed under GPLv3.

## BIOS
FreeChaF requires two BIOS files to be placed in the libretro 'system' folder:

| Filename | Description | MD5 |
|---|---|---|
| sl31253.bin | ChannelF BIOS (PSU 1) | ac9804d4c0e9d07e33472e3726ed15c3 |
| sl31254.bin | ChannelF BIOS (PSU 2) | da98f4bb3242ab80d76629021bb27585 |

If the ChannelF II BIOS is included, it will be used instead of sl31253.  All games are compatible with both.

| Filename | Description | MD5 |
|---|---|---|
| sl90025.bin | ChannelF II BIOS (PSU 1) | 95d339631d867c8f1d15a5f2ec26069d |

* BIOS filenames are case-sensitive

## Console button overlay
Access to the console buttons is provided by the Arduino. The LibRetro configuration should have the assignments of numeric values 1, 2, 3, 4 to buttons A, B, X, Z. 

## Controls
| FreeChaF Function | Retropad |
| --- | --- |
|Forward| Left-Analog Up|
|Backward| Left-Analog Down|
|Rotate Right-Analog Left |
|Rotate Right | ight-Analog Right |
|Pull Up | Right-Analog Up |
|Push Down | Right-Analog Down |
|Show/Hide Console Overlay | Start |
|Controller Swap | Select |
|Reset | Left Trigger, R |
|Console 1 | A |
|Console 2 | B |
|Console 3 | X |
|Console 4 | Z |

