// SPDX-License-Identifier: MIT
pragma solidity ^0.8.18;

contract HelloWorld {
    string public message;
    constructor() {
        message = "Hello World";
    }
    function setMessage(string memory _message) public {
        message = _message;
    }
    function sayMessage() view public returns(string memory) {
        return message;
    }
}