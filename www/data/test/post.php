<?php

    if (isset($_POST["Message"]))
    {
        $message = $_POST["Message"];
        print "<p>Merci pour ce beau message, je cite : \" $message\" </p>";
    }
    
?>