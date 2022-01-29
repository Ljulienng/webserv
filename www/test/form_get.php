<?php 
    if (isset($_GET["message"]))
    {
        echo 'Merci pour ce message  ' . htmlspecialchars($_GET["message"]) . '!';
    }
?>