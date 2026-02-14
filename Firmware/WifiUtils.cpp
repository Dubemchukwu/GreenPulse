#include "Networks.h"
#include "Utils.h"
#include "Config.h"
#include <Preferences.h>
#include <ESPmDNS.h>

// Initialize preferences
Preferences preferences;

// Initialize webserver
WebServer server(80);

// Initialize Wifi Variables
String ssid = "";
String password = "";

// Initialize ESP Restart State
bool restartESPWEB = false;

// Initialize ESP Restart Timer
unsigned long lastESPWEBTime = 0;

// Initialize permanent storage
void initializePermanentStorage(){
    preferences.begin("greenpulse", false);
    // Wifi Credentials
    ssid = preferences.getString("ssid", SSID1);
    password = preferences.getString("password", PASSWORD1);
    preferences.end();
}

const char WIFI_SUCCESS_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>GreenPulse - Connection Successful</title>
    <link href="https://fonts.googleapis.com/css2?family=Inter:wght@400;500;600;700&display=swap" rel="stylesheet">
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        body {
            font-family: 'Inter', -apple-system, BlinkMacSystemFont, sans-serif;
            min-height: 100vh;
            display: flex;
            align-items: center;
            justify-content: center;
            background: linear-gradient(135deg, #1a4d2e 0%, #0f2919 50%, #1a4d2e 100%);
            padding: 20px;
            overflow: hidden;
            position: relative;
        }

        /* Animated background elements */
        body::before {
            content: '';
            position: absolute;
            width: 800px;
            height: 800px;
            background: radial-gradient(circle, rgba(46, 204, 113, 0.15) 0%, transparent 70%);
            top: -300px;
            right: -300px;
            border-radius: 50%;
            animation: pulse 4s ease-in-out infinite;
        }

        body::after {
            content: '';
            position: absolute;
            width: 600px;
            height: 600px;
            background: radial-gradient(circle, rgba(46, 204, 113, 0.1) 0%, transparent 70%);
            bottom: -200px;
            left: -200px;
            border-radius: 50%;
            animation: pulse 3s ease-in-out infinite reverse;
        }

        @keyframes pulse {
            0%, 100% {
                transform: scale(1);
                opacity: 0.6;
            }
            50% {
                transform: scale(1.15);
                opacity: 0.4;
            }
        }

        @keyframes fadeIn {
            from {
                opacity: 0;
                transform: translateY(30px);
            }
            to {
                opacity: 1;
                transform: translateY(0);
            }
        }

        @keyframes scaleIn {
            from {
                opacity: 0;
                transform: scale(0.8);
            }
            to {
                opacity: 1;
                transform: scale(1);
            }
        }

        @keyframes checkmark {
            0% {
                stroke-dashoffset: 100;
            }
            100% {
                stroke-dashoffset: 0;
            }
        }

        @keyframes circle {
            0% {
                stroke-dashoffset: 300;
            }
            100% {
                stroke-dashoffset: 0;
            }
        }

        @keyframes progressBar {
            from {
                width: 0%;
            }
            to {
                width: 100%;
            }
        }

        .container {
            position: relative;
            z-index: 1;
            width: 100%;
            max-width: 560px;
        }

        .card {
            background: rgba(30, 70, 50, 0.35);
            backdrop-filter: blur(40px);
            -webkit-backdrop-filter: blur(40px);
            border-radius: 32px;
            border: 1px solid rgba(46, 204, 113, 0.15);
            padding: 60px 50px;
            box-shadow: 0 20px 60px rgba(0, 0, 0, 0.5),
                        0 0 0 1px rgba(46, 204, 113, 0.1) inset;
            text-align: center;
            animation: fadeIn 0.6s ease-out;
        }

        .success-icon-wrapper {
            width: 120px;
            height: 120px;
            margin: 0 auto 32px;
            position: relative;
            animation: scaleIn 0.6s cubic-bezier(0.68, -0.55, 0.265, 1.55) 0.2s both;
        }

        .success-circle {
            width: 120px;
            height: 120px;
            border-radius: 50%;
            background: linear-gradient(135deg, #2ecc71 0%, #27ae60 100%);
            display: flex;
            align-items: center;
            justify-content: center;
            box-shadow: 0 8px 32px rgba(46, 204, 113, 0.4);
            position: relative;
            overflow: hidden;
        }

        .success-circle::before {
            content: '';
            position: absolute;
            width: 200%;
            height: 200%;
            background: conic-gradient(transparent, rgba(255, 255, 255, 0.3), transparent 30%);
            animation: rotate 3s linear infinite;
        }

        @keyframes rotate {
            from {
                transform: rotate(0deg);
            }
            to {
                transform: rotate(360deg);
            }
        }

        .checkmark {
            position: relative;
            z-index: 1;
        }

        .checkmark svg {
            width: 60px;
            height: 60px;
        }

        .checkmark-circle {
            stroke-dasharray: 300;
            stroke-dashoffset: 300;
            animation: circle 0.6s ease-in-out 0.4s forwards;
        }

        .checkmark-check {
            stroke-dasharray: 100;
            stroke-dashoffset: 100;
            animation: checkmark 0.4s ease-in-out 0.8s forwards;
        }

        .title {
            font-size: 32px;
            font-weight: 700;
            color: #ffffff;
            margin-bottom: 12px;
            animation: fadeIn 0.6s ease-out 0.4s both;
        }

        .subtitle {
            font-size: 16px;
            color: rgba(255, 255, 255, 0.7);
            margin-bottom: 32px;
            line-height: 1.6;
            animation: fadeIn 0.6s ease-out 0.5s both;
        }

        .status-box {
            background: rgba(20, 50, 35, 0.5);
            border: 1px solid rgba(46, 204, 113, 0.2);
            border-radius: 16px;
            padding: 24px;
            margin-bottom: 32px;
            animation: fadeIn 0.6s ease-out 0.6s both;
        }

        .status-item {
            display: flex;
            align-items: center;
            justify-content: space-between;
            padding: 12px 0;
            border-bottom: 1px solid rgba(255, 255, 255, 0.05);
        }

        .status-item:last-child {
            border-bottom: none;
        }

        .status-label {
            font-size: 14px;
            color: rgba(255, 255, 255, 0.6);
            font-weight: 500;
        }

        .status-value {
            font-size: 14px;
            color: #2ecc71;
            font-weight: 600;
            display: flex;
            align-items: center;
            gap: 6px;
        }

        .status-icon {
            width: 16px;
            height: 16px;
            fill: #2ecc71;
        }

        .progress-section {
            margin-bottom: 32px;
            animation: fadeIn 0.6s ease-out 0.7s both;
        }

        .progress-label {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 12px;
        }

        .progress-text {
            font-size: 13px;
            color: rgba(255, 255, 255, 0.6);
            font-weight: 500;
        }

        .progress-percent {
            font-size: 13px;
            color: #2ecc71;
            font-weight: 600;
        }

        .progress-bar-bg {
            width: 100%;
            height: 8px;
            background: rgba(20, 50, 35, 0.5);
            border-radius: 10px;
            overflow: hidden;
            position: relative;
        }

        .progress-bar-fill {
            height: 100%;
            background: linear-gradient(90deg, #2ecc71 0%, #27ae60 100%);
            border-radius: 10px;
            animation: progressBar 2s ease-out 1s forwards;
            width: 0%;
            position: relative;
            overflow: hidden;
        }

        .progress-bar-fill::after {
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            bottom: 0;
            right: 0;
            background: linear-gradient(90deg, transparent, rgba(255, 255, 255, 0.3), transparent);
            animation: shimmer 1.5s infinite;
        }

        @keyframes shimmer {
            0% {
                transform: translateX(-100%);
            }
            100% {
                transform: translateX(100%);
            }
        }

        .info-text {
            font-size: 14px;
            color: rgba(255, 255, 255, 0.5);
            margin-bottom: 24px;
            line-height: 1.6;
            animation: fadeIn 0.6s ease-out 0.8s both;
        }

        .action-buttons {
            display: flex;
            gap: 12px;
            animation: fadeIn 0.6s ease-out 0.9s both;
        }

        .btn {
            flex: 1;
            padding: 16px 24px;
            border-radius: 12px;
            font-size: 15px;
            font-weight: 600;
            cursor: pointer;
            transition: all 0.3s ease;
            border: none;
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 8px;
        }

        .btn-primary {
            background: linear-gradient(135deg, #2ecc71 0%, #27ae60 100%);
            color: white;
            box-shadow: 0 4px 16px rgba(46, 204, 113, 0.3);
        }

        .btn-primary:hover {
            transform: translateY(-2px);
            box-shadow: 0 6px 20px rgba(46, 204, 113, 0.4);
        }

        .btn-secondary {
            background: rgba(255, 255, 255, 0.08);
            color: rgba(255, 255, 255, 0.8);
            border: 1px solid rgba(255, 255, 255, 0.1);
        }

        .btn-secondary:hover {
            background: rgba(255, 255, 255, 0.12);
            border-color: rgba(255, 255, 255, 0.2);
        }

        .btn svg {
            width: 18px;
            height: 18px;
            fill: currentColor;
        }

        .footer-note {
            margin-top: 24px;
            font-size: 12px;
            color: rgba(255, 255, 255, 0.4);
            animation: fadeIn 0.6s ease-out 1s both;
        }

        /* Mobile styles */
        @media (max-width: 640px) {
            .card {
                padding: 40px 32px;
            }

            .title {
                font-size: 28px;
            }

            .subtitle {
                font-size: 15px;
            }

            .success-icon-wrapper {
                width: 100px;
                height: 100px;
            }

            .success-circle {
                width: 100px;
                height: 100px;
            }

            .checkmark svg {
                width: 50px;
                height: 50px;
            }

            .action-buttons {
                flex-direction: column;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="card">
            <div class="success-icon-wrapper">
                <div class="success-circle">
                    <div class="checkmark">
                        <svg viewBox="0 0 52 52">
                            <circle class="checkmark-circle" cx="26" cy="26" r="25" fill="none" stroke="white" stroke-width="3"/>
                            <path class="checkmark-check" fill="none" stroke="white" stroke-width="4" stroke-linecap="round" d="M14 27l7.5 7.5L38 18"/>
                        </svg>
                    </div>
                </div>
            </div>

            <h1 class="title">Connection Successful!</h1>
            <p class="subtitle">Your GreenPulse device is now online and ready to monitor your plants.</p>

            <div class="status-box">
                <div class="status-item">
                    <span class="status-label">Network</span>
                    <span class="status-value">
                        <svg class="status-icon" viewBox="0 0 24 24">
                            <path d="M1 9l2 2c4.97-4.97 13.03-4.97 18 0l2-2C16.93 2.93 7.08 2.93 1 9zm8 8l3 3 3-3c-1.65-1.66-4.34-1.66-6 0zm-4-4l2 2c2.76-2.76 7.24-2.76 10 0l2-2C15.14 9.14 8.87 9.14 5 13z"/>
                        </svg>
                        <span id="networkName">HomeNetwork</span>
                    </span>
                </div>
                <div class="status-item">
                    <span class="status-label">Signal Strength</span>
                    <span class="status-value">
                        <svg class="status-icon" viewBox="0 0 24 24">
                            <path d="M12.01 21.49L23.64 7c-.45-.34-4.93-4-11.64-4C5.28 3 .81 6.66.36 7l11.63 14.49.01.01.01-.01z"/>
                        </svg>
                        Excellent
                    </span>
                </div>
                <div class="status-item">
                    <span class="status-label">Device Status</span>
                    <span class="status-value">
                        <svg class="status-icon" viewBox="0 0 24 24">
                            <path d="M12 2C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zm-2 15l-5-5 1.41-1.41L10 14.17l7.59-7.59L19 8l-9 9z"/>
                        </svg>
                        Active
                    </span>
                </div>
            </div>

            <div class="progress-section">
                <div class="progress-label">
                    <span class="progress-text">Initializing sensors...</span>
                    <span class="progress-percent" id="progressPercent">0%</span>
                </div>
                <div class="progress-bar-bg">
                    <div class="progress-bar-fill"></div>
                </div>
            </div>

            <p class="info-text">
                Your device will restart in a few seconds. You can now access the GreenPulse dashboard to monitor your plants in real-time.
            </p>

            <div class="action-buttons">
                <button class="btn btn-primary" onclick="openDashboard()">
                    <svg viewBox="0 0 24 24">
                        <path d="M3 13h8V3H3v10zm0 8h8v-6H3v6zm10 0h8V11h-8v10zm0-18v6h8V3h-8z"/>
                    </svg>
                    Open Dashboard
                </button>
                <button class="btn btn-secondary" onclick="goBack()">
                    <svg viewBox="0 0 24 24">
                        <path d="M20 11H7.83l5.59-5.59L12 4l-8 8 8 8 1.41-1.41L7.83 13H20v-2z"/>
                    </svg>
                    Go Back
                </button>
            </div>

            <p class="footer-note">
                Need help? Check the documentation or contact support.
            </p>
        </div>
    </div>

    <script>
        // Get network name from URL parameter
        const urlParams = new URLSearchParams(window.location.search);
        const networkName = urlParams.get('network') || 'Your Network';
        document.getElementById('networkName').textContent = networkName;

        // Animate progress bar percentage
        let progress = 0;
        const progressInterval = setInterval(() => {
            progress += 2;
            if (progress <= 100) {
                document.getElementById('progressPercent').textContent = progress + '%';
            } else {
                clearInterval(progressInterval);
            }
        }, 40);

        
        setTimeout(() => {
            // Redirect to home page
            window.location.href = `/`;
        }, 3000);
        
    </script>
</body>
</html>
)rawliteral";

const char WIFI_SETUP_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>GreenPulse - Network Setup</title>
    <link href="https://fonts.googleapis.com/css2?family=Inter:wght@400;500;600;700&display=swap" rel="stylesheet">
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        body {
            font-family: 'Inter', -apple-system, BlinkMacSystemFont, sans-serif;
            min-height: 100vh;
            display: flex;
            align-items: center;
            justify-content: center;
            background: linear-gradient(135deg, #1a4d2e 0%, #0f2919 50%, #1a4d2e 100%);
            padding: 20px;
            overflow: hidden;
            position: relative;
        }

        /* Background ambient effects */
        body::before {
            content: '';
            position: absolute;
            width: 800px;
            height: 800px;
            background: radial-gradient(circle, rgba(46, 204, 113, 0.1) 0%, transparent 70%);
            top: -300px;
            right: -300px;
            border-radius: 50%;
        }

        body::after {
            content: '';
            position: absolute;
            width: 600px;
            height: 600px;
            background: radial-gradient(circle, rgba(46, 204, 113, 0.08) 0%, transparent 70%);
            bottom: -200px;
            left: -200px;
            border-radius: 50%;
        }

        .container {
            position: relative;
            z-index: 1;
            width: 100%;
            max-width: 920px;
        }

        .card {
            background: rgba(30, 70, 50, 0.35);
            backdrop-filter: blur(40px);
            -webkit-backdrop-filter: blur(40px);
            border-radius: 32px;
            border: 1px solid rgba(46, 204, 113, 0.15);
            padding: 0;
            box-shadow: 0 20px 60px rgba(0, 0, 0, 0.5),
                        0 0 0 1px rgba(46, 204, 113, 0.1) inset;
            display: grid;
            grid-template-columns: 1fr 1fr;
            overflow: hidden;
            min-height: 520px;
        }

        .left-section {
            padding: 60px 50px;
            display: flex;
            flex-direction: column;
            justify-content: center;
        }

        .logo {
            width: 80px;
            height: 80px;
            background: linear-gradient(135deg, #2ecc71 0%, #27ae60 100%);
            border-radius: 20px;
            display: flex;
            align-items: center;
            justify-content: center;
            margin-bottom: 32px;
            box-shadow: 0 8px 24px rgba(46, 204, 113, 0.3);
        }

        .logo svg {
            width: 44px;
            height: 44px;
            fill: white;
        }

        .brand-title {
            font-size: 48px;
            font-weight: 700;
            color: #ffffff;
            margin-bottom: 8px;
            letter-spacing: -0.5px;
        }

        .brand-subtitle {
            font-size: 18px;
            font-weight: 500;
            color: rgba(255, 255, 255, 0.7);
            margin-bottom: 24px;
        }

        .brand-description {
            font-size: 15px;
            line-height: 1.7;
            color: rgba(255, 255, 255, 0.6);
            font-weight: 400;
        }

        .right-section {
            padding: 60px 50px;
            display: flex;
            flex-direction: column;
            justify-content: center;
            background: rgba(20, 50, 35, 0.3);
        }

        .form-header {
            margin-bottom: 32px;
        }

        .form-title {
            font-size: 28px;
            font-weight: 700;
            color: #ffffff;
            margin-bottom: 8px;
        }

        .form-subtitle {
            font-size: 14px;
            color: rgba(255, 255, 255, 0.6);
            font-weight: 400;
        }

        .form-group {
            margin-bottom: 20px;
            position: relative;
        }

        .form-label {
            display: block;
            font-size: 11px;
            font-weight: 600;
            color: rgba(255, 255, 255, 0.5);
            margin-bottom: 10px;
            letter-spacing: 1px;
            text-transform: uppercase;
        }

        .input-wrapper {
            position: relative;
        }

        .form-input {
            width: 100%;
            padding: 16px 20px;
            background: rgba(255, 255, 255, 0.95);
            border: none;
            border-radius: 12px;
            font-size: 15px;
            color: #1a1a1a;
            font-family: 'Inter', sans-serif;
            font-weight: 500;
            transition: all 0.3s ease;
            outline: none;
        }

        .form-input.has-toggle {
            padding-right: 50px;
        }

        .form-input::placeholder {
            color: rgba(0, 0, 0, 0.3);
            font-weight: 400;
        }

        .form-input:focus {
            background: #ffffff;
            box-shadow: 0 0 0 3px rgba(46, 204, 113, 0.2);
            transform: translateY(-1px);
        }

        .toggle-password {
            position: absolute;
            right: 16px;
            top: 50%;
            transform: translateY(-50%);
            background: none;
            border: none;
            cursor: pointer;
            padding: 4px;
            display: flex;
            align-items: center;
            justify-content: center;
            opacity: 0.5;
            transition: opacity 0.3s ease;
        }

        .toggle-password:hover {
            opacity: 0.8;
        }

        .toggle-password svg {
            width: 20px;
            height: 20px;
            fill: #1a1a1a;
        }

        .connect-button {
            width: 100%;
            padding: 18px;
            background: linear-gradient(135deg, #2ecc71 0%, #27ae60 100%);
            border: none;
            border-radius: 12px;
            font-size: 16px;
            font-weight: 600;
            color: white;
            cursor: pointer;
            transition: all 0.3s ease;
            margin-top: 12px;
            box-shadow: 0 8px 20px rgba(46, 204, 113, 0.3);
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 10px;
        }

        .connect-button:hover {
            transform: translateY(-2px);
            box-shadow: 0 12px 28px rgba(46, 204, 113, 0.4);
        }

        .connect-button:active {
            transform: translateY(0);
        }

        .connect-button:disabled {
            opacity: 0.6;
            cursor: not-allowed;
            transform: none;
        }

        .arrow-icon {
            width: 20px;
            height: 20px;
            fill: white;
            transition: transform 0.3s ease;
        }

        .connect-button:hover .arrow-icon {
            transform: translateX(4px);
        }

        .spinner {
            width: 20px;
            height: 20px;
            border: 2px solid rgba(255, 255, 255, 0.3);
            border-top-color: white;
            border-radius: 50%;
            animation: spin 0.6s linear infinite;
        }

        @keyframes spin {
            to { transform: rotate(360deg); }
        }

        .bottom-icons {
            display: flex;
            gap: 12px;
            margin-top: 28px;
            justify-content: center;
        }

        .icon-button {
            width: 40px;
            height: 40px;
            background: rgba(255, 255, 255, 0.08);
            border: 1px solid rgba(255, 255, 255, 0.1);
            border-radius: 10px;
            display: flex;
            align-items: center;
            justify-content: center;
            cursor: pointer;
            transition: all 0.3s ease;
        }

        .icon-button:hover {
            background: rgba(255, 255, 255, 0.12);
            border-color: rgba(255, 255, 255, 0.2);
        }

        .icon-button svg {
            width: 18px;
            height: 18px;
            fill: rgba(255, 255, 255, 0.5);
        }

        .feedback {
            margin-top: 16px;
            padding: 14px 18px;
            border-radius: 10px;
            font-size: 14px;
            font-weight: 500;
            display: flex;
            align-items: center;
            gap: 10px;
            opacity: 0;
            transform: translateY(-10px);
            transition: all 0.3s ease;
        }

        .feedback.show {
            opacity: 1;
            transform: translateY(0);
        }

        .feedback.success {
            background: rgba(46, 204, 113, 0.15);
            border: 1px solid rgba(46, 204, 113, 0.3);
            color: #2ecc71;
        }

        .feedback.error {
            background: rgba(231, 76, 60, 0.15);
            border: 1px solid rgba(231, 76, 60, 0.3);
            color: #e74c3c;
        }

        .feedback svg {
            width: 18px;
            height: 18px;
            flex-shrink: 0;
        }

        /* Mobile styles */
        @media (max-width: 768px) {
            .card {
                grid-template-columns: 1fr;
                max-width: 440px;
                margin: 0 auto;
            }

            .left-section {
                padding: 40px 32px 32px;
                text-align: center;
                align-items: center;
            }

            .logo {
                margin: 0 auto 24px;
            }

            .brand-title {
                font-size: 36px;
            }

            .brand-subtitle {
                font-size: 16px;
            }

            .brand-description {
                font-size: 14px;
            }

            .right-section {
                padding: 32px;
            }

            .form-title {
                font-size: 24px;
            }
        }

        @media (max-width: 480px) {
            body {
                padding: 16px;
            }

            .left-section {
                padding: 32px 24px 24px;
            }

            .right-section {
                padding: 24px;
            }

            .brand-title {
                font-size: 32px;
            }

            .logo {
                width: 72px;
                height: 72px;
            }

            .logo svg {
                width: 40px;
                height: 40px;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="card">
            <!-- Left Section -->
            <div class="left-section">
                <div class="logo">
                    <svg viewBox="0 0 24 24">
                        <path d="M17,8C8,10,5.9,16.17,3.82,21.34L5.71,22L6.66,19.7C7.14,19.87,7.64,20,8,20C19,20,22,3,22,3C21,5,14,5.25,9,6.25C4,7.25,2,11.5,2,13.5C2,15.5,3.75,17.25,3.75,17.25C7,8,17,8,17,8Z"/>
                    </svg>
                </div>
                <h1 class="brand-title">GreenPulse</h1>
                <p class="brand-subtitle">Smart Garden Ecosystem</p>
                <p class="brand-description">
                    Seamlessly connect your botanical sensors.<br>
                    Monitor moisture, light, and health from anywhere<br>
                    in the world.
                </p>
            </div>

            <!-- Right Section -->
            <div class="right-section">
                <div class="form-header">
                    <h2 class="form-title">Connect Device</h2>
                    <p class="form-subtitle">Configure your WiFi settings below.</p>
                </div>

                <form id="wifiForm">
                    <div class="form-group">
                        <label class="form-label" for="ssid">WiFi Name</label>
                        <div class="input-wrapper">
                            <input 
                                type="text" 
                                id="ssid" 
                                name="ssid" 
                                class="form-input" 
                                placeholder=""
                                required
                            >
                        </div>
                    </div>

                    <div class="form-group">
                        <label class="form-label" for="password">Password</label>
                        <div class="input-wrapper">
                            <input 
                                type="password" 
                                id="password" 
                                name="password" 
                                class="form-input has-toggle" 
                                placeholder=""
                                required
                            >
                            <button type="button" class="toggle-password" id="togglePassword" title="Show password">
                                <svg id="eyeIcon" viewBox="0 0 24 24">
                                    <path d="M12 4.5C7 4.5 2.73 7.61 1 12c1.73 4.39 6 7.5 11 7.5s9.27-3.11 11-7.5c-1.73-4.39-6-7.5-11-7.5zM12 17c-2.76 0-5-2.24-5-5s2.24-5 5-5 5 2.24 5 5-2.24 5-5 5zm0-8c-1.66 0-3 1.34-3 3s1.34 3 3 3 3-1.34 3-3-1.34-3-3-3z"/>
                                </svg>
                            </button>
                        </div>
                    </div>

                    <button type="submit" class="connect-button" id="connectBtn">
                        <span id="btnText">Connect</span>
                        <svg class="arrow-icon" id="arrowIcon" viewBox="0 0 24 24">
                            <path d="M12 4l-1.41 1.41L16.17 11H4v2h12.17l-5.58 5.59L12 20l8-8z"/>
                        </svg>
                    </button>

                    <div id="feedback" class="feedback"></div>

                    <div class="bottom-icons">
                        <div class="icon-button" title="Settings">
                            <svg viewBox="0 0 24 24">
                                <path d="M19.14,12.94c0.04-0.3,0.06-0.61,0.06-0.94c0-0.32-0.02-0.64-0.07-0.94l2.03-1.58c0.18-0.14,0.23-0.41,0.12-0.61 l-1.92-3.32c-0.12-0.22-0.37-0.29-0.59-0.22l-2.39,0.96c-0.5-0.38-1.03-0.7-1.62-0.94L14.4,2.81c-0.04-0.24-0.24-0.41-0.48-0.41 h-3.84c-0.24,0-0.43,0.17-0.47,0.41L9.25,5.35C8.66,5.59,8.12,5.92,7.63,6.29L5.24,5.33c-0.22-0.08-0.47,0-0.59,0.22L2.74,8.87 C2.62,9.08,2.66,9.34,2.86,9.48l2.03,1.58C4.84,11.36,4.8,11.69,4.8,12s0.02,0.64,0.07,0.94l-2.03,1.58 c-0.18,0.14-0.23,0.41-0.12,0.61l1.92,3.32c0.12,0.22,0.37,0.29,0.59,0.22l2.39-0.96c0.5,0.38,1.03,0.7,1.62,0.94l0.36,2.54 c0.05,0.24,0.24,0.41,0.48,0.41h3.84c0.24,0,0.44-0.17,0.47-0.41l0.36-2.54c0.59-0.24,1.13-0.56,1.62-0.94l2.39,0.96 c0.22,0.08,0.47,0,0.59-0.22l1.92-3.32c0.12-0.22,0.07-0.47-0.12-0.61L19.14,12.94z M12,15.6c-1.98,0-3.6-1.62-3.6-3.6 s1.62-3.6,3.6-3.6s3.6,1.62,3.6,3.6S13.98,15.6,12,15.6z"/>
                            </svg>
                        </div>
                        <div class="icon-button" title="User">
                            <svg viewBox="0 0 24 24">
                                <path d="M12 12c2.21 0 4-1.79 4-4s-1.79-4-4-4-4 1.79-4 4 1.79 4 4 4zm0 2c-2.67 0-8 1.34-8 4v2h16v-2c0-2.66-5.33-4-8-4z"/>
                            </svg>
                        </div>
                        <div class="icon-button" title="Info">
                            <svg viewBox="0 0 24 24">
                                <path d="M12 2C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zm1 15h-2v-6h2v6zm0-8h-2V7h2v2z"/>
                            </svg>
                        </div>
                    </div>
                </form>
            </div>
        </div>
    </div>

    <script>
        const form = document.getElementById('wifiForm');
        const connectBtn = document.getElementById('connectBtn');
        const btnText = document.getElementById('btnText');
        const arrowIcon = document.getElementById('arrowIcon');
        const feedback = document.getElementById('feedback');
        const ssidInput = document.getElementById('ssid');
        const passwordInput = document.getElementById('password');
        const togglePasswordBtn = document.getElementById('togglePassword');
        const eyeIcon = document.getElementById('eyeIcon');

        // Toggle password visibility
        togglePasswordBtn.addEventListener('click', () => {
            const type = passwordInput.type === 'password' ? 'text' : 'password';
            passwordInput.type = type;
            
            if (type === 'text') {
                // Eye with slash (hide password)
                eyeIcon.innerHTML = '<path d="M12 7c2.76 0 5 2.24 5 5 0 .65-.13 1.26-.36 1.83l2.92 2.92c1.51-1.26 2.7-2.89 3.43-4.75-1.73-4.39-6-7.5-11-7.5-1.4 0-2.74.25-3.98.7l2.16 2.16C10.74 7.13 11.35 7 12 7zM2 4.27l2.28 2.28.46.46C3.08 8.3 1.78 10.02 1 12c1.73 4.39 6 7.5 11 7.5 1.55 0 3.03-.3 4.38-.84l.42.42L19.73 22 21 20.73 3.27 3 2 4.27zM7.53 9.8l1.55 1.55c-.05.21-.08.43-.08.65 0 1.66 1.34 3 3 3 .22 0 .44-.03.65-.08l1.55 1.55c-.67.33-1.41.53-2.2.53-2.76 0-5-2.24-5-5 0-.79.2-1.53.53-2.2zm4.31-.78l3.15 3.15.02-.16c0-1.66-1.34-3-3-3l-.17.01z"/>';
            } else {
                // Normal eye (show password)
                eyeIcon.innerHTML = '<path d="M12 4.5C7 4.5 2.73 7.61 1 12c1.73 4.39 6 7.5 11 7.5s9.27-3.11 11-7.5c-1.73-4.39-6-7.5-11-7.5zM12 17c-2.76 0-5-2.24-5-5s2.24-5 5-5 5 2.24 5 5-2.24 5-5 5zm0-8c-1.66 0-3 1.34-3 3s1.34 3 3 3 3-1.34 3-3-1.34-3-3-3z"/>';
            }
        });

        function showFeedback(message, type) {
            feedback.className = `feedback ${type} show`;
            feedback.innerHTML = `
                <svg viewBox="0 0 24 24" fill="currentColor">
                    ${type === 'success' 
                        ? '<path d="M12 2C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zm-2 15l-5-5 1.41-1.41L10 14.17l7.59-7.59L19 8l-9 9z"/>'
                        : '<path d="M12 2C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zm1 15h-2v-2h2v2zm0-4h-2V7h2v6z"/>'
                    }
                </svg>
                <span>${message}</span>
            `;
        }

        function hideFeedback() {
            feedback.classList.remove('show');
        }

        form.addEventListener('submit', async (e) => {
            e.preventDefault();
            
            const ssid = ssidInput.value.trim();
            const password = passwordInput.value.trim();

            // Validation
            if (!ssid) {
                showFeedback('Please enter a WiFi name', 'error');
                return;
            }

            if (!password) {
                showFeedback('Please enter a password', 'error');
                return;
            }

            hideFeedback();
            connectBtn.disabled = true;
            btnText.textContent = 'Connecting...';
            arrowIcon.outerHTML = '<div class="spinner"></div>';

            // Send to ESP32
            try {
                const response = await fetch('/processWifi', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/x-www-form-urlencoded',
                    },
                    body: `ssid=${encodeURIComponent(ssid)}&password=${encodeURIComponent(password)}`
                });

                if (response.ok) {
                    btnText.textContent = 'Connected';
                    document.querySelector('.spinner').outerHTML = `
                        <svg class="arrow-icon" viewBox="0 0 24 24">
                            <path d="M9 16.17L4.83 12l-1.42 1.41L9 19 21 7l-1.41-1.41z"/>
                        </svg>
                    `;
                    showFeedback(`Successfully connected to "${ssid}"! Device is online.`, 'success');
        
                    setTimeout(() => {
                        // Redirect to success page with network name as parameter
                        window.location.href = `/success?network=${encodeURIComponent(ssid)}`;
                    }, 500);
                    
                    // Reset form after 3 seconds
                    setTimeout(() => {
                        ssidInput.value = '';
                        passwordInput.value = '';
                        btnText.textContent = 'Connect';
                        hideFeedback();
                        document.querySelector('.arrow-icon').outerHTML = `
                            <svg class="arrow-icon" id="arrowIcon" viewBox="0 0 24 24">
                                <path d="M12 4l-1.41 1.41L16.17 11H4v2h12.17l-5.58 5.59L12 20l8-8z"/>
                            </svg>
                        `;
                    }, 3000);
                
                } else {
                    throw new Error('Connection failed');
                }
            } catch (error) {
                btnText.textContent = 'Connect';
                const spinner = document.querySelector('.spinner');
                if (spinner) {
                    spinner.outerHTML = `
                        <svg class="arrow-icon" id="arrowIcon" viewBox="0 0 24 24">
                            <path d="M12 4l-1.41 1.41L16.17 11H4v2h12.17l-5.58 5.59L12 20l8-8z"/>
                        </svg>
                    `;
                }
                showFeedback('Connection failed. Please check credentials and try again.', 'error');
            } finally {
                connectBtn.disabled = false;
            }
        });

        // Clear error feedback on input
        [ssidInput, passwordInput].forEach(input => {
            input.addEventListener('input', () => {
                if (feedback.classList.contains('error')) {
                    hideFeedback();
                }
            });
        });
    </script>
</body>
</html>
)rawliteral";

void changeWifiSettings(){
    preferences.begin("greenpulse", false);
    
    server.send(200, "text/plain", "Settings changed successfully.");
    
    String _ssid_ = server.arg("ssid");
    String _password_ = server.arg("password");
    Serial.printf("[UTILS] SSID changed to %s\n", _ssid_.c_str());
    preferences.putString("ssid", _ssid_);
    Serial.printf("[UTILS] Password changed to %s\n", _password_.c_str());
    preferences.putString("password", _password_);
    preferences.end();
}

void wifiSettingsPage(){
    server.send(200, "text/html", WIFI_SETUP_HTML);
}

void initializeWebServer(){
    // Initialize MDNS
    if (!MDNS.begin("greenpulse")) {
        Serial.println("Error setting up MDNS responder!");
    }
    
    // Initialize web server routes
    server.on("/", HTTP_GET, wifiSettingsPage);
    server.on("/processWifi", HTTP_POST, changeWifiSettings);
    server.on("/success", HTTP_GET, [](){
        Serial.println("[UTILS] Wifi settings processed successfully.");
        server.send(200, "text/html", WIFI_SUCCESS_HTML);
        restartESPWEB = true;
        lastESPWEBTime = millis();
    });
    
    // Start Webserver effective immediately
    server.begin();
}