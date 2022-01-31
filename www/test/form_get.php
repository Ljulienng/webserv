<?php 
    if (isset($_GET["message"]))
    {
        echo 'Merci pour ce message " ' . htmlspecialchars($_GET["message"]) . ' "';
    }
    else
    {
        echo 'Je crois que vous avez oublié de nous laisser un message !';
    }
?>