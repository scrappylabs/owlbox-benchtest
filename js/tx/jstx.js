var raw = require ("raw-socket");

sentPkt = 0;
recvPkt = 0;

var options = {
    addressFamily: raw.AddressFamily.IPv4,
    protocol: raw.Protocol.None,
    bufferSize: 4096,
    generateChecksums: false,
    checksumOffset: 0
};

var socket = raw.createSocket (options);
console.log("Socket Created!");

socket.on ("error", function (error) {
    //console.log (error.toString ());
    socket.close ();
});


//socket.on ("message", function (buffer, source) {
	//console.log(buffer.toString().substr(14)) //Don't change this number.
	//recvPkt++;
    //console.log ("received " + buffer.length + " bytes from " + source);
//});


var buffer = Buffer.from("Hello, Jordan!", "utf-8");
function sendPkt(){
socket.send (buffer, 0, buffer.length, "1.1.1.1", function (error, bytes) {
		sentPkt++;
		console.log("TX: "+sentPkt);
});

}

process.on('SIGINT', function() {
    console.log("Done!");
	socket.close ();
	console.log("Sent Pkt: " + sentPkt);
    process.exit();
});

//console.log("Enter speed");
//speed = readline();
a=setInterval(sendPkt,(0.001));
console.log("Sending...");
