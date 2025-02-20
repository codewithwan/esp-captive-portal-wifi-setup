#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

const byte DNS_PORT = 53;
DNSServer dnsServer;
WebServer server(80);

String ssid = "";
String password = "";
bool isConnected = false;
bool isAPActive = false;

void setup() {
  Serial.begin(9600);

  // Start in AP mode if no saved credentials or can't connect
  if (!tryConnectSavedWiFi()) {
    startAccessPoint();
  }

  // Setup server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/connect", HTTP_POST, handleConnect);
  server.on("/status", HTTP_GET, handleStatus);
  server.onNotFound([]() {
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
  });

  server.begin();
  Serial.println("Server Started");
}

void loop() {
  if (isAPActive) {
    dnsServer.processNextRequest();
  }
  server.handleClient();
}

bool tryConnectSavedWiFi() {
  // Check if we have saved credentials
  if (ssid.length() > 0 && password.length() > 0) {
    Serial.println("Trying to connect with saved credentials...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());

    // Try to connect for 10 seconds
    int attempts = 20;
    while (attempts > 0 && WiFi.status() != WL_CONNECTED) {
      delay(500);
      attempts--;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Connected to saved WiFi!");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      isConnected = true;
      return true;
    }
  }

  Serial.println("No saved credentials or connection failed");
  return false;
}

void startAccessPoint() {
  if (!isAPActive) {
    WiFi.mode(WIFI_AP);
    WiFi.softAP("@codewithwan_device");

    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
    isAPActive = true;
    isConnected = false;

    Serial.println("Access Point Started: @codewithwan_device");
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
  }
}

void stopAccessPoint() {
  if (isAPActive) {
    dnsServer.stop();
    WiFi.softAPdisconnect(true);
    isAPActive = false;
    Serial.println("Access Point Stopped");
  }
}

void handleRoot() {
  String page = R"rawliteral(
    <!DOCTYPE html>
    <html lang="id">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>WiFi Setup</title>
        <style>
            * {
                margin: 0;
                padding: 0;
                box-sizing: border-box;
                font-family: system-ui, -apple-system, sans-serif;
            }
  
            body {
                min-height: 100vh;
                background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
                display: flex;
                align-items: center;
                justify-content: center;
                padding: 20px;
            }
  
            .container {
                width: 100%;
                max-width: 400px;
                background: rgba(255, 255, 255, 0.95);
                backdrop-filter: blur(10px);
                border-radius: 16px;
                padding: 2rem;
                box-shadow: 0 8px 32px rgba(0, 0, 0, 0.1);
            }
  
            .logo {
                text-align: center;
                margin-bottom: 2rem;
            }
  
            .wifi-icon {
                width: 60px;
                height: 60px;
                background: linear-gradient(45deg, #667eea, #764ba2);
                border-radius: 50%;
                display: flex;
                align-items: center;
                justify-content: center;
                margin: 0 auto;
            }
  
            .wifi-icon svg {
                width: 32px;
                height: 32px;
                fill: white;
            }
  
            h1 {
                color: #1a1a1a;
                font-size: 1.5rem;
                margin-bottom: 0.5rem;
                text-align: center;
            }
  
            .subtitle {
                color: #666;
                text-align: center;
                margin-bottom: 2rem;
                font-size: 0.9rem;
            }
  
            .form-group {
                margin-bottom: 1.5rem;
            }
  
            label {
                display: block;
                margin-bottom: 0.5rem;
                color: #4a5568;
                font-size: 0.9rem;
                font-weight: 500;
            }
  
            input {
                width: 100%;
                padding: 0.75rem 1rem;
                border: 2px solid #e2e8f0;
                border-radius: 8px;
                font-size: 1rem;
                transition: all 0.3s;
                background: white;
            }
  
            input:focus {
                outline: none;
                border-color: #667eea;
                box-shadow: 0 0 0 3px rgba(102, 126, 234, 0.1);
            }
  
            button {
                width: 100%;
                padding: 0.75rem;
                background: linear-gradient(45deg, #667eea, #764ba2);
                color: white;
                border: none;
                border-radius: 8px;
                font-size: 1rem;
                font-weight: 500;
                cursor: pointer;
                transition: all 0.3s;
            }
  
            button:hover {
                transform: translateY(-1px);
                box-shadow: 0 4px 12px rgba(102, 126, 234, 0.2);
            }
  
            button:active {
                transform: translateY(0);
            }
  
            .overlay {
                position: fixed;
                top: 0;
                left: 0;
                right: 0;
                bottom: 0;
                background: rgba(0, 0, 0, 0.7);
                backdrop-filter: blur(5px);
                display: none;
                align-items: center;
                justify-content: center;
                z-index: 1000;
            }
  
            .loader {
                background: white;
                padding: 2rem;
                border-radius: 16px;
                text-align: center;
                box-shadow: 0 8px 32px rgba(0, 0, 0, 0.1);
            }
  
            .spinner {
                width: 40px;
                height: 40px;
                border: 4px solid #f3f3f3;
                border-top: 4px solid #667eea;
                border-radius: 50%;
                margin: 0 auto 1rem;
                animation: spin 1s linear infinite;
            }
  
            .alert {
                position: fixed;
                top: 20px;
                right: 20px;
                padding: 1rem 1.5rem;
                border-radius: 8px;
                color: white;
                font-weight: 500;
                transform: translateX(150%);
                transition: transform 0.3s ease-in-out;
                z-index: 1000;
            }
  
            .alert.success {
                background: #48bb78;
            }
  
            .alert.error {
                background: #f56565;
            }
  
            .alert.show {
                transform: translateX(0);
            }
  
            @keyframes spin {
                0% { transform: rotate(0deg); }
                100% { transform: rotate(360deg); }
            }
  
            @media (max-width: 480px) {
                body {
                    padding: 16px;
                }
                
                .container {
                    padding: 1.5rem;
                }
  
                .wifi-icon {
                    width: 50px;
                    height: 50px;
                }
  
                .wifi-icon svg {
                    width: 28px;
                    height: 28px;
                }
  
                h1 {
                    font-size: 1.25rem;
                }
  
                .subtitle {
                    font-size: 0.8rem;
                }
  
                input {
                    font-size: 0.9rem;
                }
  
                button {
                    font-size: 0.9rem;
                }
            }
        </style>
    </head>
    <body>
        <div class="container">
            <div class="logo">
                <div class="wifi-icon">
                    <svg viewBox="0 0 24 24">
                        <path d="M12 6C8.62 6 5.5 7.12 3 9L1.2 6.6C4.21 4.34 7.95 3 12 3C16.05 3 19.79 4.34 22.8 6.6L21 9C18.5 7.12 15.38 6 12 6M12 15C10.65 15 9.4 15.45 8.4 16.2L12 21L15.6 16.2C14.6 15.45 13.35 15 12 15M12 9C9.3 9 6.81 9.89 4.8 11.4L6.6 13.8C8.1 12.67 9.97 12 12 12C14.03 12 15.9 12.67 17.4 13.8L19.2 11.4C17.19 9.89 14.7 9 12 9Z"/>
                    </svg>
                </div>
                <h1>WiFi Setup</h1>
                <p class="subtitle">Connect your IoT device to your network</p>
            </div>
  
            <form id="wifiForm" onsubmit="submitForm(event)">
                <div class="form-group">
                    <label for="ssid">Network Name (SSID)</label>
                    <input type="text" id="ssid" name="ssid" required>
                </div>
                <div class="form-group">
                    <label for="password">Password</label>
                    <input type="password" id="password" name="password" required>
                </div>
                <button type="submit">Connect</button>
            </form>
        </div>
  
        <div class="overlay" id="overlay">
            <div class="loader">
                <div class="spinner"></div>
                <p>Connecting to network...</p>
            </div>
        </div>
  
        <div class="alert success" id="successAlert">
            Successfully connected to WiFi network!
        </div>
  
        <div class="alert error" id="errorAlert">
            Failed to connect. Please check your credentials.
        </div>
  
        <script>
            let connectionTimeout;
  
            function showAlert(type, duration = 3000) {
                const alert = document.getElementById(`${type}Alert`);
                alert.classList.add('show');
                setTimeout(() => alert.classList.remove('show'), duration);
            }
  
            function submitForm(event) {
                event.preventDefault();
                const overlay = document.getElementById('overlay');
                overlay.style.display = 'flex';
  
                const ssid = document.getElementById('ssid').value;
                const password = document.getElementById('password').value;
  
                fetch('/connect', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                    body: `ssid=${encodeURIComponent(ssid)}&password=${encodeURIComponent(password)}`
                });
  
                connectionTimeout = setTimeout(() => {
                    overlay.style.display = 'none';
                    showAlert('error');
                }, 10000);
  
                checkConnection();
            }
  
            function checkConnection() {
                let attempts = 0;
                const maxAttempts = 20;
                
                const checkStatus = () => {
                    fetch('/status')
                        .then(response => response.text())
                        .then(status => {
                            if (status === 'connected') {
                                clearTimeout(connectionTimeout);
                                document.getElementById('overlay').style.display = 'none';
                                showAlert('success');
                            } else if (attempts < maxAttempts) {
                                attempts++;
                                setTimeout(checkStatus, 500);
                            }
                        })
                        .catch(() => {
                            if (attempts < maxAttempts) {
                                attempts++;
                                setTimeout(checkStatus, 500);
                            }
                        });
                };
  
                checkStatus();
            }
        </script>
    </body>
    </html>
    )rawliteral";

  server.send(200, "text/html", page);
}

void handleConnect() {
  ssid = server.arg("ssid");
  password = server.arg("password");

  if (ssid.length() > 0 && password.length() > 0) {
    // Stop AP before trying to connect
    stopAccessPoint();

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    Serial.printf("Attempting to connect to: %s\n", ssid.c_str());

    // Try to connect for 15 seconds
    int timeout = 30;  // 30 * 500ms = 15 seconds
    while (timeout > 0 && WiFi.status() != WL_CONNECTED) {
      delay(500);
      timeout--;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Successfully connected to WiFi!");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      isConnected = true;
      server.send(200, "text/plain", "connected");
    } else {
      Serial.println("Failed to connect, restarting AP");
      startAccessPoint();
      server.send(200, "text/plain", "failed");
    }
  } else {
    server.send(400, "text/plain", "Invalid credentials");
  }
}

void handleStatus() {
  if (WiFi.status() == WL_CONNECTED) {
    server.send(200, "text/plain", "connected");
  } else {
    // If connection is lost, restart AP
    if (!isAPActive) {
      startAccessPoint();
    }
    server.send(200, "text/plain", "not_connected");
  }
}

void checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected! Reactivating Access Point...");
    startAccessPoint();
  }
}