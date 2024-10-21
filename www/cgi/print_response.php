#!/usr/bin/php-cgi
<?php
// header("Content-Type: text/html; charset=UTF-8");
// file_put_contents('log.txt', print_r($_GET, true));
if (isset($_GET['?uri'])) {
    $uri = htmlspecialchars($_GET['?uri']);
    echo "<head>";
    echo ' <style>
        body {
            font-family: "Arial", sans-serif;
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #f4f7f6;
        }

        h1 {
            font-size: 48px;
            color: #333;
            margin-bottom: 20px;
        }

        p {
            font-size: 18px;
            color: #777;
            margin-bottom: 40px;
        }

        .button-container {
            display: flex;
            gap: 20px;
        }

        button, .link-button {
            padding: 15px 30px;
            font-size: 18px;
            cursor: pointer;
            border: none;
            border-radius: 50px;
            transition: background-color 0.3s, box-shadow 0.3s;
        }

        button {
            background-color: #3498db;
            color: white;
        }

        button:hover {
            background-color: #2980b9;
            box-shadow: 0px 4px 15px rgba(0, 0, 0, 0.1);
        }

        .link-button {
            background-color: #2ecc71;
            color: white;
            text-decoration: none;
            display: inline-block;
            line-height: 1.5;
        }

        .link-button:hover {
            background-color: #03eb63;
            box-shadow: 0px 4px 15px rgba(0, 0, 0, 0.1);
        }

    </style>';
    echo "</head>";
    echo '<html><body>';
    echo '<h1>Résultat is: ' . $uri . '</h1>';
    echo '<div class="button-container">';
    echo '<button onclick="showMessage()">Show Message</button>';
    echo '<a href="/home/index.html" class="link-button">Go to Page</a>';
    echo '<a href="/bla/index2.html" class="link-button">Go to Page bla</a>';
    echo '<a href="/cgi/basic.html" class="link-button">Go to CGI</a>';
    echo '</div>';
    echo '<script>
        function showMessage() {
            alert("Hello! You clicked the button.");
        }
    </script>';
    echo "</body></html>";
} else {
    echo "Aucun URI fourni !";
}
?>