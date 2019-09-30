export PATH=$PATH:/home/ish/projects/2015-05-25-mxe/mxe2/mxe/usr/bin:/home/ish/projects/2016-02-29-korrino/implementation-6/local/generators

export PATH=$PATH:/home/ish/projects/2015-05-25-mxe/mxe2/mxe/usr/bin:/home/ish/projects/korrino/local/generators

i686-w64-mingw32.static-g++ -l TitanEngine -L. trace.c
i686-w64-mingw32.static-g++ -l TitanEngine -L. -I ../../2013-09-04-win-headers/msvcpp/inc/ trace2.c
i686-w64-mingw32.static-g++ -I ../../2013-09-04-win-headers/msvcpp/inc/ trace2.c
i686-w64-mingw32.static-g++ -I ../../2013-09-04-win-headers/msvcpp/inc/ trace2.c
i686-w64-mingw32.static-g++ -w -I ../../2013-09-04-win-headers/msvcpp/inc/ trace4.c
i686-w64-mingw32.static-g++ -w -I /home/ish/projects/2013-09-04-win-headers/msvcpp/inc/ trace4.c -o a.exe
i686-w64-mingw32.static-g++ -w -I /home/ish/projects/2013-09-04-win-headers/msvcpp/inc/ trace5.c -o b.exe
i686-w64-mingw32.static-g++ -w -I /home/ish/projects/2013-09-04-win-headers/msvcpp/inc/  -I /home/ish/projects/2015-05-25-mxe/mxe2/mxe/usr/i686-w64-mingw32.shared/include/ trace5.c -o b.exe -L /home/ish/projects/2015-05-25-mxe/mxe2/mxe/usr/i686-w64-mingw32.shared/lib/  -lws2_32
i686-w64-mingw32.static-g++ -w -I /home/ish/projects/2013-09-04-win-headers/msvcpp/inc/  -I /home/ish/projects/2015-05-25-mxe/mxe2/mxe/usr/i686-w64-mingw32.shared/include/ trace6.c -o b.exe -L /home/ish/projects/2015-05-25-mxe/mxe2/mxe/usr/i686-w64-mingw32.shared/lib/  -lws2_32

# latest i686
i686-w64-mingw32.static-g++ -w -I /home/ish/projects/2013-09-04-win-headers/msvcpp/inc/  -I /home/ish/projects/2015-05-25-mxe/mxe2/mxe/usr/i686-w64-mingw32.shared/include/ trace6.c -o b.exe -L /home/ish/projects/2015-05-25-mxe/mxe2/mxe/usr/i686-w64-mingw32.shared/lib/  -lws2_32 -ldbghelp

i686-w64-mingw32.static-g++ -w -I /home/ish/projects/2013-09-04-win-headers/msvcpp/inc/  -I /home/ish/projects/2015-05-25-mxe/mxe2/mxe/usr/i686-w64-mingw32.shared/include/ trace6.c -o b.exe -L /home/ish/projects/2015-05-25-mxe/mxe2/mxe/usr/i686-w64-mingw32.shared/lib/  -lws2_32 -lpsapi -ldbghelp



x86_64-w64-mingw32.static-g++ -w -I /home/ish/projects/2013-09-04-win-headers/msvcpp/inc/  -I /home/ish/projects/2015-05-25-mxe/mxe2/mxe/usr/i686-w64-mingw32.shared/include/ trace6_64.c -o b.exe -L /home/ish/projects/2015-05-25-mxe/mxe2/mxe/usr/i686-w64-mingw32.shared/lib/  -lws2_32

x86_64-w64-mingw32.static-g++ -w -I /home/ish/projects/2013-09-04-win-headers/msvcpp/inc/  -I /home/ish/projects/2015-05-25-mxe/mxe2/mxe/usr/x86_64-w64-mingw32.static/include/ trace6_64.c -o b.exe -L /home/ish/projects/2015-05-25-mxe/mxe2/mxe/usr/x86_64-w64-mingw32.static/lib/  -lws2_32

x86_64-w64-mingw32.static-g++ -w -I /home/ish/projects/2013-09-04-win-headers/msvcpp/inc/  -I /home/ish/projects/2015-05-25-mxe/mxe2/mxe/usr/x86_64-w64-mingw32.static/include/ trace6_64.c -o b.exe -L /home/ish/projects/2015-05-25-mxe/mxe2/mxe/usr/x86_64-w64-mingw32.static/lib/  -lws2_32; strip b.exe

# latest x86_64
x86_64-w64-mingw32.static-g++ -w -I /home/ish/projects/2013-09-04-win-headers/msvcpp/inc/  -I /home/ish/projects/2015-05-25-mxe/mxe2/mxe/usr/x86_64-w64-mingw32.static/include/ trace6_64.c -o c.exe -L /home/ish/projects/2015-05-25-mxe/mxe2/mxe/usr/x86_64-w64-mingw32.static/lib/  -lws2_32
cp b.exe 32/b.exe

# two staged:
x86_64-w64-mingw32.static-g++ -S -w -I /home/ish/projects/2013-09-04-win-headers/msvcpp/inc/  -I /home/ish/projects/2015-05-25-mxe/mxe2/mxe/usr/x86_64-w64-mingw32.static/include/ trace6_64.c -L /home/ish/projects/2015-05-25-mxe/mxe2/mxe/usr/x86_64-w64-mingw32.static/lib/  -lws2_32
x86_64-w64-mingw32.static-as trace6_64.s 

