import json
import mysql.connector
import asyncio
from web3 import Web3
from solcx import compile_standard, install_solc
import socket
import datetime

contract_path = 'C:/Users/amzie/Downloads/SimpleSmartContract.sol'
api_key = None


def compile_contract(contract_file):
    install_solc("0.8.18")
    compiled_solidity = compile_standard({
        "language": "Solidity",
        "sources": {"SimpleSmartContract.sol": {"content": contract_file}},
        "settings": {"outputSelection": {"*": {"*": ["abi", "metadata", "evm.bytecode", "evm.sourceMap"]}}}
    }, solc_version="0.8.18")
    return compiled_solidity


# Create an asyncio coroutine to handle each client connection
async def handle_client(client_socket, client_address, w3, contract_instance):
    global api_key
    print("Connected to", client_address)
    transaction_index = 1
    while True:
        try:
            data = await loop.sock_recv(client_socket, 1024)
            received_string = data.decode()

            if not data:
                break

            if received_string.startswith("New API key|"):
                provided_api_key = received_string.split("|", 1)[1].strip()  # Extract and clean the provided API key
                api_key = provided_api_key
                client_socket.send(api_key.encode())
                print("API Key Set:", api_key)

            if received_string.startswith("API key|"):
                provided_api_key = received_string.split("|", 1)[1].strip()

                if api_key == provided_api_key:
                    validity = "VALID"
                    client_socket.send(validity.encode())
                    print("API Key:", api_key, "Validity:", validity)

                else:
                    validity = "INVALID"
                    client_socket.send(validity.encode())
                    client_socket.close()  # Close the connection for invalid API keys
                    print("API Key:", api_key, "Validity:", validity)
                    break  # Exit the loop when the API key is invalid

            if received_string.startswith("Transaction|"):
                payload = received_string.split("|", 1)[1]
                print("Received:", payload)

                # Update nonce value for each transaction
                nonce = w3.eth.get_transaction_count(wallet)
                update_message = contract_instance.functions.setMessage(payload).build_transaction(
                    {
                        "gasPrice": w3.eth.gas_price,
                        "chainId": chain_id,
                        "from": wallet,
                        "nonce": nonce
                    }
                )
                transaction_index += 1
                signed_transaction = w3.eth.account.sign_transaction(update_message, private_key)
                transaction_hash = w3.eth.send_raw_transaction(signed_transaction.rawTransaction)
                transaction_receipt = w3.eth.wait_for_transaction_receipt(transaction_hash)

                print(transaction_hash.hex())
                masa = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")

                # Send a response back to the client
                response = "CxID" + payload + "|TxID: " + transaction_hash.hex()
                client_socket.sendall(response.encode())

                # send to database
                query = "INSERT INTO hash (TxID, Payload, Timestamp) VALUES (%s, %s, %s)"
                val = (transaction_hash.hex(), payload, masa)
                cursor.execute(query, val)
                db.commit()

            if received_string.startswith("Tx Hash|"):
                TxHash = received_string.split("|", 1)[1]
                TxID = w3.eth.get_transaction(TxHash)
                input_data = TxID.input

                def decode_input_data(input_data, contract_abi):
                    contract = w3.eth.contract(abi=contract_abi)
                    decoded_input = contract.decode_function_input(input_data)
                    return decoded_input

                decoded_input = decode_input_data(input_data, abi)
                dictionary = decoded_input[1]
                message_value = dictionary['_message']
                start_index = message_value.find("'") + 1

                extracted_part = "Input Data|" + message_value[start_index:]
                client_socket.sendall(extracted_part.encode())
                print("Input Data: " + extracted_part)

        except ConnectionResetError:
            print("Client disconnected")
            break

    client_socket.close()


# Create an asyncio coroutine to start the server
async def start_server(loop, w3, contract_instance):
    server_address = ('127.0.0.1', 12345)
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(server_address)
    server_socket.listen()

    print("Waiting for connections...")

    while True:
        client_socket, client_address = await loop.sock_accept(server_socket)
        asyncio.create_task(handle_client(client_socket, client_address, w3, contract_instance))


# Main function
def build_transaction(w3, SmartContract, chain_id, wallet, nonce):
    transaction = SmartContract.constructor().build_transaction({
        "gasPrice": w3.eth.gas_price,
        "chainId": chain_id,
        "from": wallet,
        "nonce": nonce
    })
    return transaction


if __name__ == "__main__":
    with open(contract_path, 'r') as file:
        contract_file = file.read()

    compiled_solidity = compile_contract(contract_file)

    provider_url = 'https://goerli.infura.io/v3/bacca2a3a6fd42a19cd6f0b674e24fa3'
    w3 = Web3(Web3.HTTPProvider(provider_url))

    abi = compiled_solidity['contracts']['SimpleSmartContract.sol']['HelloWorld']['abi']
    bytecode = compiled_solidity['contracts']['SimpleSmartContract.sol']['HelloWorld']['evm']['bytecode']['object']

    SmartContract = w3.eth.contract(abi=abi, bytecode=bytecode)

    chain_id = 5
    wallet = "0x1c949A949c5d436aceA531b415CE092698406978"
    nonce = w3.eth.get_transaction_count(wallet)

    transaction = build_transaction(w3, SmartContract, chain_id, wallet, nonce)
    private_key = "0x4b7c972bb5ab98050b6f75afb6b8afa382390f6a3893ee3095e2165802cd2cd7"
    signed_transaction = w3.eth.account.sign_transaction(transaction, private_key)
    transaction_hash = w3.eth.send_raw_transaction(signed_transaction.rawTransaction)
    transaction_receipt = w3.eth.wait_for_transaction_receipt(transaction_hash)

    contract_instance = w3.eth.contract(address=transaction_receipt.contractAddress, abi=abi)

    db = mysql.connector.connect(
        host="127.0.0.1",
        user="root",
        password="",
        database="socket"
    )
    cursor = db.cursor()

    loop = asyncio.get_event_loop()
    try:
        loop.run_until_complete(start_server(loop, w3, contract_instance))
    except KeyboardInterrupt:
        pass
    finally:
        loop.close()