echo 'g++ -c -g -Wall -Werror -std=c++0x -I../../api/include -I/opt/redpitaya/include -DVERSION=0.00-0000 -DREVISION=devbuild psd.cpp -o psd.o'
g++ -c -g -Wall -Werror -std=c++0x -I../../api/include -I/opt/redpitaya/include -DVERSION=0.00-0000 -DREVISION=devbuild psd.cpp -o psd.o
echo 'g++ -c -g -Wall -Werror -std=c++0x -I../../api/include -I/opt/redpitaya/include -DVERSION=0.00-0000 -DREVISION=devbuild trig_params.cpp -o trig_params.o'
g++ -c -g -Wall -Werror -std=c++0x -I../../api/include -I/opt/redpitaya/include -DVERSION=0.00-0000 -DREVISION=devbuild trig_params.cpp -o trig_params.o
echo 'g++ -c -g -Wall -Werror -std=c++0x -I../../api/include -I/opt/redpitaya/include -DVERSION=0.00-0000 -DREVISION=devbuild sampling_params.cpp -o sampling_params.o'
g++ -c -g -Wall -Werror -std=c++0x -I../../api/include -I/opt/redpitaya/include -DVERSION=0.00-0000 -DREVISION=devbuild sampling_params.cpp -o sampling_params.o
echo 'g++ -c -g -Wall -Werror -std=c++0x -I../../api/include -I/opt/redpitaya/include -DVERSION=0.00-0000 -DREVISION=devbuild psd_params.cpp -o psd_params.o'
g++ -c -g -Wall -Werror -std=c++0x -I../../api/include -I/opt/redpitaya/include -DVERSION=0.00-0000 -DREVISION=devbuild psd_params.cpp -o psd_params.o
echo 'g++ -c -g -Wall -Werror -std=c++0x -I../../api/include -I/opt/redpitaya/include -DVERSION=0.00-0000 -DREVISION=devbuild proj_misc.cpp -o proj_misc.o'
g++ -c -g -Wall -Werror -std=c++0x -I../../api/include -I/opt/redpitaya/include -DVERSION=0.00-0000 -DREVISION=devbuild proj_misc.cpp -o proj_misc.o
echo 'g++ -c -g -Wall -Werror -std=c++0x -I../../api/include -I/opt/redpitaya/include -DVERSION=0.00-0000 -DREVISION=devbuild psd_output.cpp -o psd_output.o'
g++ -c -g -Wall -Werror -std=c++0x -I../../api/include -I/opt/redpitaya/include -DVERSION=0.00-0000 -DREVISION=devbuild psd_output.cpp -o psd_output.o
echo 'g++ -c -g -Wall -Werror -std=c++0x -I../../api/include -I/opt/redpitaya/include -DVERSION=0.00-0000 -DREVISION=devbuild psd_out_params.cpp -o psd_out_params.o'
g++ -c -g -Wall -Werror -std=c++0x -I../../api/include -I/opt/redpitaya/include -DVERSION=0.00-0000 -DREVISION=devbuild psd_out_params.cpp -o psd_out_params.o
echo 'g++ -o psd psd.o trig_params.o sampling_params.o psd_params.o proj_misc.o psd_output.o psd_out_params.o -L/opt/redpitaya/lib -ljsoncpp -lm -lpthread -l:librp.so'
g++ -o psd psd.o trig_params.o sampling_params.o psd_params.o proj_misc.o psd_output.o psd_out_params.o -L/opt/redpitaya/lib -ljsoncpp -lm -lpthread -l:librp.so
