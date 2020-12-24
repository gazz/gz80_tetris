mkdir out
sdasz80 -plosgff crt0.s
mv crt0.rel crt0.lst crt0.sym out 
sdcc main.c out/crt0.rel -o out/ -mz80  --no-std-crt0 --code-loc 0x2200 --data-loc 0 || { echo 'compile failed' ; exit 1; }
hex2bin out/main.ihx
xxd out/main.bin
