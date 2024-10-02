<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Basic CGI with PHP</title>
</head>
<body>

    <h1>Welcome to my PHP CGI Page</h1>
    <p>This is a basic CGI script using PHP to generate HTML output.</p>

    <!-- PHP code embedded inside HTML -->

    <p>Your IP address is: 
        <?php 
            // Display the visitor's IP address
            echo $_SERVER['REMOTE_ADDR']; 
        ?>
    </p>

</body>
</html>