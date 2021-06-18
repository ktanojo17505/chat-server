import net from 'net';
var client = new net.Socket(); 

client.connect(8080, '127.0.0.1', function() {
    console.log('Connected')
    client.write("Hello, server! Love, Client.")
})

client.on('data', function(data) {
    console.log('Recieved: ' + data);
})

export function addUser(username){
    console.log(username)
    client.write(username + " " + client.remoteAddress + " " + client.remotePort)
}