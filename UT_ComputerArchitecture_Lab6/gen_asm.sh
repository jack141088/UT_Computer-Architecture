for i in $(find . -name \*.asm);
do
    ./assembler.linux $i ${i:15:(-4)}
done
