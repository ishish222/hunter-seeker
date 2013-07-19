function listen-port($port)
{
	$enc = new-object system.text.asciiEncoding
	$buffer = new-object system.byte[] 1024
	$endpoint = new-object system.net.ipendpoint([system.net.ipaddress]::any, $port)
	$listener = new-object system.net.sockets.tcplistener($endpoint)
	$listener.start()
	$client = new-object system.net.sockets.tcpclient
	$client = $listener.AcceptTcpClient()
	"connected"
#	$ns = system.net.sockets.networkstream
	$ns = $client.getstream()
	while($true)
	{
		$i = $ns.read($buffer, 0, $buffer.length)
		$output = $enc.getstring($buffer, 0, $i-1)
		$output
		if($output -eq "quit")
		{
			break
		}
	}
#	$ns
	$listener.stop()
}

"test"
listen-port(12345)