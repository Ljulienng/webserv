<?php 
    if (isset($_POST["message"]))
    {
        echo 'Merci pour ce message' . htmlspecialchars($_POST["message"]) . '!';
    }
?>