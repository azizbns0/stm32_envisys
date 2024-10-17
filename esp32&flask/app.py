from flask import Flask, request, jsonify, render_template_string

app = Flask(__name__)


latest_temp = None

@app.route('/')
def home():
    return "Flask server is running. Use /data endpoint to POST data."

@app.route('/data', methods=['POST'])
def handle_data():
    global latest_temp  # Using a global variable to store the latest temperature
    temp = request.form.get('Temp')
    
    if temp:
        latest_temp = temp  # Store the latest temperature
        print(f'Received temperature: {temp}')
        return jsonify({"message": "Data received", "temperature": temp}), 200
    return jsonify({"error": "No data received"}), 400

@app.route('/show-data')
def show_data():
    return render_template_string("""
        <html>
        <head>
            <title>Latest Temperature Data</title>
            <script>
                function fetchData() {
                    fetch('/get-data')
                        .then(response => response.json())
                        .then(data => {
                            document.getElementById('temp-value').textContent = data.latest_temp || 'No data yet';
                        });
                }
                setInterval(fetchData, 1000); // Update every 1 second
            </script>
        </head>
        <body>
            <h1>Latest Temperature from ESP32</h1>
            <p>Current Temperature: <span id="temp-value">No data yet</span></p>
        </body>
        </html>
    """)

@app.route('/get-data')
def get_data():
    # Return the latest temperature as JSON
    return jsonify({"latest_temp": latest_temp})

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)  # Adjust port as necessary


