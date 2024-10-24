#!/usr/bin/php-cgi
<?php
// file_put_contents('log.txt', print_r($_GET, true));
if (isset($_GET['?username']) && isset($_GET['bio'])) {
    $username = htmlspecialchars($_GET['?username']);
    $bio = htmlspecialchars($_GET['bio']);
    echo '
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>User Profile</title>
    <style>
        body {
            font-family: "Arial", sans-serif;
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
            margin: 0;
            background: linear-gradient(to right, #3498db, #2ecc71);
            color: #fff;
            text-align: center;
        }

        h2 {
            font-size: 48px;
            margin-bottom: 10px;
        }

        h3 {
            font-size: 24px;
            margin-bottom: 40px;
            font-weight: 300;
            color: #f1c40f;
        }

        .button-container {
            display: flex;
            gap: 20px;
            margin-top: 20px;
        }

        button, .link-button {
            padding: 15px 30px;
            font-size: 18px;
            font-weight: bold;
            cursor: pointer;
            border: none;
            border-radius: 50px;
            transition: background-color 0.3s, transform 0.3s;
        }

        button {
            background-color: #e74c3c;
            color: white;
        }

        button:hover {
            background-color: #c0392b;
            transform: scale(1.1);
        }

        .link-button {
            background-color: #8e44ad;
            color: white;
            text-decoration: none;
            display: inline-block;
            line-height: 1.5;
        }

        .link-button:hover {
            background-color: #9b59b6;
            transform: scale(1.1);
        }

        /* Optional subtle animation */
        @keyframes fadeIn {
            from { opacity: 0; transform: translateY(10px); }
            to { opacity: 1; transform: translateY(0); }
        }

        h2, h3, .button-container {
            animation: fadeIn 1s ease-out;
        }

    </style>
</head>
<body>

    <h2>Your Username: ' . $username . '</h2>
    <h3>Bio: ' . $bio . '</h3>
    
    <div class="button-container">
        <button onclick="showMessage()">Show Message</button>
        <a href="/home/index.html" class="link-button">Go to Home</a>
        <a href="/bla/index2.html" class="link-button">Go to Page bla</a>
        <a href="/cgi/basicphp.html" class="link-button">Go to CGI for PHP</a>
        <a href="/cgi/basicpy.html" class="link-button">Go to CGI for PY</a>
    </div>

    <script>
        function showMessage() {
            alert("Hello, ' . $username . '! You clicked the button.");
        }
    </script>

</body>
</html>';
} else {
    echo '<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Error 404 - Page Not Found</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f2f2f2;
            color: #333;
            text-align: center;
            padding: 50px;
        }
        h1 {
            font-size: 72px;
            color: #e74c3c;
        }
        p {
            font-size: 24px;
            color: #555;
        }
        .error-container {
            max-width: 600px;
            margin: auto;
        }
        a {
            color: #3498db;
            text-decoration: none;
        }
        a:hover {
            text-decoration: underline;
        }
        .back-home {
            display: inline-block;
            padding: 10px 20px;
            background-color: #3498db;
            color: white;
            border-radius: 5px;
            margin-top: 20px;
        }
        .back-home:hover {
            background-color: #2980b9;
        }
    </style>
</head>
<body>
    <div class="error-container">
        <h1>404</h1>
        <p>Oops! The page you are looking for cannot be found.</p>
        <a href="/index.html" class="back-home">Go Back to Home</a>
    </div>
</body>
</html>';
    }
?>