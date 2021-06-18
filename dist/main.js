/*
 * ATTENTION: The "eval" devtool has been used (maybe by default in mode: "development").
 * This devtool is neither made for production nor for readable output files.
 * It uses "eval()" calls to create a separate source file in the browser devtools.
 * If you are trying to read the output file, select a different devtool (https://webpack.js.org/configuration/devtool/)
 * or disable the default devtool with "devtool: false".
 * If you are looking for production-ready output files, see mode: "production" (https://webpack.js.org/configuration/mode/).
 */
/******/ (() => { // webpackBootstrap
/******/ 	var __webpack_modules__ = ({

/***/ "./src/index.js":
/*!**********************!*\
  !*** ./src/index.js ***!
  \**********************/
/***/ (() => {

eval("window.WebSocket = window.WebSocket || window.MozWebSocket;\nvar websocket = new WebSocket('ws://127.0.0.1:9000', 'lws-minimal');\nwebsocket.onopen = function () {\n    $('h1').css('color', 'green');\n    var request = constructPost(\"/new-user\", `${username} joined`);\n    \n    websocket.send(request);\n}\n\nwebsocket.onerror = function () {\n    $('h1').css('color', 'red');\n};\nwebsocket.onmessage = function (message) {\n    console.log(message.data);\n    appendMessage(message.data);\n};\n\nvar username; \n\n$(window).load(function (){\n    username = prompt(\"What is your username?\");\n    document.getElementById('sendMessage').onclick = function (event) {\n        event.preventDefault(); \n        var input = document.getElementsByTagName(\"INPUT\")[0];\n        console.log(input.value);\n        appendMessage(`You: ${input.value}`);\n        var request = constructPost(\"/message\", `${username}: ${input.value}`)\n        websocket.send(request);\n        input.value = '';\n    }\n})\n\nfunction constructPost(path, content){\n    var postRequest = \"POST \" + path + \" HTTP/1.1\"\n    var contentType = \"Content-Type: text-plain\"\n    var contentLen = \"Content-Length: \" + content.length.toString();\n\n    return postRequest + \"\\n\" + contentType + \"\\n\" + contentLen + \"\\n\\n\" + content + \"\\n\";\n}\n\nfunction appendMessage(message){\n    var messageDiv = document.getElementsByTagName(\"div\")[0];\n    var messageContent = document.createElement(\"p\");\n    messageContent.innerHTML = message;\n    messageDiv.append(messageContent);\n}\n\n//# sourceURL=webpack://chat-app/./src/index.js?");

/***/ })

/******/ 	});
/************************************************************************/
/******/ 	
/******/ 	// startup
/******/ 	// Load entry module and return exports
/******/ 	// This entry module can't be inlined because the eval devtool is used.
/******/ 	var __webpack_exports__ = {};
/******/ 	__webpack_modules__["./src/index.js"]();
/******/ 	
/******/ })()
;