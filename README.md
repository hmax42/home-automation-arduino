# home-automation-arduino
my arduino code used in my home-automation

* mqtt433bridge
  * allows control of these https://www.amazon.de/Brennenstuhl-Funkschalt-Set-Funksteckdosen-Innenbereich-Kindersicherung/dp/B001AX8QUM
  * supports right now these two home-networks 10100 and 10101
  * listens for "/esp/433bridge/1010?/ID"
    * ID is A, B, C, D
  * is uniderectional, does not listen for signals from the remote to update it's status
  * used with a wemos d1 mini and https://www.ebay.de/itm/433-Mhz-Sender-Empfanger-RF-Funk-Modul-FS1000A-xy-mk-5v-Arduino-Raspberry-Pi/252713874815?hash=item3ad6ebb57f:g:7R0AAOSw7D1cP4go
* mqtt_ir_bridge
  * support coding for Samsung, NEC and basically all standards that are supported by the IRremoteESP8266 lib
    * Samsung tested on my TV
    * NEC tested with this https://www.amazon.de/gp/product/B07BQ4JJS6/ref=ppx_yo_dt_b_asin_title_o06_s00?ie=UTF8&psc=1
    * implemented MANUFACTURERs are
      * samsung  
      * nec
      * sony
      * mitsubishi
  * listens for "esp/IRbridge/MANUFACTURER"
  * the transmitted value must be the Hex-code to send
    * to get therse values i use IRrecvDemoOled from the Lib with a separate Arduino Pro Mini with an IR-LDR
  * used with a wemos d1 mini and this https://www.tindie.com/products/analysir/esp8266esp32-trx-advanced-infrared-module-ir/
* espaudio
  * very unpolished for publication
  * requires to select one and only one of the defines to select the used DAC
