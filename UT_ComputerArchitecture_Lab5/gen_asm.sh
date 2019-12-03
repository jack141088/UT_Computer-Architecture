for i in $(find -name \*.asm);
do
    ./assembler.linux $i ${i:2:(-4)}
done
