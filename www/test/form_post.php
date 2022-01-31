<?php 
    if (isset($_POST["message"]))
    {
        echo 'Merci pour ce message " ' . htmlspecialchars($_POST["message"]) . ' "';
    }
    else
    {
        echo 'Je crois que vous avez oublié de nous laisser un message !';
    }
?>