import sqlite3

def create_table():
    conn = sqlite3.connect('messages.db')  # Your SQLite database file
    cursor = conn.cursor()

    # Create the messages table if it does not exist already
    cursor.execute('''
        CREATE TABLE IF NOT EXISTS messages (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            sender TEXT NOT NULL,
            recipient TEXT NOT NULL,
            message TEXT NOT NULL,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    ''')

    conn.commit()
    conn.close()
    print("Table 'messages' created or already exists.")

if __name__ == '__main__':
    create_table()
