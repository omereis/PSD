#echo $'gcc -c -g -std=gnu99 -Wall -Werror -I../../api/include -I/opt/redpitaya -DVERSION=0.00-0000 -DREVISION=devbuild rfin.c -o rfin.o'
echo $'gcc -c -g -std=gnu99 -Wall -Werror -I../../api/include -I/opt/redpitaya -DVERSION=0.00-0000 -DREVISION=devbuild fpga.c -o fpga.o'
gcc -c -g -std=gnu99 -Wall -Werror -I../../api/include -I/opt/redpitaya/include -DVERSION=0.00-0000 -DREVISION=devbuild rfin.c -o rfin.o

echo $'gcc -o rfin rfin.o -g -std=gnu99 -Wall -Werror -I../../api/include -DVERSION=0.00-0000 -DREVISION=devbuild -L/opt/redpitaya/lib -lm -lpthread -lrp'
gcc -o rfin rfin.o -g -std=gnu99 -Wall -Werror -I../../api/include -DVERSION=0.00-0000 -DREVISION=devbuild -L/opt/redpitaya/lib -lm -lpthread -lrp
