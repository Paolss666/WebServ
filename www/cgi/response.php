<?php
if (isset($_GET['uri'])) {
    // Récupérer et assainir l'URI
    $uri = htmlspecialchars($_GET['uri']);
    
    // Générer la réponse dans une nouvelle page
    echo "<html><body>";
    echo "<h1>Résultat</h1>";
    echo "<p>L'URI que vous avez entré est : " . $uri . "</p>";
    echo "</body></html>";
} else {
    echo "Aucun URI fourni !";
}
?>