*This is out of date*

setup
=====

POWER
-----
* walwart   -> arduino power

bluetooth
---------
* rts-o
* rx-i      -> tx1 18
* tx-o      -> rx1 19
* gnd       -> ground
* vcc       -> 5v/3.3v
* cts-i     -> rts-o (loop back)

LED
---
* LED red (red from 4-lead)             -> battery +
* LED black (blue from 4-lead)          -> battery -
* LED green taped (green from 4-lead)   -> duemilanove: 11 mega: 51
* LED green (yellow from 4-lead)        -> duemilanove: 13 mega: 52

accelerometers
--------------
* vcc/red       -> multiplex shield 5v
* gnd/black     -> multiplex shield ground
* scl/yellow    -> multiplex SCLx - top/outside row 0, 1, 2, 3
* sda/white     -> multiplex SDAx - bottom/inside row 0, 1, 2, 3

microphone
----------
* vcc   -> +3.3v
* gnd   -> Audio IN gnd
* aud   -> Audio IN R
