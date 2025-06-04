# Fake omapi service written in cpp

For running SecureElement service (omapi) without java environment.  
Providing a method to decrypt in native environment like TWRP

Only implemented **part** of original secure element app.

How to use:
-----------
- Clone this repo to aosp source tree, eg. $TWRP_ROOT/vendor/twrp
- Define your KM uuid properly
- lunch target device
- run `make android.se.omapi.fake` 
 
Then, the binary will be found in out/target/product/$DEVICE/vendor 
 
- Copy binary file, needed dependencies, manifest file and rc file to your TWRP device tree
- Build twrp
- Enjoy
