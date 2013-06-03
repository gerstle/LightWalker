setup
=====

POWER
-----
* walwart   -> arduino power

bluetooth
---------
* vcc       -> 5v/3.3v
* gnd       -> ground
* cts-i     -> rts-o (loop back)
* tx-o      -> rx1 19
* rx-i      -> tx1 18

LED
---
* LED red           -> battery +
* LED black         -> battery -
* LED green taped   -> duemilanove: 11 mega: 51
* LED green         -> duemilanove: 13 mega: 52

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
