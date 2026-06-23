from flask import Flask, render_template, request, jsonify, session
import os, time

app = Flask(__name__)
app.secret_key = "csms_secret_key_2025"

# ── Data files stored relative to app.py ──
BASE_DIR     = os.path.dirname(os.path.abspath(__file__))
USERS_FILE   = os.path.join(BASE_DIR, "data", "users.txt")
LOGS_FILE    = os.path.join(BASE_DIR, "data", "logs.txt")
RECORDS_FILE = os.path.join(BASE_DIR, "data", "filerecords.txt")
FILES_DIR    = os.path.join(BASE_DIR, "data", "files")

# Create directories if they don't exist
os.makedirs(os.path.join(BASE_DIR, "data"), exist_ok=True)
os.makedirs(FILES_DIR, exist_ok=True)

XOR_KEY = '#'

# ════════════════════════════════════════
#  Helpers
# ════════════════════════════════════════

def xor_crypt(text):
    return ''.join(chr(ord(c) ^ ord(XOR_KEY)) for c in text)

def compute_hash(content):
    h = 0
    for i, c in enumerate(content):
        h += ord(c) * (i + 1)
    return format(h & 0xFFFFFFFFFFFFFFFF, 'x')

def write_log(level, event, username=""):
    timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
    labels = {"INFO": "INFO    ", "WARNING": "WARNING ", "CRITICAL": "CRITICAL"}
    label = labels.get(level, "INFO    ")
    user_part = f"<{username}> " if username else ""
    entry = f"[{timestamp}] [{label}] {user_part}{event}\n"
    with open(LOGS_FILE, 'a') as f:
        f.write(entry)

def is_strong_password(pwd):
    if len(pwd) < 8:
        return False
    return any(c.isalpha() for c in pwd) and any(c.isdigit() for c in pwd)

def password_strength(pwd):
    score = 0
    checks = {
        "Length 8+":        len(pwd) >= 8,
        "Length 12+":       len(pwd) >= 12,
        "Uppercase letter": any(c.isupper() for c in pwd),
        "Lowercase letter": any(c.islower() for c in pwd),
        "Number":           any(c.isdigit() for c in pwd),
        "Special char":     any(not c.isalnum() for c in pwd),
    }
    for v in checks.values():
        if v: score += 1
    if score <= 2:   rating = "WEAK"
    elif score <= 4: rating = "MODERATE"
    else:            rating = "STRONG"
    return checks, rating, score

def load_users():
    users = []
    if not os.path.exists(USERS_FILE):
        return users
    with open(USERS_FILE, 'r') as f:
        for line in f:
            parts = line.strip().split('|')
            if len(parts) == 5:
                users.append({
                    "username": parts[0],
                    "encPwd":   parts[1],
                    "isAdmin":  parts[2] == '1',
                    "attempts": int(parts[3]),
                    "isLocked": parts[4] == '1'
                })
    return users

def save_users(users):
    with open(USERS_FILE, 'w') as f:
        for u in users:
            f.write(f"{u['username']}|{u['encPwd']}|{int(u['isAdmin'])}|{u['attempts']}|{int(u['isLocked'])}\n")

def find_user(users, username):
    return next((u for u in users if u['username'] == username), None)

def load_records():
    records = []
    if not os.path.exists(RECORDS_FILE):
        return records
    with open(RECORDS_FILE, 'r') as f:
        for line in f:
            parts = line.strip().split('|')
            if len(parts) == 3:
                records.append({
                    "filename":    parts[0],
                    "storedHash":  parts[1],
                    "isEncrypted": parts[2] == '1'
                })
    return records

def save_records(records):
    with open(RECORDS_FILE, 'w') as f:
        for r in records:
            f.write(f"{r['filename']}|{r['storedHash']}|{int(r['isEncrypted'])}\n")

def find_record(records, filename):
    return next((r for r in records if r['filename'] == filename), None)

# ════════════════════════════════════════
#  Routes
# ════════════════════════════════════════

@app.route("/")
def index():
    return render_template("index.html",
                           user=session.get("user", ""),
                           is_admin=session.get("is_admin", False))

@app.route("/register", methods=["POST"])
def register():
    data = request.json
    username = data.get("username", "").strip()
    password = data.get("password", "").strip()
    is_admin = data.get("is_admin", False)

    if not username or not password:
        return jsonify({"success": False, "message": "Username and password required."})

    users = load_users()
    if find_user(users, username):
        write_log("WARNING", "Registration failed - username taken", username)
        return jsonify({"success": False, "message": f"Username '{username}' already exists."})

    if not is_strong_password(password):
        write_log("WARNING", "Registration failed - weak password", username)
        return jsonify({"success": False, "message": "Weak password! Must be 8+ characters with letters and digits."})

    enc_pwd = xor_crypt(password)
    users.append({"username": username, "encPwd": enc_pwd,
                  "isAdmin": is_admin, "attempts": 0, "isLocked": False})
    save_users(users)
    role = "Admin" if is_admin else "Regular User"
    write_log("INFO", f"New {role} registered", username)
    return jsonify({"success": True, "message": f"User '{username}' registered successfully as {role}."})

@app.route("/login", methods=["POST"])
def login():
    data = request.json
    username = data.get("username", "").strip()
    password = data.get("password", "").strip()

    users = load_users()
    user = find_user(users, username)

    if not user:
        write_log("WARNING", "Login failed - username not found", username)
        return jsonify({"success": False, "message": "Username not found."})

    if user["isLocked"]:
        write_log("CRITICAL", "Login attempt on locked account", username)
        return jsonify({"success": False, "message": "Account is LOCKED after too many failed attempts."})

    if xor_crypt(password) == user["encPwd"]:
        user["attempts"] = 0
        save_users(users)
        session["user"] = username
        session["is_admin"] = user["isAdmin"]
        write_log("INFO", "Login successful", username)
        role = "Admin" if user["isAdmin"] else "Regular User"
        return jsonify({"success": True, "message": f"Welcome, {username}! [{role}]",
                        "is_admin": user["isAdmin"], "user": username})
    else:
        user["attempts"] += 1
        if user["attempts"] >= 3:
            user["isLocked"] = True
            save_users(users)
            write_log("CRITICAL", "Account locked after repeated failures", username)
            return jsonify({"success": False, "message": "Wrong password. Account is now LOCKED."})
        remaining = 3 - user["attempts"]
        save_users(users)
        write_log("WARNING", f"Failed login attempt ({user['attempts']}/3)", username)
        return jsonify({"success": False, "message": f"Wrong password. {remaining} attempt(s) remaining."})

@app.route("/logout", methods=["POST"])
def logout():
    write_log("INFO", "User logged out", session.get("user", ""))
    session.clear()
    return jsonify({"success": True, "message": "Logged out successfully."})

@app.route("/encrypt", methods=["POST"])
def encrypt():
    data     = request.json
    filename = data.get("filename", "").strip()
    content  = data.get("content", "").strip()
    filepath = os.path.join(FILES_DIR, filename)

    records = load_records()
    record  = find_record(records, filename)

    if record and record["isEncrypted"]:
        return jsonify({"success": False, "message": "File is already encrypted."})

    if content:
        with open(filepath, 'w') as f:
            f.write(content)

    if not os.path.exists(filepath):
        return jsonify({"success": False, "message": f"File not found: {filename}"})

    with open(filepath, 'r') as f:
        plain = f.read()

    stored_hash = compute_hash(plain)
    encrypted   = xor_crypt(plain)

    with open(filepath, 'w') as f:
        f.write(encrypted)

    if record:
        record["storedHash"]  = stored_hash
        record["isEncrypted"] = True
    else:
        records.append({"filename": filename, "storedHash": stored_hash, "isEncrypted": True})

    save_records(records)
    write_log("INFO", "File encrypted", filename)
    return jsonify({"success": True, "message": f"File '{filename}' encrypted.\nHash: {stored_hash}"})

@app.route("/decrypt", methods=["POST"])
def decrypt():
    data     = request.json
    filename = data.get("filename", "").strip()
    filepath = os.path.join(FILES_DIR, filename)

    records = load_records()
    record  = find_record(records, filename)

    if not record or not record["isEncrypted"]:
        return jsonify({"success": False, "message": "File is not encrypted or not tracked."})

    if not os.path.exists(filepath):
        return jsonify({"success": False, "message": f"File not found: {filename}"})

    with open(filepath, 'r') as f:
        encrypted = f.read()

    with open(filepath, 'w') as f:
        f.write(xor_crypt(encrypted))

    record["isEncrypted"] = False
    save_records(records)
    write_log("INFO", "File decrypted", filename)
    return jsonify({"success": True, "message": f"File '{filename}' decrypted successfully."})

@app.route("/integrity", methods=["POST"])
def integrity():
    data     = request.json
    filename = data.get("filename", "").strip()
    filepath = os.path.join(FILES_DIR, filename)

    records = load_records()
    record  = find_record(records, filename)

    if not record:
        return jsonify({"success": False, "message": f"No record found for: {filename}"})

    if not os.path.exists(filepath):
        return jsonify({"success": False, "message": f"File not found: {filename}"})

    with open(filepath, 'r') as f:
        content = f.read()

    plain        = xor_crypt(content) if record["isEncrypted"] else content
    current_hash = compute_hash(plain)

    if current_hash == record["storedHash"]:
        write_log("INFO", "Integrity check passed", filename)
        return jsonify({"success": True, "message": f"✓ Integrity check PASSED — file is unchanged.\nHash: {current_hash}"})
    else:
        write_log("CRITICAL", "Integrity check FAILED", filename)
        return jsonify({"success": False, "message": f"✗ Integrity check FAILED — file may have been tampered!\nExpected: {record['storedHash']}\nFound:    {current_hash}"})

@app.route("/list_files", methods=["GET"])
def list_files():
    records = load_records()
    if not records:
        return jsonify({"message": "No files tracked yet.", "files": []})
    msg = ""
    for r in records:
        msg += f"File: {r['filename']}\nEncrypted: {'Yes' if r['isEncrypted'] else 'No'}\nHash: {r['storedHash']}\n\n"
    return jsonify({"message": msg.strip(), "files": records})

@app.route("/view_file", methods=["POST"])
def view_file():
    data     = request.json
    filename = data.get("filename", "").strip()
    filepath = os.path.join(FILES_DIR, filename)

    records = load_records()
    record  = find_record(records, filename)

    if not os.path.exists(filepath):
        return jsonify({"message": f"File not found: {filename}"})

    with open(filepath, 'r') as f:
        content = f.read()

    display = xor_crypt(content) if (record and record["isEncrypted"]) else content
    write_log("INFO", "File viewed", filename)
    return jsonify({"message": display})

@app.route("/edit_file", methods=["POST"])
def edit_file():
    data        = request.json
    filename    = data.get("filename", "").strip()
    new_content = data.get("content", "").strip()
    filepath    = os.path.join(FILES_DIR, filename)

    records = load_records()
    record  = find_record(records, filename)

    if not os.path.exists(filepath):
        return jsonify({"success": False, "message": f"File not found: {filename}"})

    if record and record["isEncrypted"]:
        with open(filepath, 'w') as f:
            f.write(xor_crypt(new_content))
        record["storedHash"] = compute_hash(new_content)
        save_records(records)
        write_log("INFO", "File edited and re-encrypted", filename)
        return jsonify({"success": True, "message": f"File '{filename}' updated and re-encrypted."})
    else:
        with open(filepath, 'w') as f:
            f.write(new_content)
        if record:
            record["storedHash"] = compute_hash(new_content)
            save_records(records)
        write_log("INFO", "File edited", filename)
        return jsonify({"success": True, "message": f"File '{filename}' updated successfully."})

@app.route("/check_password", methods=["POST"])
def check_password():
    data     = request.json
    password = data.get("password", "")
    checks, rating, score = password_strength(password)
    msg = "── Password Strength Report ──\n"
    for label, passed in checks.items():
        msg += f"{'✓' if passed else '✗'}  {label}\n"
    msg += f"\nRating: {rating}"
    return jsonify({"message": msg, "rating": rating, "score": score})

@app.route("/get_users", methods=["GET"])
def get_users():
    users = load_users()
    if not users:
        return jsonify({"message": "No users registered yet."})
    msg = "── Registered Users ──\n"
    for u in users:
        role   = "Admin" if u["isAdmin"] else "Regular User"
        locked = "Yes" if u["isLocked"] else "No"
        msg += f"[{role}] {u['username']}  |  Locked: {locked}\n"
    return jsonify({"message": msg})

@app.route("/get_logs", methods=["GET"])
def get_logs():
    if not os.path.exists(LOGS_FILE):
        return jsonify({"message": "No logs found."})
    with open(LOGS_FILE, 'r') as f:
        content = f.read()
    return jsonify({"message": content if content else "Log file is empty."})

if __name__ == "__main__":
    app.run(debug=False, host='0.0.0.0', port=5000)