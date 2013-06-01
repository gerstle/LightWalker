setup
=====

POWER
-----
* walwart   -> arduino power

bluetooth
---------
* vcc       -> 5v
* gnd       -> ground
* cts-i     -> rts-o
* tx-o      -> arduino digital 10
* rx-i      -> arduino digital 9 

LED
---
* LED red           -> battery +
* LED black         -> battery -
* LED green taped   -> 11
* LED green         -> 13

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
