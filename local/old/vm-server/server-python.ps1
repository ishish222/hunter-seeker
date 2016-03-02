$version = "ver. 0.5"

$global:pipe = $null
$global:pipeName = "\\.\control"
$global:pipeName2 = "\\.\pipe\control"
$global:pipeName3 = "control"
$global:pipeStream = $null
$global:ns = $null
$global:nsReader = $null
$global:nsWriter = $null
$global:lastProc = $null
$global:spawnDelay = 10
$global:binnerPath = "W:\paimei\binner-opera-1.py"

$enc = new-object system.text.asciiEncoding

function dn4on()
{
	reg add hklm\software\microsoft\.netframework /v OnlyUseLatestCLR /t REG_DWORD /d 1

}

function dn4off()
{
	reg delete hklm\software\microsoft\.netframework /v OnlyUseLatestCLR /f
}

function inject($mypid)
{
	& ".\p021-instrumentation-server.exe" $mypid
}

function set-pipe-server($name)
{
	write-socket "setting pipe server"
	$pipedir = [system.io.pipes.pipedirection]::inout
	$maxinstances = 5
	$pipemode = [system.io.pipes.pipetransmissionmode]::message
	$pipeoptions = [system.io.pipes.pipeoptions]::asynchronous
	$global:pipeStream = new-object system.io.pipes.namedpipeserverstream($name, $pipedir, $maxinstances, $pipemode, $pipeoptions)
	$global:pipeStream.waitforconnection()
	$global:pipeStream
	write-socket "got connection"
}

function read-pipe($count)
{
	$i = $global:pipeStream.read($recv, 0, $count)
	$ans = $enc.getstring($recv, 0, $i)
	return $ans
}

function write-pipe($string)
{
	$bytes = $enc.getbytes($string)
	$global:pipeStream.write($bytes, 0, $bytes.length)
	$global:pipeStream.flush()
}

function read-socket($count)
{
	$i = $global:ns.read($buffer, 0, $buffer.length)
	$ans = $enc.getstring($buffer, 0, $i-0)
	return $ans
}


function write-socket($string)
{
#	$bytes = $enc.getbytes($string)
#	$global:ns.write($bytes, 0, $bytes.length)
	$global:nsWriter.write($string)
	$global:nsWriter.Flush()
}

function get-pipe-stream()
{
	write-socket "Pojebalo cie"
#	$global:pipe = new-object system.io.pipes.namedpipeclientstream($pipeName)
#	$global:pipe.connect()
#	$global:pipeStream = new-object system.io.bufferedstream($pipe)
}

function get-socket-streams()
{
	$global:nsReader = new-object system.io.streamreader($global:ns)
	$global:nsWriter = new-object system.io.streamwriter($global:ns)
}

function kill-pid($pidd)
{
    stop-process $pidd
}

function kill-name($name)
{
    stop-process -processname $name
}

function kill-explorer()
{
    taskkill /F /IM explorer.exe
}

function dispatch-command([string]$command, [system.net.sockets.tcpclient]$client)
{
	$recv = new-object system.byte[] 128
	$cmdarray = $command.split(" ")

	if($cmdarray[0] -eq "ps")
	{
		$a = get-process | out-string
		write-socket $a
	}

	if($cmdarray[0] -eq "dn4off")
	{
        dn4off
        write-socket "DN4OFF"
	}

	if($cmdarray[0] -eq "dn4on")
	{
        dn4on
        write-socket "DN4ON"
	}

	if($cmdarray[0] -eq "inject")
	{
		& ".\p021-instrumentation-server.exe" $cmdarray[1]
		set-pipe-server $global:pipeName3
		write-socket "Thread injected"
	}

	if($cmdarray[0] -eq "testFile")
	{
		write-pipe "waitTest"
		$ans = read-pipe 100
		write-socket $ans
		Invoke-item $cmdarray[1]
		$ans = read-pipe 100
		write-socket $ans
	}

	if($cmdarray[0] -eq "invoke")
	{
		Invoke-item $cmdarray[1]
		write-socket "Invoked"
	}

	if($cmdarray[0] -eq "testmode")
	{
		if($cmdarray[1] -eq "enter")
		{
			write-socket "Entering testmode"
			while($true)
			{
				$file = read-socket
				
				if($file -eq "testmode exit")
				{
					break
				}

				write-pipe "waitTest"
				$ans = read-pipe 100
#				write-socket $ans

				Invoke-item $file
				$ans = read-pipe 100
#				write-socket $ans

				write-pipe "KillClass"
				$ans = read-pipe 100
				write-socket $ans
			}
			write-socket "Exiting test mode"
		}
	}

	if($cmdarray[0] -eq "checkpipe")
	{
		if(test-path $global:pipeName2)
		{
			write-socket "Pipe exists"
		}
		else
		{
			write-socket "Pipe doesn't exist"
		}
	}

	if($cmdarray[0] -eq "getpipe")
	{
		get-pipe-stream
		write-socket "Assumed control"
	}

	if($cmdarray[0] -eq "killExplorer")
	{
        kill-explorer
		write-socket "Explorer's dead"
	}

	if($cmdarray[0] -eq "killLast")
	{
		kill-pid $global:lastProc.id
		write-socket "Last app is dead"
	}

	if($cmdarray[0] -eq "injectLast")
	{
		$id = $global:lastProc.id
		inject $id
		set-pipe-server $global:PipeName3
		write-socket "Thread injected"
	}

	if($cmdarray[0] -eq "startBinner")
	{
        "Starting binner"
        Invoke-item $global:binnerPath
		set-pipe-server $global:pipeName3
        read-pipe 100
        write-socket "Binner started"
    }

	if($cmdarray[0] -eq "spawn")
	{
		$rest = ""
		for($i = 1; $i -le $cmdarray.count; $i++)
		{
			$rest += $cmdarray[$i]
			$rest += " "
		}

		dn4off
		$global:lastProc = [Diagnostics.Process]::Start($rest)
		start-sleep -seconds $global:spawnDelay
		dn4on
		write-socket $global:lastProc.Id
		write-socket "OK"
	}


	if($cmdarray[0] -eq "pipe")
	{
		$rest = ""
		for($i = 1; $i -le $cmdarray.count; $i++)
		{
			$rest += $cmdarray[$i]
			$rest += " "
		}

		$rest = $rest.substring(0, $rest.length)

		write-pipe $rest

		do
		{	
			$ans = read-pipe 100
			write-socket $ans
		}
		while($ans -ne "OK")
	}

	if($cmdarray[0] -eq "testPipe")
	{
		write-pipe "testPipe"

		$ans = read-pipe 100
		write-socket $ans

		$ans = read-pipe 100
		write-socket $ans
	}
}

function listen-port($port)
{
	$enc = new-object system.text.asciiEncoding
	$buffer = new-object system.byte[] 1024
	$endpoint = new-object system.net.ipendpoint([system.net.ipaddress]::any, $port)
	$listener = new-object system.net.sockets.tcplistener($endpoint)
	$listener.start()
	$client = new-object system.net.sockets.tcpclient
	$client = $listener.AcceptTcpClient()
	$global:ns = $client.getstream()

	get-socket-streams
	"connected"

	write-socket $version

	while($true)
	{
		$command = read-socket
		if($command -eq "quit")
		{
			break
		}
		dispatch-command $command $client
	}
	$listener.stop()
}

function acad-preparations()
{
    reg add hkcu\software\autodesk\mc3 /v NotificationRemindOn /t REG_DWORD /d 0 /f
    reg add "hkcu\software\autodesk\AutoCAD\R18.0\ACAD-8001:409\Profiles\Initial Setup Profile\General" /v RecoveryMode /t REG_DWORD /d 0 /f
}

[reflection.Assembly]::loadwithpartialname("system.core")

acad-preparations

"got it, strating server"
listen-port(12345)

"finishing"
$bytes = $enc.getbytes("quit")
$pipeStream.write($bytes, 0, $bytes.length)
$pipeStream.flush()