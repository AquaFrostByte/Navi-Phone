from flask import Flask, request, jsonify
import sqlite3

app = Flask(__name__)

def get_db_connection():
    conn = sqlite3.connect('messages.db')
    conn.row_factory = sqlite3.Row
    return conn

@app.route('/messages', methods=['GET', 'POST'])
def messages():
    if request.method == 'POST':
        sender = request.form['sender']
        recipient = request.form['recipient']
        message = request.form['message']
        conn = get_db_connection()
        conn.execute('INSERT INTO messages (sender, recipient, message) VALUES (?, ?, ?)',
                     (sender, recipient, message))
        conn.commit()
        conn.close()
        return 'Message stored!', 201

    if request.method == 'GET':
        conn = get_db_connection()
        messages = conn.execute('SELECT * FROM messages ORDER BY timestamp ASC').fetchall()
        conn.close()
        messages_list = [dict(msg) for msg in messages]
        return jsonify(messages_list)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
