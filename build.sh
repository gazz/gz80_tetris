mkdir out
sdasz80 -plosgff crt0.s || { echo 'crt0 assembly failed' ; exit 1; }
mv crt0.rel crt0.lst crt0.sym out 
sdcc vga.c -c -o out/ -mz80 || { echo 'vga.c compile failed' ; exit 1; }
sdcc main.c out/crt0.rel out/vga.rel -o out/ -mz80  --no-std-crt0 --code-loc 0x2200 --data-loc 0 || { echo 'main.c compile failed' ; exit 1; }
hex2bin out/main.ihx
xxd out/main.bin
./upload.sh
