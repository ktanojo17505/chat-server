window.WebSocket = window.WebSocket || window.MozWebSocket;
var websocket = new WebSocket('ws://127.0.0.1:9000', 'lws-minimal');
websocket.onopen = function () {
    $('h1').css('color', 'green');
    var request = constructPost("/new-user", `${username} joined`);
    
    websocket.send(request);
}

websocket.onerror = function () {
    $('h1').css('color', 'red');
};
websocket.onmessage = function (message) {
    console.log(message.data);
    appendMessage(message.data);
};

var username; 

$(window).load(function (){
    username = prompt("What is your username?");
    document.getElementById('sendMessage').onclick = function (event) {
        event.preventDefault(); 
        var input = document.getElementsByTagName("INPUT")[0];
        console.log(input.value);
        appendMessage(`You: ${input.value}`);
        var request = constructPost("/message", `${username}: ${input.value}`)
        websocket.send(request);
        input.value = '';
    }
})

function constructPost(path, content){
    var postRequest = "POST " + path + " HTTP/1.1"
    var contentType = "Content-Type: text-plain"
    var contentLen = "Content-Length: " + content.length.toString();

    return postRequest + "\n" + contentType + "\n" + contentLen + "\n\n" + content + "\n";
}

function appendMessage(message){
    var messageDiv = document.getElementsByTagName("div")[0];
    var messageContent = document.createElement("p");
    messageContent.innerHTML = message;
    messageDiv.append(messageContent);
}