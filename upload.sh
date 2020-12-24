built_bin=out/main.bin
# python /darbi/pyWorkspace/z80_upload_ram/flash.py /dev/cu.usbserial-AG0JNMSW $built_bin --baud 76800
python /darbi/pyWorkspace/z80_upload_ram/flash.py /dev/cu.usbserial-AG0JNMSW $built_bin --baud 76800 --norun
