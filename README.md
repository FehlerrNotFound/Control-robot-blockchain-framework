# Control-robot-blockchain-framework
Framework for Cloud-Based Robot Navigation Control with Blockchain Technology

This is a project for my master's thesis to control a cloud-connected robot with blockchain as it's cybersecurity. The project uses TCP websocket to send a payload from the user to the blockchain processor. The user controls the navigation of the robot from a UI developed using Xojo software. The command sent by the user to the robot is sent to the internet through API. The command is then read by the robot using web API and then move according to the code programmed using Arduino IDE. 

At the moment the user sent command for the robot to move, the command is turn into a payload to be sent to a blockchain processor by websocket. The server side of the websocket is coded to be an asynchronous server using Python. The payload is then kept inside a MySQL database and sent to Ethereum blockchain server to be used as a transaction by making use of smart contract in Ethereum. Ethereum will then return a transaction hash to be kept inside the database and also be displayed to the user as a completed transaction.

Software used:
1. Python
2. Xojo
3. Arduino
4. Solidity
5. Web API
