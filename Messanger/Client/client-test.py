import requests

SERVER_URL = 'http://192.168.2.22:5000/messages'  # change to your server IP

def send_message():
    sender = input("Enter your name (sender): ").strip()
    recipient = input("Enter recipient name: ").strip()
    message = input("Enter your message: ").strip()

    data = {'sender': sender, 'recipient': recipient, 'message': message}
    response = requests.post(SERVER_URL, data=data)
    if response.status_code == 201:
        print("Message sent successfully.")
    else:
        print("Failed to send message:", response.text)

def receive_messages():
    username = input("Enter your name to fetch messages: ").strip().lower()
    response = requests.get(SERVER_URL)
    if response.status_code == 200:
        messages = response.json()
        # Filter messages case-insensitively by lowering both sides
        user_msgs = [
            m for m in messages 
            if m['sender'].lower() == username or m['recipient'].lower() == username
        ]
        if user_msgs:
            for msg in user_msgs:
                print(f"{msg['timestamp']} {msg['sender']} -> {msg['recipient']}: {msg['message']}")
        else:
            print("No messages found for user", username)
    else:
        print("Failed to get messages")


def main():
    while True:
        choice = input("Choose (1) Send Message, (2) Receive Messages, (q) Quit: ").strip()
        if choice == '1':
            send_message()
        elif choice == '2':
            receive_messages()
        elif choice.lower() == 'q':
            print("Goodbye!")
            break
        else:
            print("Invalid choice, try again.")

if __name__ == "__main__":
    main()
